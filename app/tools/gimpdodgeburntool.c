/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
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

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "core/gimptoolinfo.h"

#include "paint/gimpdodgeburn.h"

#include "gimpdodgeburntool.h"
#include "paint_options.h"

#include "libgimp/gimpintl.h"


static void   gimp_dodgeburn_tool_class_init (GimpDodgeBurnToolClass *klass);
static void   gimp_dodgeburn_tool_init       (GimpDodgeBurnTool      *dodgeburn);

static void   gimp_dodgeburn_tool_modifier_key  (GimpTool        *tool,
                                                 GdkModifierType  key,
                                                 gboolean         press,
                                                 GdkModifierType  state,
                                                 GimpDisplay     *gdisp);
static void   gimp_dodgeburn_tool_cursor_update (GimpTool        *tool,
                                                 GimpCoords      *coords,
                                                 GdkModifierType  state,
                                                 GimpDisplay     *gdisp);

static GimpToolOptions * gimp_dodgeburn_tool_options_new   (GimpToolInfo    *tool_info);
static void              gimp_dodgeburn_tool_options_reset (GimpToolOptions *tool_options);


static GimpPaintToolClass *parent_class = NULL;


void
gimp_dodgeburn_tool_register (Gimp                     *gimp,
                              GimpToolRegisterCallback  callback)
{
  (* callback) (gimp,
                GIMP_TYPE_DODGEBURN_TOOL,
                gimp_dodgeburn_tool_options_new,
                TRUE,
                "gimp:dodgeburn_tool",
                _("Dodge/Burn"),
                _("Dodge or Burn strokes"),
                N_("/Tools/Paint Tools/DodgeBurn"), "<shift>D",
                NULL, "tools/dodgeburn.html",
                GIMP_STOCK_TOOL_DODGE);
}

GType
gimp_dodgeburn_tool_get_type (void)
{
  static GType tool_type = 0;

  if (!tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpDodgeBurnToolClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_dodgeburn_tool_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpDodgeBurnTool),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_dodgeburn_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_PAINT_TOOL,
					  "GimpDodgeBurnTool",
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_dodgeburn_tool_class_init (GimpDodgeBurnToolClass *klass)
{
  GimpToolClass	*tool_class;

  tool_class = GIMP_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  tool_class->modifier_key  = gimp_dodgeburn_tool_modifier_key;
  tool_class->cursor_update = gimp_dodgeburn_tool_cursor_update;
}

static void
gimp_dodgeburn_tool_init (GimpDodgeBurnTool *dodgeburn)
{
  GimpTool	*tool;
  GimpPaintTool *paint_tool;

  tool       = GIMP_TOOL (dodgeburn);
  paint_tool = GIMP_PAINT_TOOL (dodgeburn);

  tool->tool_cursor         = GIMP_DODGE_TOOL_CURSOR;
  tool->toggle_tool_cursor  = GIMP_BURN_TOOL_CURSOR;

  paint_tool->core = g_object_new (GIMP_TYPE_DODGEBURN, NULL);
}

static void
gimp_dodgeburn_tool_modifier_key (GimpTool        *tool,
                                  GdkModifierType  key,
                                  gboolean         press,
				  GdkModifierType  state,
				  GimpDisplay     *gdisp)
{
  GimpDodgeBurnOptions *options;

  options = (GimpDodgeBurnOptions *) tool->tool_info->tool_options;

  if (key == GDK_CONTROL_MASK &&
      ! (state & GDK_SHIFT_MASK)) /* leave stuff untouched in line draw mode */
    {
      switch (options->type)
        {
        case DODGE:
          gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                                       GINT_TO_POINTER (BURN));
          break;
        case BURN:
          gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                                       GINT_TO_POINTER (DODGE));
          break;
        default:
          break;
        }
    }

  tool->toggled = (options->type == BURN);
}

static void
gimp_dodgeburn_tool_cursor_update (GimpTool        *tool,
                                   GimpCoords      *coords,
				   GdkModifierType  state,
				   GimpDisplay     *gdisp)
{
  GimpDodgeBurnOptions *options;

  options = (GimpDodgeBurnOptions *) tool->tool_info->tool_options;

  tool->toggled = (options->type == BURN);

  GIMP_TOOL_CLASS (parent_class)->cursor_update (tool, coords, state, gdisp);
}


/*  tool options stuff  */

static GimpToolOptions *
gimp_dodgeburn_tool_options_new (GimpToolInfo *tool_info)
{
  GimpDodgeBurnOptions *options;
  GtkWidget            *vbox;
  GtkWidget            *hbox;
  GtkWidget            *label;
  GtkWidget            *scale;
  GtkWidget            *frame;

  options = gimp_dodgeburn_options_new ();

  paint_options_init ((GimpPaintOptions *) options, tool_info);

  ((GimpToolOptions *) options)->reset_func = gimp_dodgeburn_tool_options_reset;

  /*  the main vbox  */
  vbox = ((GimpToolOptions *) options)->main_vbox;

  /*  the exposure scale  */
  hbox = gtk_hbox_new (FALSE, 4);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (_("Exposure:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 1.0);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  options->exposure_w =
    gtk_adjustment_new (options->exposure_d, 0.0, 100.0, 1.0, 1.0, 0.0);
  scale = gtk_hscale_new (GTK_ADJUSTMENT (options->exposure_w));
  gtk_box_pack_start (GTK_BOX (hbox), scale, TRUE, TRUE, 0);
  gtk_scale_set_value_pos (GTK_SCALE (scale), GTK_POS_TOP);
  gtk_range_set_update_policy (GTK_RANGE (scale), GTK_UPDATE_DELAYED);

  g_signal_connect (G_OBJECT (options->exposure_w), "value_changed",
                    G_CALLBACK (gimp_double_adjustment_update),
                    &options->exposure);

  gtk_widget_show (scale);
  gtk_widget_show (hbox);

  /* the type (dodge or burn) */
  frame = gimp_radio_group_new2 (TRUE, _("Type (<Ctrl>)"),
				 G_CALLBACK (gimp_radio_button_update),
				 &options->type,
				 GINT_TO_POINTER (options->type),

				 _("Dodge"),
                                 GINT_TO_POINTER (DODGE),
				 &options->type_w[0],

				 _("Burn"),
                                 GINT_TO_POINTER (BURN),
				 &options->type_w[1],

				 NULL);

  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  /*  mode (highlights, midtones, or shadows)  */
  frame = gimp_radio_group_new2 (TRUE, _("Mode"),
                                 G_CALLBACK (gimp_radio_button_update),
                                 &options->mode,
                                 GINT_TO_POINTER (options->mode),

                                 _("Highlights"),
                                 GINT_TO_POINTER (GIMP_HIGHLIGHTS), 
                                 &options->mode_w[0],

                                 _("Midtones"),
                                 GINT_TO_POINTER (GIMP_MIDTONES),
                                 &options->mode_w[1],

                                 _("Shadows"),
                                 GINT_TO_POINTER (GIMP_SHADOWS),
                                 &options->mode_w[2],

                                 NULL);

  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  return (GimpToolOptions *) options;
}

static void
gimp_dodgeburn_tool_options_reset (GimpToolOptions *tool_options)
{
  GimpDodgeBurnOptions *options;

  options = (GimpDodgeBurnOptions *) tool_options;

  paint_options_reset (tool_options);

  gtk_adjustment_set_value (GTK_ADJUSTMENT (options->exposure_w),
			    options->exposure_d);

  gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                               GINT_TO_POINTER (options->type_d));

  gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->mode_w[0]),
                               GINT_TO_POINTER (options->mode_d));
}
