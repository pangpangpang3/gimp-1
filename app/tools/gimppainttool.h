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

#ifndef __GIMP_PAINT_TOOL_H__
#define __GIMP_PAINT_TOOL_H__


#include "gimpdrawtool.h"


/* the different states that the painting function can be called with  */

typedef enum /*< pdb-skip >*/
{
  INIT_PAINT,       /* Setup PaintFunc internals */
  MOTION_PAINT,     /* PaintFunc performs motion-related rendering */
  PAUSE_PAINT,      /* Unused. Reserved */
  RESUME_PAINT,     /* Unused. Reserved */
  FINISH_PAINT,     /* Cleanup and/or reset PaintFunc operation */
  PRETRACE_PAINT,   /* PaintFunc performs window tracing activity prior to rendering */
  POSTTRACE_PAINT   /* PaintFunc performs window tracing activity following rendering */
} PaintState;

typedef enum /*< pdb-skip >*/
{
  TOOL_CAN_HANDLE_CHANGING_BRUSH = 0x0001, /* Set for tools that don't mind
					    * if the brush changes while
					    * painting.
					    */

  TOOL_TRACES_ON_WINDOW                    /* Set for tools that perform temporary
                                            * rendering directly to the window. These
                                            * require sequencing with gdisplay_flush()
                                            * routines. See clone.c for example.
                                            */
} ToolFlags;


#define GIMP_TYPE_PAINT_TOOL            (gimp_paint_tool_get_type ())
#define GIMP_PAINT_TOOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_PAINT_TOOL, GimpPaintTool))
#define GIMP_PAINT_TOOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_PAINT_TOOL, GimpPaintToolClass))
#define GIMP_IS_PAINT_TOOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_PAINT_TOOL))
#define GIMP_IS_PAINT_TOOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_PAINT_TOOL))
#define GIMP_PAINT_TOOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_PAINT_TOOL, GimpPaintToolClass))


typedef struct _GimpPaintToolClass GimpPaintToolClass;

struct _GimpPaintTool
{
  GimpDrawTool  parent_instance;

  GimpCoords    start_coords;  /*  starting coords            */
  GimpCoords    cur_coords;    /*  current coords             */
  GimpCoords    last_coords;   /*  last coords                */

  gint          state;         /*  state of buttons and keys  */

  gdouble       distance;      /*  distance traveled by brush */
  gdouble       pixel_dist;    /*  distance in pixels         */
  gdouble       spacing;       /*  spacing                    */

  gint          x1, y1;        /*  image space coordinate     */
  gint          x2, y2;        /*  image space coords         */

  GimpBrush    *brush;         /*  current brush	      */

  gboolean      pick_colors;   /*  pick color if ctrl or alt is pressed  */
  gboolean      pick_state;    /*  was ctrl or alt pressed when clicked? */
  ToolFlags     flags;         /*  tool flags, see ToolFlags above       */
};

struct _GimpPaintToolClass
{
  GimpDrawToolClass parent_class;

  /*  virtual function  */

  void (* paint) (GimpPaintTool *tool,
		  GimpDrawable 	*drawable,
		  PaintState     paint_state);
};


/*  Special undo type  */
typedef struct _PaintUndo PaintUndo;

struct _PaintUndo
{
  gint        tool_ID;
  GType       tool_type;

  GimpCoords  last_coords;
};


GType   gimp_paint_tool_get_type        (void) G_GNUC_CONST;

void    gimp_paint_tool_paint           (GimpPaintTool       *paint_tool,
					 GimpDrawable        *drawable,
					 PaintState	      state);

int     gimp_paint_tool_start           (GimpPaintTool       *paint_tool,
					 GimpDrawable        *drawable,
					 gdouble              x,
					 gdouble              y);
void    gimp_paint_tool_interpolate     (GimpPaintTool       *paint_tool,
					 GimpDrawable        *drawable);
void    gimp_paint_tool_finish          (GimpPaintTool       *paint_tool,
					 GimpDrawable        *drawable);
void    gimp_paint_tool_cleanup         (GimpPaintTool       *paint_tool);

void    gimp_paint_tool_get_color_from_gradient (GimpPaintTool     *paint_tool,
                                                 GimpGradient      *gradient,
						 gdouble            gradient_length,
						 GimpRGB           *color,
						 GradientPaintMode  mode);

/*  paint tool painting functions  */
TempBuf * gimp_paint_tool_get_paint_area        (GimpPaintTool        *paint_tool,
                                                 GimpDrawable         *drawable,
                                                 gdouble               scale);
TempBuf * gimp_paint_tool_get_orig_image        (GimpPaintTool        *paint_tool,
                                                 GimpDrawable         *drawable,
                                                 gint                  x1,
                                                 gint                  y1,
                                                 gint                  x2,
                                                 gint                  y2);
void      gimp_paint_tool_paste_canvas          (GimpPaintTool        *paint_tool,
                                                 GimpDrawable         *drawable,
                                                 gint                  brush_opacity,
                                                 gint                  image_opacity,
                                                 GimpLayerModeEffects  paint_mode,
                                                 BrushApplicationMode  brush_hardness,
                                                 gdouble               brush_scale,
                                                 PaintApplicationMode  mode);
void      gimp_paint_tool_replace_canvas        (GimpPaintTool        *paint_tool,
                                                 GimpDrawable         *drawable,
                                                 gint                  brush_opacity,
                                                 gint                  image_opacity,
                                                 BrushApplicationMode  brush_hardness,
                                                 gdouble               brush_scale,
                                                 PaintApplicationMode  mode);
void     gimp_paint_tool_color_area_with_pixmap (GimpPaintTool        *paint_tool,
                                                 GimpImage            *dest, 
                                                 GimpDrawable         *drawable,
                                                 TempBuf              *area, 
                                                 gdouble               scale, 
                                                 BrushApplicationMode  mode);


#endif  /*  __GIMP_PAINT_TOOL_H__  */
