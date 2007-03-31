/* GIMP - The GNU Image Manipulation Program
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

#include "core/gimpchannel-select.h"
#include "core/gimpimage.h"

#include "widgets/gimphelp-ids.h"

#include "display/gimpdisplay.h"

#include "gimpellipseselecttool.h"
#include "gimprectangleselectoptions.h"
#include "gimptoolcontrol.h"

#include "gimp-intl.h"


static void   gimp_ellipse_select_tool_draw   (GimpDrawTool       *draw_tool);

static void   gimp_ellipse_select_tool_select (GimpRectSelectTool *rect_tool,
                                               GimpChannelOps      operation,
                                               gint                x,
                                               gint                y,
                                               gint                w,
                                               gint                h);


G_DEFINE_TYPE (GimpEllipseSelectTool, gimp_ellipse_select_tool,
               GIMP_TYPE_RECT_SELECT_TOOL)

#define parent_class gimp_ellipse_select_tool_parent_class


/*  public functions  */

void
gimp_ellipse_select_tool_register (GimpToolRegisterCallback  callback,
                                   gpointer                  data)
{
  (* callback) (GIMP_TYPE_ELLIPSE_SELECT_TOOL,
                GIMP_TYPE_RECT_SELECT_OPTIONS,
                gimp_rect_select_options_gui,
                0,
                "gimp-ellipse-select-tool",
                _("Ellipse Select"),
                _("Ellipse Select Tool: Select an elliptical region"),
                N_("_Ellipse Select"), "E",
                NULL, GIMP_HELP_TOOL_ELLIPSE_SELECT,
                GIMP_STOCK_TOOL_ELLIPSE_SELECT,
                data);
}


/*  private functions  */

static void
gimp_ellipse_select_tool_class_init (GimpEllipseSelectToolClass *klass)
{
  GimpDrawToolClass       *draw_tool_class = GIMP_DRAW_TOOL_CLASS (klass);
  GimpRectSelectToolClass *rect_tool_class = GIMP_RECT_SELECT_TOOL_CLASS (klass);

  draw_tool_class->draw   = gimp_ellipse_select_tool_draw;

  rect_tool_class->select = gimp_ellipse_select_tool_select;
}

static void
gimp_ellipse_select_tool_init (GimpEllipseSelectTool *ellipse_select)
{
  GimpTool *tool = GIMP_TOOL (ellipse_select);

  gimp_tool_control_set_tool_cursor (tool->control,
                                     GIMP_TOOL_CURSOR_ELLIPSE_SELECT);
}

static void
gimp_ellipse_select_tool_draw (GimpDrawTool *draw_tool)
{
  gint x1, y1, x2, y2;

  GIMP_DRAW_TOOL_CLASS (parent_class)->draw (draw_tool);

  g_object_get (draw_tool,
                "x1", &x1,
                "y1", &y1,
                "x2", &x2,
                "y2", &y2,
                NULL);

  gimp_draw_tool_draw_arc (draw_tool,
                           FALSE,
                           x1, y1,
                           x2 - x1, y2 - y1,
                           0, 360 * 64,
                           FALSE);
}

static void
gimp_ellipse_select_tool_select (GimpRectSelectTool *rect_tool,
                                 GimpChannelOps      operation,
                                 gint                x,
                                 gint                y,
                                 gint                w,
                                 gint                h)
{
  GimpTool             *tool    = GIMP_TOOL (rect_tool);
  GimpSelectionOptions *options = GIMP_SELECTION_TOOL_GET_OPTIONS (rect_tool);

  gimp_channel_select_ellipse (gimp_image_get_mask (tool->display->image),
                               x, y, w, h,
                               operation,
                               options->antialias,
                               options->feather,
                               options->feather_radius,
                               options->feather_radius,
                               TRUE);
}
