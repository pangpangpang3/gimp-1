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

#include "base/gimplut.h"
#include "base/lut-funcs.h"

#include "core/gimpcontext.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimpimagemap.h"

#include "display/gimpdisplay.h"

#include "gimpbrightnesscontrasttool.h"

#include "gimp-intl.h"


#define SLIDER_WIDTH 200

#define BRIGHTNESS  0x1
#define CONTRAST    0x2
#define ALL        (BRIGHTNESS | CONTRAST)


static void   gimp_brightness_contrast_tool_class_init (GimpBrightnessContrastToolClass *klass);
static void   gimp_brightness_contrast_tool_init       (GimpBrightnessContrastTool      *bc_tool);

static void   gimp_brightness_contrast_tool_finalize   (GObject          *object);

static void   gimp_brightness_contrast_tool_initialize (GimpTool         *tool,
							GimpDisplay      *gdisp);

static void   gimp_brightness_contrast_tool_map        (GimpImageMapTool *image_map_tool);
static void   gimp_brightness_contrast_tool_dialog     (GimpImageMapTool *image_map_tool);
static void   gimp_brightness_contrast_tool_reset      (GimpImageMapTool *image_map_tool);

static void   brightness_contrast_update          (GimpBrightnessContrastTool *bc_tool,
						   gint                        update);
static void   brightness_contrast_brightness_adjustment_update (GtkAdjustment *adj,
								gpointer       data);
static void   brightness_contrast_contrast_adjustment_update   (GtkAdjustment *adj,
								gpointer      data);


static GimpImageMapToolClass *parent_class = NULL;


/*  functions  */

void
gimp_brightness_contrast_tool_register (GimpToolRegisterCallback  callback,
                                        gpointer                  data)
{
  (* callback) (GIMP_TYPE_BRIGHTNESS_CONTRAST_TOOL,
                G_TYPE_NONE, NULL,
                FALSE,
                "gimp-brightness-contrast-tool",
                _("Brightness-Contrast"),
                _("Adjust brightness and contrast"),
                N_("/Layer/Colors/Brightness-Contrast..."), NULL,
                NULL, "tools/brightness_contrast.html",
                GIMP_STOCK_TOOL_BRIGHTNESS_CONTRAST,
                data);
}

GType
gimp_brightness_contrast_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpBrightnessContrastToolClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_brightness_contrast_tool_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpBrightnessContrastTool),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_brightness_contrast_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_IMAGE_MAP_TOOL,
					  "GimpBrightnessContrastTool", 
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_brightness_contrast_tool_class_init (GimpBrightnessContrastToolClass *klass)
{
  GObjectClass          *object_class;
  GimpToolClass         *tool_class;
  GimpImageMapToolClass *image_map_tool_class;

  object_class         = G_OBJECT_CLASS (klass);
  tool_class           = GIMP_TOOL_CLASS (klass);
  image_map_tool_class = GIMP_IMAGE_MAP_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize       = gimp_brightness_contrast_tool_finalize;

  tool_class->initialize       = gimp_brightness_contrast_tool_initialize;

  image_map_tool_class->map    = gimp_brightness_contrast_tool_map;
  image_map_tool_class->dialog = gimp_brightness_contrast_tool_dialog;
  image_map_tool_class->reset  = gimp_brightness_contrast_tool_reset;
}

static void
gimp_brightness_contrast_tool_init (GimpBrightnessContrastTool *bc_tool)
{
  GimpImageMapTool *image_map_tool;

  image_map_tool = GIMP_IMAGE_MAP_TOOL (bc_tool);

  image_map_tool->shell_identifier = "gimp-brightness-contrast-tool-dialog";
  image_map_tool->shell_desc       = _("Adjust Brightness and Contrast");

  bc_tool->brightness = 0.0;
  bc_tool->contrast   = 0.0;
  bc_tool->lut        = gimp_lut_new ();
}

static void
gimp_brightness_contrast_tool_finalize (GObject *object)
{
  GimpBrightnessContrastTool *bc_tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (object);

  if (bc_tool->lut)
    {
      gimp_lut_free (bc_tool->lut);
      bc_tool->lut = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_brightness_contrast_tool_initialize (GimpTool    *tool,
					  GimpDisplay *gdisp)
{
  GimpBrightnessContrastTool *bc_tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (tool);

  if (gimp_drawable_is_indexed (gimp_image_active_drawable (gdisp->gimage)))
    {
      g_message (_("Brightness-Contrast does not operate on indexed drawables."));
      return;
    }

  bc_tool->brightness = 0.0;
  bc_tool->contrast   = 0.0;

  GIMP_TOOL_CLASS (parent_class)->initialize (tool, gdisp);

  brightness_contrast_update (bc_tool, ALL);
}

static void
gimp_brightness_contrast_tool_map (GimpImageMapTool *image_map_tool)
{
  GimpBrightnessContrastTool *bc_tool;
  GimpTool                   *tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (image_map_tool);
  tool    = GIMP_TOOL (image_map_tool);

  brightness_contrast_lut_setup (bc_tool->lut,
                                 bc_tool->brightness / 255.0,
				 bc_tool->contrast / 127.0,
				 gimp_drawable_bytes (image_map_tool->drawable));
  gimp_image_map_apply (image_map_tool->image_map,
                        (GimpImageMapApplyFunc) gimp_lut_process_2,
                        bc_tool->lut);
}


/********************************/
/*  Brightness Contrast dialog  */
/********************************/

static void
gimp_brightness_contrast_tool_dialog (GimpImageMapTool *image_map_tool)
{
  GimpBrightnessContrastTool *bc_tool;
  GtkWidget                  *table;
  GtkWidget                  *slider;
  GtkObject                  *data;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (image_map_tool);

  /*  The table containing sliders  */
  table = gtk_table_new (2, 3, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_table_set_row_spacings (GTK_TABLE (table), 2);
  gtk_box_pack_start (GTK_BOX (image_map_tool->main_vbox), table,
                      FALSE, FALSE, 0);
  gtk_widget_show (table);

  /*  Create the brightness scale widget  */
  data = gimp_scale_entry_new (GTK_TABLE (table), 0, 0,
                               _("_Brightness:"), SLIDER_WIDTH, -1,
                               bc_tool->brightness,
                               -127.0, 127.0, 1.0, 10.0, 0,
                               TRUE, 0.0, 0.0,
                               NULL, NULL);
  bc_tool->brightness_data = GTK_ADJUSTMENT (data);
  slider = GIMP_SCALE_ENTRY_SCALE (data);
  gtk_range_set_update_policy (GTK_RANGE (slider), GTK_UPDATE_DELAYED);

  g_signal_connect (data, "value_changed",
                    G_CALLBACK (brightness_contrast_brightness_adjustment_update),
                    bc_tool);

  /*  Create the contrast scale widget  */
  data = gimp_scale_entry_new (GTK_TABLE (table), 0, 1,
                               _("Con_trast:"), SLIDER_WIDTH, -1,
                               bc_tool->contrast,
                               -127.0, 127.0, 1.0, 10.0, 0,
                               TRUE, 0.0, 0.0,
                               NULL, NULL);
  bc_tool->contrast_data = GTK_ADJUSTMENT (data);
  slider = GIMP_SCALE_ENTRY_SCALE (data);
  gtk_range_set_update_policy (GTK_RANGE (slider), GTK_UPDATE_DELAYED);

  g_signal_connect (data, "value_changed",
                    G_CALLBACK (brightness_contrast_contrast_adjustment_update),
                    bc_tool);
}

static void
gimp_brightness_contrast_tool_reset (GimpImageMapTool *image_map_tool)
{
  GimpBrightnessContrastTool *bc_tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (image_map_tool);

  bc_tool->brightness = 0.0;
  bc_tool->contrast   = 0.0;

  brightness_contrast_update (bc_tool, ALL);
}

static void
brightness_contrast_update (GimpBrightnessContrastTool *bc_tool,
			    gint                        update)
{
  if (update & BRIGHTNESS)
    gtk_adjustment_set_value (bc_tool->brightness_data, bc_tool->brightness);

  if (update & CONTRAST)
    gtk_adjustment_set_value (bc_tool->contrast_data, bc_tool->contrast);
}

static void
brightness_contrast_brightness_adjustment_update (GtkAdjustment *adjustment,
						  gpointer       data)
{
  GimpBrightnessContrastTool *bc_tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (data);

  if (bc_tool->brightness != adjustment->value)
    {
      bc_tool->brightness = adjustment->value;

      gimp_image_map_tool_preview (GIMP_IMAGE_MAP_TOOL (bc_tool));
    }
}

static void
brightness_contrast_contrast_adjustment_update (GtkAdjustment *adjustment,
						gpointer       data)
{
  GimpBrightnessContrastTool *bc_tool;

  bc_tool = GIMP_BRIGHTNESS_CONTRAST_TOOL (data);

  if (bc_tool->contrast != adjustment->value)
    {
      bc_tool->contrast = adjustment->value;

      gimp_image_map_tool_preview (GIMP_IMAGE_MAP_TOOL (bc_tool));
    }
}
