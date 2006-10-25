/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"


#include <glib-object.h>

#include "pdb-types.h"
#include "gimppdb.h"
#include "gimpprocedure.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "core/gimpimage-undo.h"
#include "core/gimpimage.h"
#include "plug-in/gimpplugin-cleanup.h"
#include "plug-in/gimpplugin.h"
#include "plug-in/gimppluginmanager.h"


static GValueArray *
image_undo_group_start_invoker (GimpProcedure     *procedure,
                                Gimp              *gimp,
                                GimpContext       *context,
                                GimpProgress      *progress,
                                const GValueArray *args)
{
  gboolean success = TRUE;
  GimpImage *image;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
      GimpPlugIn *plug_in   = gimp->plug_in_manager->current_plug_in;
      gchar      *undo_desc = NULL;

      if (plug_in)
        {
          success = gimp_plug_in_cleanup_undo_group_start (plug_in, image);

          if (success)
            undo_desc = gimp_plug_in_get_undo_desc (plug_in);
        }

      if (success)
        gimp_image_undo_group_start (image, GIMP_UNDO_GROUP_MISC, undo_desc);

      if (undo_desc)
        g_free (undo_desc);
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
image_undo_group_end_invoker (GimpProcedure     *procedure,
                              Gimp              *gimp,
                              GimpContext       *context,
                              GimpProgress      *progress,
                              const GValueArray *args)
{
  gboolean success = TRUE;
  GimpImage *image;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
      GimpPlugIn *plug_in = gimp->plug_in_manager->current_plug_in;

      if (plug_in)
        success = gimp_plug_in_cleanup_undo_group_end (plug_in, image);

      if (success)
        gimp_image_undo_group_end (image);
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
image_undo_is_enabled_invoker (GimpProcedure     *procedure,
                               Gimp              *gimp,
                               GimpContext       *context,
                               GimpProgress      *progress,
                               const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  GimpImage *image;
  gboolean enabled = FALSE;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
      enabled = gimp_image_undo_is_enabled (image);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_boolean (&return_vals->values[1], enabled);

  return return_vals;
}

static GValueArray *
image_undo_disable_invoker (GimpProcedure     *procedure,
                            Gimp              *gimp,
                            GimpContext       *context,
                            GimpProgress      *progress,
                            const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  GimpImage *image;
  gboolean disabled = FALSE;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
    #if 0
      GimpPlugIn *plug_in = gimp->plug_in_manager->current_plug_in;

      if (plug_in)
        success = gimp_plug_in_cleanup_undo_disable (plug_in, image);
    #endif

      if (success)
        disabled = gimp_image_undo_disable (image);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_boolean (&return_vals->values[1], disabled);

  return return_vals;
}

static GValueArray *
image_undo_enable_invoker (GimpProcedure     *procedure,
                           Gimp              *gimp,
                           GimpContext       *context,
                           GimpProgress      *progress,
                           const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  GimpImage *image;
  gboolean enabled = FALSE;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
    #if 0
      GimpPlugIn *plug_in = gimp->plug_in_manager->current_plug_in;

      if (plug_in)
        success = gimp_plug_in_cleanup_undo_enable (plug_in, image);
    #endif

      if (success)
        enabled = gimp_image_undo_enable (image);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_boolean (&return_vals->values[1], enabled);

  return return_vals;
}

static GValueArray *
image_undo_freeze_invoker (GimpProcedure     *procedure,
                           Gimp              *gimp,
                           GimpContext       *context,
                           GimpProgress      *progress,
                           const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  GimpImage *image;
  gboolean frozen = FALSE;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
    #if 0
      GimpPlugIn *plug_in = gimp->plug_in_manager->current_plug_in;

      if (plug_in)
        success = gimp_plug_in_cleanup_undo_freeze (plug_in, image);
    #endif

      if (success)
        frozen = gimp_image_undo_freeze (image);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_boolean (&return_vals->values[1], frozen);

  return return_vals;
}

static GValueArray *
image_undo_thaw_invoker (GimpProcedure     *procedure,
                         Gimp              *gimp,
                         GimpContext       *context,
                         GimpProgress      *progress,
                         const GValueArray *args)
{
  gboolean success = TRUE;
  GValueArray *return_vals;
  GimpImage *image;
  gboolean thawed = FALSE;

  image = gimp_value_get_image (&args->values[0], gimp);

  if (success)
    {
    #if 0
      GimpPlugIn *plug_in = gimp->plug_in_manager->current_plug_in;

      if (plug_in)
        success = gimp_plug_in_cleanup_undo_thaw (plug_in, image);
    #endif

      if (success)
        thawed = gimp_image_undo_thaw (image);
    }

  return_vals = gimp_procedure_get_return_values (procedure, success);

  if (success)
    g_value_set_boolean (&return_vals->values[1], thawed);

  return return_vals;
}

void
register_undo_procs (GimpPDB *pdb)
{
  GimpProcedure *procedure;

  /*
   * gimp-image-undo-group-start
   */
  procedure = gimp_procedure_new (image_undo_group_start_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-group-start");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-group-start",
                                     "Starts a group undo.",
                                     "This function is used to start a group undo--necessary for logically combining two or more undo operations into a single operation. This call must be used in conjunction with a 'gimp-image-undo-group-end' call.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1997",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The ID of the image in which to open an undo group",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-group-end
   */
  procedure = gimp_procedure_new (image_undo_group_end_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-group-end");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-group-end",
                                     "Finish a group undo.",
                                     "This function must be called once for each 'gimp-image-undo-group-start' call that is made.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1997",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The ID of the image in which to close an undo group",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-is-enabled
   */
  procedure = gimp_procedure_new (image_undo_is_enabled_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-is-enabled");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-is-enabled",
                                     "Check if the image's undo stack is enabled.",
                                     "This procedure checks if the image's undo stack is currently enabled or disabled. This is useful when several plugins or scripts call each other and want to check if their caller has already used 'gimp-image-undo-disable' or 'gimp-image-undo-freeze'.",
                                     "Rapha\xc3\xabl Quinet <raphael@gimp.org>",
                                     "Rapha\xc3\xabl Quinet",
                                     "1999",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The image",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   g_param_spec_boolean ("enabled",
                                                         "enabled",
                                                         "TRUE if undo is enabled for this image",
                                                         FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-disable
   */
  procedure = gimp_procedure_new (image_undo_disable_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-disable");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-disable",
                                     "Disable the image's undo stack.",
                                     "This procedure disables the image's undo stack, allowing subsequent operations to ignore their undo steps. This is generally called in conjunction with 'gimp-image-undo-enable' to temporarily disable an image undo stack. This is advantageous because saving undo steps can be time and memory intensive.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The image",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   g_param_spec_boolean ("disabled",
                                                         "disabled",
                                                         "TRUE if the image undo has been disabled",
                                                         FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-enable
   */
  procedure = gimp_procedure_new (image_undo_enable_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-enable");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-enable",
                                     "Enable the image's undo stack.",
                                     "This procedure enables the image's undo stack, allowing subsequent operations to store their undo steps. This is generally called in conjunction with 'gimp-image-undo-disable' to temporarily disable an image undo stack.",
                                     "Spencer Kimball & Peter Mattis",
                                     "Spencer Kimball & Peter Mattis",
                                     "1995-1996",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The image",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   g_param_spec_boolean ("enabled",
                                                         "enabled",
                                                         "TRUE if the image undo has been enabled",
                                                         FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-freeze
   */
  procedure = gimp_procedure_new (image_undo_freeze_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-freeze");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-freeze",
                                     "Freeze the image's undo stack.",
                                     "This procedure freezes the image's undo stack, allowing subsequent operations to ignore their undo steps. This is generally called in conjunction with 'gimp-image-undo-thaw' to temporarily disable an image undo stack. This is advantageous because saving undo steps can be time and memory intensive. 'gimp-image-undo-freeze' / 'gimp-image-undo-thaw' and 'gimp-image-undo-disable' / 'gimp-image-undo-enable' differ in that the former does not free up all undo steps when undo is thawed, so is more suited to interactive in-situ previews. It is important in this case that the image is back to the same state it was frozen in before thawing, else 'undo' behaviour is undefined.",
                                     "Adam D. Moss",
                                     "Adam D. Moss",
                                     "1999",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The image",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   g_param_spec_boolean ("frozen",
                                                         "frozen",
                                                         "TRUE if the image undo has been frozen",
                                                         FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-image-undo-thaw
   */
  procedure = gimp_procedure_new (image_undo_thaw_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-image-undo-thaw");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-image-undo-thaw",
                                     "Thaw the image's undo stack.",
                                     "This procedure thaws the image's undo stack, allowing subsequent operations to store their undo steps. This is generally called in conjunction with 'gimp-image-undo-freeze' to temporarily freeze an image undo stack. 'gimp-image-undo-thaw' does NOT free the undo stack as 'gimp-image-undo-enable' does, so is suited for situations where one wishes to leave the undo stack in the same state in which one found it despite non-destructively playing with the image in the meantime. An example would be in-situ plugin previews. Balancing freezes and thaws and ensuring image consistancy is the responsibility of the caller.",
                                     "Adam D. Moss",
                                     "Adam D. Moss",
                                     "1999",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_image_id ("image",
                                                         "image",
                                                         "The image",
                                                         pdb->gimp, FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_procedure_add_return_value (procedure,
                                   g_param_spec_boolean ("thawed",
                                                         "thawed",
                                                         "TRUE if the image undo has been thawed",
                                                         FALSE,
                                                         GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);
}
