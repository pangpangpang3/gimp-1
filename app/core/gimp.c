/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2002 Spencer Kimball, Peter Mattis, and others
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h> /* strlen */

#include <glib-object.h>

#include "libgimpbase/gimpbase.h"

#include "core-types.h"

#include "config/gimpconfig.h"
#include "config/gimpconfig-params.h"
#include "config/gimpconfig-path.h"
#include "config/gimprc.h"

#include "pdb/procedural_db.h"
#include "pdb/internal_procs.h"

#include "paint/paint.h"

#include "text/gimpfontlist.h"

#include "xcf/xcf.h"

#include "gimp.h"
#include "gimp-documents.h"
#include "gimp-gradients.h"
#include "gimp-parasites.h"
#include "gimpbrush.h"
#include "gimpbrushgenerated.h"
#include "gimpbrushpipe.h"
#include "gimpbuffer.h"
#include "gimpcontext.h"
#include "gimpdatafactory.h"
#include "gimpdocumentlist.h"
#include "gimpenvirontable.h"
#include "gimpgradient.h"
#include "gimpimage.h"
#include "gimpimage-new.h"
#include "gimpimagefile.h"
#include "gimplist.h"
#include "gimpmarshal.h"
#include "gimpmodules.h"
#include "gimppalette.h"
#include "gimppattern.h"
#include "gimpparasitelist.h"
#include "gimptoolinfo.h"
#include "gimpunits.h"

#include "gimp-intl.h"


enum
{
  EXIT,
  LAST_SIGNAL
};


static void       gimp_class_init           (GimpClass  *klass);
static void       gimp_init                 (Gimp       *gimp);

static void       gimp_dispose              (GObject    *object);
static void       gimp_finalize             (GObject    *object);

static gsize      gimp_get_memsize          (GimpObject *object);

static gboolean   gimp_real_exit            (Gimp       *gimp,
                                             gboolean    kill_it);

static void       gimp_global_config_notify (GObject    *global_config,
                                             GParamSpec *param_spec,
                                             GObject    *edit_config);
static void       gimp_edit_config_notify   (GObject    *edit_config,
                                             GParamSpec *param_spec,
                                             GObject    *global_config);


static GimpObjectClass *parent_class = NULL;

static guint gimp_signals[LAST_SIGNAL] = { 0, };


GType 
gimp_get_type (void)
{
  static GType object_type = 0;

  if (! object_type)
    {
      static const GTypeInfo object_info =
      {
        sizeof (GimpClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data     */
	sizeof (Gimp),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_init,
      };

      object_type = g_type_register_static (GIMP_TYPE_OBJECT,
					    "Gimp", 
					    &object_info, 0);
    }

  return object_type;
}

gboolean
gimp_boolean_handled_accumulator (GSignalInvocationHint *ihint,
                                  GValue                *return_accu,
                                  const GValue          *handler_return,
                                  gpointer               dummy)
{
  gboolean continue_emission;
  gboolean signal_handled;

  signal_handled = g_value_get_boolean (handler_return);
  g_value_set_boolean (return_accu, signal_handled);
  continue_emission = ! signal_handled;

  return continue_emission;
}

static void
gimp_class_init (GimpClass *klass)
{
  GObjectClass    *object_class;
  GimpObjectClass *gimp_object_class;

  object_class      = G_OBJECT_CLASS (klass);
  gimp_object_class = GIMP_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  gimp_signals[EXIT] =
    g_signal_new ("exit",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GimpClass, exit),
                  gimp_boolean_handled_accumulator, NULL,
                  gimp_marshal_BOOLEAN__BOOLEAN,
                  G_TYPE_BOOLEAN, 1,
                  G_TYPE_BOOLEAN);

  object_class->dispose          = gimp_dispose;
  object_class->finalize         = gimp_finalize;

  gimp_object_class->get_memsize = gimp_get_memsize;

  klass->exit                    = gimp_real_exit;
}

static void
gimp_init (Gimp *gimp)
{
  gimp->config                  = NULL;

  gimp->be_verbose              = FALSE;
  gimp->no_data                 = FALSE;
  gimp->no_interface            = FALSE;
  gimp->use_shm                 = FALSE;
  gimp->message_handler         = GIMP_CONSOLE;
  gimp->stack_trace_mode        = GIMP_STACK_TRACE_NEVER;

  gimp->gui_threads_enter_func  = NULL;
  gimp->gui_threads_leave_func  = NULL;
  gimp->gui_create_display_func = NULL;
  gimp->gui_set_busy_func       = NULL;
  gimp->gui_unset_busy_func     = NULL;
  gimp->gui_message_func        = NULL;

  gimp->busy                    = 0;
  gimp->busy_idle_id            = 0;

  gimp_units_init (gimp);

  gimp->parasites               = gimp_parasite_list_new ();

  paint_init (gimp);

  gimp_modules_init (gimp);
 
  gimp->environ_table       = gimp_environ_table_new (); 

  gimp->plug_in_debug       = NULL;

  gimp->images              = gimp_list_new (GIMP_TYPE_IMAGE,
					     GIMP_CONTAINER_POLICY_WEAK);
  gimp_object_set_name (GIMP_OBJECT (gimp->images), "images");

  gimp->next_image_ID       = 1;
  gimp->next_guide_ID       = 1;
  gimp->image_table         = g_hash_table_new (g_direct_hash, NULL);

  gimp->next_item_ID        = 1;
  gimp->item_table          = g_hash_table_new (g_direct_hash, NULL);

  gimp->displays            = gimp_list_new (GIMP_TYPE_OBJECT,
                                             GIMP_CONTAINER_POLICY_WEAK);
  gimp_object_set_name (GIMP_OBJECT (gimp->displays), "displays");

  gimp->next_display_ID     = 1;

  gimp->global_buffer       = NULL;
  gimp->named_buffers       = gimp_list_new (GIMP_TYPE_BUFFER,
					     GIMP_CONTAINER_POLICY_STRONG);
  gimp_object_set_name (GIMP_OBJECT (gimp->named_buffers), "named buffers");

  gimp->fonts               = gimp_font_list_new (72.0, 72.0);
  gimp_object_set_name (GIMP_OBJECT (gimp->fonts), "fonts");

  gimp->brush_factory       = NULL;
  gimp->pattern_factory     = NULL;
  gimp->gradient_factory    = NULL;
  gimp->palette_factory     = NULL;

  procedural_db_init (gimp);

  gimp->load_procs          = NULL;
  gimp->save_procs          = NULL;

  xcf_init (gimp);

  gimp->tool_info_list      = gimp_list_new (GIMP_TYPE_TOOL_INFO,
					     GIMP_CONTAINER_POLICY_STRONG);
  gimp_object_set_name (GIMP_OBJECT (gimp->tool_info_list), "tool infos");

  gimp->standard_tool_info  = NULL;

  gimp->documents           = gimp_document_list_new (gimp);

  gimp->have_current_cut_buffer = FALSE;

  gimp->context_list            = NULL;
  gimp->standard_context        = NULL;
  gimp->default_context         = NULL;
  gimp->user_context            = NULL;
  gimp->current_context         = NULL;
}

static void
gimp_dispose (GObject *object)
{
  Gimp *gimp;

  gimp = GIMP (object);

  if (gimp->brush_factory)
    gimp_data_factory_data_free (gimp->brush_factory);

  if (gimp->pattern_factory)
    gimp_data_factory_data_free (gimp->pattern_factory);

  if (gimp->gradient_factory)
    gimp_data_factory_data_free (gimp->gradient_factory);

  if (gimp->palette_factory)
    gimp_data_factory_data_free (gimp->palette_factory);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gimp_finalize (GObject *object)
{
  Gimp *gimp;

  gimp = GIMP (object);

  gimp_set_current_context (gimp, NULL);
  gimp_set_user_context (gimp, NULL);
  gimp_set_default_context (gimp, NULL);

  if (gimp->standard_context)
    {
      g_object_unref (gimp->standard_context);
      gimp->standard_context = NULL;
    }

  if (gimp->documents)
    {
      g_object_unref (gimp->documents);
      gimp->documents = NULL;
    }

  gimp_tool_info_set_standard (gimp, NULL);

  if (gimp->tool_info_list)
    {
      g_object_unref (gimp->tool_info_list);
      gimp->tool_info_list = NULL;
    }

  xcf_exit (gimp);

  procedural_db_free (gimp);

  if (gimp->load_procs)
    {
      g_slist_free (gimp->load_procs);
      gimp->load_procs = NULL;
    }

  if (gimp->save_procs)
    {
      g_slist_free (gimp->save_procs);
      gimp->save_procs = NULL;
    }

  if (gimp->brush_factory)
    {
      g_object_unref (gimp->brush_factory);
      gimp->brush_factory = NULL;
    }

  if (gimp->pattern_factory)
    {
      g_object_unref (gimp->pattern_factory);
      gimp->pattern_factory = NULL;
    }

  if (gimp->gradient_factory)
    {
      g_object_unref (gimp->gradient_factory);
      gimp->gradient_factory = NULL;
    }

  if (gimp->palette_factory)
    {
      g_object_unref (gimp->palette_factory);
      gimp->palette_factory = NULL;
    }

  if (gimp->fonts)
    {
      g_object_unref (gimp->fonts);
      gimp->fonts = NULL;
    }

  if (gimp->named_buffers)
    {
      g_object_unref (gimp->named_buffers);
      gimp->named_buffers = NULL;
    }

  if (gimp->global_buffer)
    {
      g_object_unref (gimp->global_buffer);
      gimp->global_buffer = NULL;
    }

  if (gimp->displays)
    {
      g_object_unref (gimp->displays);
      gimp->displays = NULL;
    }

  if (gimp->item_table)
    {
      g_hash_table_destroy (gimp->item_table);
      gimp->item_table = NULL;
    }

  if (gimp->image_table)
    {
      g_hash_table_destroy (gimp->image_table);
      gimp->image_table = NULL;
    }

  if (gimp->images)
    {
      g_object_unref (gimp->images);
      gimp->images = NULL;
    }

  if (gimp->environ_table)
    {
      g_object_unref (gimp->environ_table);
      gimp->environ_table = NULL;
    }

  if (gimp->module_db)
    gimp_modules_exit (gimp);

  paint_exit (gimp);

  if (gimp->parasites)
    {
      g_object_unref (gimp->parasites);
      gimp->parasites = NULL;
    }

  if (gimp->edit_config)
    {
      g_object_unref (gimp->edit_config);
      gimp->edit_config = NULL;
    }

  if (gimp->user_units)
    gimp_units_exit (gimp);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gsize
gimp_get_memsize (GimpObject *object)
{
  Gimp  *gimp;
  gsize  memsize = 0;

  gimp = GIMP (object);

  memsize += g_list_length (gimp->user_units) * sizeof (GList); /* FIXME */

  memsize += gimp_object_get_memsize (GIMP_OBJECT (gimp->parasites));

  memsize += gimp_g_object_get_memsize (G_OBJECT (gimp->module_db));

  memsize += (g_hash_table_size (gimp->image_table) *
              3 * sizeof (gpointer)); /* FIXME */
  memsize += (g_hash_table_size (gimp->item_table) *
              3 * sizeof (gpointer)); /* FIXME */

  memsize += gimp_object_get_memsize (GIMP_OBJECT (gimp->displays));

  if (gimp->global_buffer)
    memsize += gimp_object_get_memsize (GIMP_OBJECT (gimp->global_buffer));

  memsize += (gimp_object_get_memsize (GIMP_OBJECT (gimp->named_buffers)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->fonts)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->brush_factory)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->pattern_factory)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->gradient_factory)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->palette_factory)));

  memsize += (g_hash_table_size (gimp->procedural_ht) *
              3 * sizeof (gpointer)); /* FIXME */

  memsize += (g_list_length (gimp->procedural_db_data_list) *
              sizeof (GList)); /* FIXME */

  memsize += g_slist_length (gimp->load_procs) * sizeof (GSList); /* FIXME */
  memsize += g_slist_length (gimp->save_procs) * sizeof (GSList); /* FIXME */

  memsize += (gimp_object_get_memsize (GIMP_OBJECT (gimp->tool_info_list)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->standard_tool_info)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->documents)));

  memsize += g_list_length (gimp->context_list) * sizeof (GList);

  memsize += (gimp_object_get_memsize (GIMP_OBJECT (gimp->standard_context)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->default_context)) +
              gimp_object_get_memsize (GIMP_OBJECT (gimp->user_context)));

  return memsize + GIMP_OBJECT_CLASS (parent_class)->get_memsize (object);
}

static gboolean
gimp_real_exit (Gimp     *gimp,
                gboolean  kill_it)
{
  gimp_modules_unload (gimp);
  gimp_data_factory_data_save (gimp->brush_factory);
  gimp_data_factory_data_save (gimp->pattern_factory);
  gimp_data_factory_data_save (gimp->gradient_factory);
  gimp_data_factory_data_save (gimp->palette_factory);
  gimp_documents_save (gimp);
  gimp_parasiterc_save (gimp);
  gimp_unitrc_save (gimp);

  return FALSE; /* continue exiting */
}

Gimp *
gimp_new (gboolean           be_verbose,
          gboolean           no_data,
          gboolean           no_interface,
          gboolean           use_shm,
          GimpStackTraceMode stack_trace_mode)
{
  Gimp *gimp;

  gimp = g_object_new (GIMP_TYPE_GIMP, NULL);

  gimp->be_verbose       = be_verbose   ? TRUE : FALSE;
  gimp->no_data          = no_data      ? TRUE : FALSE;
  gimp->no_interface     = no_interface ? TRUE : FALSE;
  gimp->use_shm          = use_shm      ? TRUE : FALSE;
  gimp->stack_trace_mode = stack_trace_mode;

  return gimp;
}

static void
gimp_global_config_notify (GObject    *global_config,
                           GParamSpec *param_spec,
                           GObject    *edit_config)
{
  GValue global_value = { 0, };
  GValue edit_value   = { 0, };

  g_value_init (&global_value, param_spec->value_type);
  g_value_init (&edit_value,   param_spec->value_type);

  g_object_get_property (global_config, param_spec->name, &global_value);
  g_object_get_property (edit_config,   param_spec->name, &edit_value);

  if (g_param_values_cmp (param_spec, &global_value, &edit_value))
    {
      g_signal_handlers_block_by_func (edit_config,
                                       gimp_edit_config_notify,
                                       global_config);

      g_object_set_property (edit_config, param_spec->name, &global_value);

      g_signal_handlers_unblock_by_func (edit_config,
                                         gimp_edit_config_notify,
                                         global_config);
    }

  g_value_unset (&global_value);
  g_value_unset (&edit_value);
}

static void
gimp_edit_config_notify (GObject    *edit_config,
                         GParamSpec *param_spec,
                         GObject    *global_config)
{
  GValue edit_value   = { 0, };
  GValue global_value = { 0, };

  g_value_init (&edit_value,   param_spec->value_type);
  g_value_init (&global_value, param_spec->value_type);

  g_object_get_property (edit_config,   param_spec->name, &edit_value);
  g_object_get_property (global_config, param_spec->name, &global_value);

  if (g_param_values_cmp (param_spec, &edit_value, &global_value))
    {
      if (param_spec->flags & GIMP_PARAM_RESTART)
        {
#ifdef GIMP_CONFIG_DEBUG
          g_print ("NOT Applying edit_config change of '%s' to global_config "
                   "because it needs restart\n",
                   param_spec->name);
#endif
        }
      else
        {
#ifdef GIMP_CONFIG_DEBUG
          g_print ("Applying edit_config change of '%s' to global_config\n",
                   param_spec->name);
#endif
          g_signal_handlers_block_by_func (global_config,
                                           gimp_global_config_notify,
                                           edit_config);

          g_object_set_property (global_config, param_spec->name, &edit_value);

          g_signal_handlers_unblock_by_func (global_config,
                                             gimp_global_config_notify,
                                             edit_config);
        }
    }

  g_value_unset (&edit_value);
  g_value_unset (&global_value);
}

void
gimp_set_config (Gimp           *gimp,
                 GimpCoreConfig *core_config)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (GIMP_IS_CORE_CONFIG (core_config));
  g_return_if_fail (gimp->config == NULL);
  g_return_if_fail (gimp->edit_config == NULL);

  gimp->config = g_object_ref (core_config);

  gimp->edit_config =
    GIMP_CORE_CONFIG (gimp_config_duplicate (G_OBJECT (gimp->config)));

  g_signal_connect_object (gimp->config, "notify",
                           G_CALLBACK (gimp_global_config_notify),
                           gimp->edit_config, 0);
  g_signal_connect_object (gimp->edit_config, "notify",
                           G_CALLBACK (gimp_edit_config_notify),
                           gimp->config, 0);
}

void
gimp_initialize (Gimp               *gimp,
                 GimpInitStatusFunc  status_callback)
{
  GimpContext *context;
  gchar       *path;

  static const GimpDataFactoryLoaderEntry brush_loader_entries[] =
  {
    { gimp_brush_load,           GIMP_BRUSH_FILE_EXTENSION           },
    { gimp_brush_load,           GIMP_BRUSH_PIXMAP_FILE_EXTENSION    },
    { gimp_brush_generated_load, GIMP_BRUSH_GENERATED_FILE_EXTENSION },
    { gimp_brush_pipe_load,      GIMP_BRUSH_PIPE_FILE_EXTENSION      }
  };

  static const GimpDataFactoryLoaderEntry pattern_loader_entries[] =
  {
    { gimp_pattern_load, GIMP_PATTERN_FILE_EXTENSION }
  };

  static const GimpDataFactoryLoaderEntry gradient_loader_entries[] =
  {
    { gimp_gradient_load, GIMP_GRADIENT_FILE_EXTENSION },
    { gimp_gradient_load, NULL /* legacy loader */     }
  };

  static const GimpDataFactoryLoaderEntry palette_loader_entries[] =
  {
    { gimp_palette_load, GIMP_PALETTE_FILE_EXTENSION },
    { gimp_palette_load, NULL /* legacy loader */    }
  };

  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (status_callback != NULL);
  g_return_if_fail (GIMP_IS_CORE_CONFIG (gimp->config));

  gimp->brush_factory =
    gimp_data_factory_new (gimp,
                           GIMP_TYPE_BRUSH,
			   "brush_path",
			   brush_loader_entries,
			   G_N_ELEMENTS (brush_loader_entries),
			   gimp_brush_new,
			   gimp_brush_get_standard);
  gimp_object_set_name (GIMP_OBJECT (gimp->brush_factory), "brush factory");

  gimp->pattern_factory =
    gimp_data_factory_new (gimp,
                           GIMP_TYPE_PATTERN,
			   "pattern_path",
			   pattern_loader_entries,
			   G_N_ELEMENTS (pattern_loader_entries),
			   gimp_pattern_new,
			   gimp_pattern_get_standard);
  gimp_object_set_name (GIMP_OBJECT (gimp->pattern_factory), "pattern factory");

  gimp->gradient_factory =
    gimp_data_factory_new (gimp,
                           GIMP_TYPE_GRADIENT,
			   "gradient_path",
			   gradient_loader_entries,
			   G_N_ELEMENTS (gradient_loader_entries),
			   gimp_gradient_new,
			   gimp_gradient_get_standard);
  gimp_object_set_name (GIMP_OBJECT (gimp->gradient_factory), "gradient factory");

  gimp->palette_factory =
    gimp_data_factory_new (gimp,
                           GIMP_TYPE_PALETTE,
			   "palette_path",
			   palette_loader_entries,
			   G_N_ELEMENTS (palette_loader_entries),
			   gimp_palette_new,
			   gimp_palette_get_standard);
  gimp_object_set_name (GIMP_OBJECT (gimp->palette_factory), "palette factory");

  /* Set the last values used to default values. */

  gimp->image_new_last_values.width       = gimp->config->default_image_width;
  gimp->image_new_last_values.height      = gimp->config->default_image_height;
  gimp->image_new_last_values.unit        = gimp->config->default_unit;
  gimp->image_new_last_values.xresolution = gimp->config->default_xresolution;
  gimp->image_new_last_values.yresolution = gimp->config->default_yresolution;
  gimp->image_new_last_values.res_unit    = gimp->config->default_resolution_unit;
  gimp->image_new_last_values.type        = gimp->config->default_image_type;
  gimp->image_new_last_values.fill_type   = GIMP_BACKGROUND_FILL;

  gimp->have_current_cut_buffer = FALSE;

  gimp->standard_context = gimp_context_new (gimp, "Standard", NULL);

  /*  the default context contains the user's saved preferences
   *
   *  TODO: load from disk
   */
  context = gimp_context_new (gimp, "Default", NULL);
  gimp_set_default_context (gimp, context);
  g_object_unref (context);

  /*  the initial user_context is a straight copy of the default context
   */
  context = gimp_context_new (gimp, "User", context);
  gimp_set_user_context (gimp, context);
  g_object_unref (context);

  gimp_set_current_context (gimp, context);

  /*  add the builtin FG -> BG etc. gradients  */
  gimp_gradients_init (gimp);

  /*  register all internal procedures  */
  (* status_callback) (_("Procedural Database"), NULL, -1);
  internal_procs_init (gimp, status_callback);

  (* status_callback) (_("Plug-In Environment"), "", -1);

  path = gimp_config_path_expand (gimp->config->environ_path, TRUE, NULL);
  gimp_environ_table_load (gimp->environ_table, path);
  g_free (path);
}

void
gimp_restore (Gimp               *gimp,
              GimpInitStatusFunc  status_callback,
	      gboolean            no_data)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (status_callback != NULL);

  /*  initialize  the global parasite table  */
  (* status_callback) (_("Looking for data files"), _("Parasites"), 0.00);
  gimp_parasiterc_load (gimp);

  /*  initialize the list of gimp brushes    */
  (* status_callback) (NULL, _("Brushes"), 0.15);
  gimp_data_factory_data_init (gimp->brush_factory, no_data); 

  /*  initialize the list of gimp patterns   */
  (* status_callback) (NULL, _("Patterns"), 0.30);
  gimp_data_factory_data_init (gimp->pattern_factory, no_data); 

  /*  initialize the list of gimp palettes   */
  (* status_callback) (NULL, _("Palettes"), 0.45);
  gimp_data_factory_data_init (gimp->palette_factory, no_data); 

  /*  initialize the list of gimp gradients  */
  (* status_callback) (NULL, _("Gradients"), 0.60);
  gimp_data_factory_data_init (gimp->gradient_factory, no_data); 

  /*  initialize the list of gimp fonts  */
  (* status_callback) (NULL, _("Fonts"), 0.75);
  gimp_container_freeze (gimp->fonts);
  gimp_font_list_restore (GIMP_FONT_LIST (gimp->fonts));
  gimp_container_thaw (gimp->fonts);

  /*  initialize the document history  */
  (* status_callback) (NULL, _("Documents"), 0.90);
  gimp_documents_load (gimp);

  (* status_callback) (NULL, NULL, 1.00);

  gimp_modules_load (gimp);
}

void
gimp_exit (Gimp     *gimp,
           gboolean  kill_it)
{
  gboolean handled;

  g_return_if_fail (GIMP_IS_GIMP (gimp));

  g_signal_emit (gimp, gimp_signals[EXIT], 0,
                 kill_it ? TRUE : FALSE,
                 &handled);
}

void
gimp_threads_enter (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (gimp->gui_threads_enter_func)
    gimp->gui_threads_enter_func (gimp);
}

void
gimp_threads_leave (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (gimp->gui_threads_leave_func)
    gimp->gui_threads_leave_func (gimp);
}

void
gimp_set_busy (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  /* FIXME: gimp_busy HACK */
  gimp->busy++;

  if (gimp->busy == 1)
    {
      if (gimp->gui_set_busy_func)
        gimp->gui_set_busy_func (gimp);
    }
}

static gboolean
gimp_idle_unset_busy (gpointer data)
{
  Gimp *gimp;

  gimp = (Gimp *) data;

  gimp_unset_busy (gimp);

  gimp->busy_idle_id = 0;

  return FALSE;
}

void
gimp_set_busy_until_idle (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (! gimp->busy_idle_id)
    {
      gimp_set_busy (gimp);

      gimp->busy_idle_id = g_idle_add_full (G_PRIORITY_HIGH,
					    gimp_idle_unset_busy, gimp,
					    NULL);
    }
}

void
gimp_unset_busy (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (gimp->busy > 0);

  /* FIXME: gimp_busy HACK */
  gimp->busy--;

  if (gimp->busy == 0)
    {
      if (gimp->gui_unset_busy_func)
        gimp->gui_unset_busy_func (gimp);
    }
}

void
gimp_message (Gimp        *gimp,
              const gchar *message)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  switch (gimp->message_handler)
    {
    case GIMP_MESSAGE_BOX:
    case GIMP_ERROR_CONSOLE:
      if (gimp->gui_message_func)
        {
          gimp->gui_message_func (gimp, message);
          return;
        }
      break;

    default:
      break;
    }

  g_printerr ("%s: %s\n", GIMP_OBJECT (gimp)->name, message);
}

GimpImage *
gimp_create_image (Gimp              *gimp,
		   gint               width,
		   gint               height,
		   GimpImageBaseType  type,
		   gboolean           attach_comment)
{
  GimpImage *gimage;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  gimage = gimp_image_new (gimp, width, height, type);

  gimp_container_add (gimp->images, GIMP_OBJECT (gimage));

  if (attach_comment && gimp->config->default_comment)
    {
      GimpParasite *parasite;

      parasite = gimp_parasite_new ("gimp-comment",
				    GIMP_PARASITE_PERSISTENT,
				    strlen (gimp->config->default_comment) + 1,
				    gimp->config->default_comment);
      gimp_image_parasite_attach (gimage, parasite);
      gimp_parasite_free (parasite);
    }

  return gimage;
}

GimpObject *
gimp_create_display (Gimp      *gimp,
		     GimpImage *gimage,
                     guint      scale)
{
  GimpObject *display = NULL;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (GIMP_IS_IMAGE (gimage), NULL);

  if (gimp->gui_create_display_func)
    {
      display = gimp->gui_create_display_func (gimage, scale);

      gimp_container_add (gimp->displays, display);
    }

  return display;
}

GimpContext *
gimp_get_standard_context (Gimp *gimp)
{
  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  return gimp->standard_context;
}

void
gimp_set_default_context (Gimp        *gimp,
			  GimpContext *context)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (! context || GIMP_IS_CONTEXT (context));

  if (gimp->default_context)
    {
      g_object_unref (gimp->default_context);
    }

  gimp->default_context = context;

  if (gimp->default_context)
    {
      g_object_ref (gimp->default_context);
    }
}

GimpContext *
gimp_get_default_context (Gimp *gimp)
{
  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  return gimp->default_context;
}

void
gimp_set_user_context (Gimp        *gimp,
		       GimpContext *context)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (! context || GIMP_IS_CONTEXT (context));

  if (gimp->user_context)
    {
      g_object_unref (gimp->user_context);
    }

  gimp->user_context = context;

  if (gimp->user_context)
    {
      g_object_ref (gimp->user_context);
    }
}

GimpContext *
gimp_get_user_context (Gimp *gimp)
{
  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  return gimp->user_context;
}

void
gimp_set_current_context (Gimp        *gimp,
			  GimpContext *context)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (! context || GIMP_IS_CONTEXT (context));

  if (gimp->current_context)
    {
      g_object_unref (gimp->current_context);
    }

  gimp->current_context = context;

  if (gimp->current_context)
    {
      g_object_ref (gimp->current_context);
    }
}

GimpContext *
gimp_get_current_context (Gimp *gimp)
{
  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);

  return gimp->current_context;
}
