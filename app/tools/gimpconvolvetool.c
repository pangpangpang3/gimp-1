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
#include <gdk/gdkkeysyms.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "base/pixel-region.h"
#include "base/temp-buf.h"

#include "paint-funcs/paint-funcs.h"

#include "core/gimp.h"
#include "core/gimpbrush.h"
#include "core/gimpcontext.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimptoolinfo.h"

#include "paint/gimpconvolve.h"

#include "gimpconvolvetool.h"
#include "paint_options.h"

#include "libgimp/gimpintl.h"


#define FIELD_COLS    4
#define MIN_BLUR      64         /*  (8/9 original pixel)   */
#define MAX_BLUR      0.25       /*  (1/33 original pixel)  */
#define MIN_SHARPEN   -512
#define MAX_SHARPEN   -64


static void   gimp_convolve_tool_class_init     (GimpConvolveToolClass *klass);
static void   gimp_convolve_tool_init           (GimpConvolveTool      *tool);

static void   gimp_convolve_tool_modifier_key   (GimpTool              *tool,
                                                 GdkModifierType        key,
                                                 gboolean               press,
                                                 GdkModifierType        state,
                                                 GimpDisplay           *gdisp);
static void   gimp_convolve_tool_cursor_update  (GimpTool              *tool,
                                                 GimpCoords            *coords,
                                                 GdkModifierType        state,
                                                 GimpDisplay           *gdisp);

static GimpToolOptions * convolve_options_new   (GimpToolInfo          *tool_info);
static void              convolve_options_reset (GimpToolOptions       *options);


static GimpPaintToolClass *parent_class;


/*  public functions  */

void
gimp_convolve_tool_register (Gimp                     *gimp,
                             GimpToolRegisterCallback  callback)
{
  (* callback) (gimp,
                GIMP_TYPE_CONVOLVE_TOOL,
                convolve_options_new,
                TRUE,
                "gimp:convolve_tool",
                _("Convolve"),
                _("Blur or Sharpen"),
                N_("/Tools/Paint Tools/Convolve"), "B",
                NULL, "tools/convolve.html",
                GIMP_STOCK_TOOL_BLUR);
}

GType
gimp_convolve_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpConvolveToolClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_convolve_tool_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpConvolveTool),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_convolve_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_PAINT_TOOL,
					  "GimpConvolveTool",
                                          &tool_info, 0);
    }

  return tool_type;
}

/* static functions  */

static void
gimp_convolve_tool_class_init (GimpConvolveToolClass *klass)
{
  GimpToolClass *tool_class;

  tool_class = GIMP_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  tool_class->modifier_key  = gimp_convolve_tool_modifier_key;
  tool_class->cursor_update = gimp_convolve_tool_cursor_update;
}

static void
gimp_convolve_tool_init (GimpConvolveTool *convolve)
{
  GimpTool      *tool;
  GimpPaintTool *paint_tool;

  tool       = GIMP_TOOL (convolve);
  paint_tool = GIMP_PAINT_TOOL (convolve);

  tool->tool_cursor            = GIMP_BLUR_TOOL_CURSOR;
  tool->cursor_modifier        = GIMP_CURSOR_MODIFIER_NONE;
  tool->toggle_tool_cursor     = GIMP_BLUR_TOOL_CURSOR;
  tool->toggle_cursor_modifier = GIMP_CURSOR_MODIFIER_MINUS;

  paint_tool->core = g_object_new (GIMP_TYPE_CONVOLVE, NULL);
}

static void
gimp_convolve_tool_modifier_key (GimpTool        *tool,
                                 GdkModifierType  key,
                                 gboolean         press,
				 GdkModifierType  state,
				 GimpDisplay     *gdisp)
{
  GimpConvolveOptions *options;

  options = (GimpConvolveOptions *) tool->tool_info->tool_options;

  if ((key == GDK_CONTROL_MASK) &&
      ! (state & GDK_SHIFT_MASK)) /* leave stuff untouched in line draw mode */
    {
      switch (options->type)
        {
        case BLUR_CONVOLVE:
          gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                                       GINT_TO_POINTER (SHARPEN_CONVOLVE));
          break;
        case SHARPEN_CONVOLVE:
          gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                                       GINT_TO_POINTER (BLUR_CONVOLVE));
          break;
        default:
          break;
        }
    }
}

static void
gimp_convolve_tool_cursor_update (GimpTool        *tool,
                                  GimpCoords      *coords,
				  GdkModifierType  state,
				  GimpDisplay     *gdisp)
{
  GimpConvolveOptions *options;

  options = (GimpConvolveOptions *) tool->tool_info->tool_options;

  tool->toggled = (options->type == SHARPEN_CONVOLVE);

  GIMP_TOOL_CLASS (parent_class)->cursor_update (tool, coords, state, gdisp);
}


/*  tool options stuff  */

static GimpToolOptions *
convolve_options_new (GimpToolInfo *tool_info)
{
  GimpConvolveOptions *options;
  GtkWidget           *vbox;
  GtkWidget           *hbox;
  GtkWidget           *label;
  GtkWidget           *scale;
  GtkWidget           *frame;

  options = gimp_convolve_options_new ();

  paint_options_init ((GimpPaintOptions *) options, tool_info);

  ((GimpToolOptions *) options)->reset_func = convolve_options_reset;

  /*  the main vbox  */
  vbox = ((GimpToolOptions *) options)->main_vbox;

  /*  the rate scale  */
  hbox = gtk_hbox_new (FALSE, 4);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  label = gtk_label_new (_("Rate:"));
  gtk_misc_set_alignment (GTK_MISC (label), 1.0, 1.0);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  options->rate_w =
    gtk_adjustment_new (options->rate_d, 0.0, 100.0, 1.0, 1.0, 0.0);
  scale = gtk_hscale_new (GTK_ADJUSTMENT (options->rate_w));
  gtk_box_pack_start (GTK_BOX (hbox), scale, TRUE, TRUE, 0);
  gtk_scale_set_value_pos (GTK_SCALE (scale), GTK_POS_TOP);
  gtk_range_set_update_policy (GTK_RANGE (scale), GTK_UPDATE_DELAYED);
  g_signal_connect (G_OBJECT (options->rate_w), "value_changed",
                    G_CALLBACK (gimp_double_adjustment_update),
                    &options->rate);
  gtk_widget_show (scale);
  gtk_widget_show (hbox);

  frame = gimp_radio_group_new2 (TRUE, _("Convolve Type (<Ctrl>)"),
				 G_CALLBACK (gimp_radio_button_update),
				 &options->type,
                                 GINT_TO_POINTER (options->type),

				 _("Blur"),
                                 GINT_TO_POINTER (BLUR_CONVOLVE),
				 &options->type_w[0],

				 _("Sharpen"),
                                 GINT_TO_POINTER (SHARPEN_CONVOLVE),
				 &options->type_w[1],

				 NULL);

  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  return (GimpToolOptions *) options;
}

static void
convolve_options_reset (GimpToolOptions *tool_options)
{
  GimpConvolveOptions *options;

  options = (GimpConvolveOptions *) tool_options;

  paint_options_reset (tool_options);

  gtk_adjustment_set_value (GTK_ADJUSTMENT (options->rate_w),
			    options->rate_d);

  gimp_radio_group_set_active (GTK_RADIO_BUTTON (options->type_w[0]),
                               GINT_TO_POINTER (options->type_d));
}
