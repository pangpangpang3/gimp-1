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

#ifndef __GIMP_DRAWABLE_H__
#define __GIMP_DRAWABLE_H__


#include "gimpitem.h"


#define GIMP_TYPE_DRAWABLE            (gimp_drawable_get_type ())
#define GIMP_DRAWABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_DRAWABLE, GimpDrawable))
#define GIMP_DRAWABLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_DRAWABLE, GimpDrawableClass))
#define GIMP_IS_DRAWABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_DRAWABLE))
#define GIMP_IS_DRAWABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_DRAWABLE))
#define GIMP_DRAWABLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_DRAWABLE, GimpDrawableClass))


typedef struct _GimpDrawableClass GimpDrawableClass;

struct _GimpDrawable
{
  GimpItem       parent_instance;

  TileManager   *tiles;              /* tiles for drawable data        */
  gboolean       visible;            /* controls visibility            */

  gint           bytes;              /* bytes per pixel                */
  GimpImageType  type;               /* type of drawable               */
  gboolean       has_alpha;          /* drawable has alpha             */

  /*  Preview variables  */
  GSList        *preview_cache;      /* preview caches of the channel  */
  gboolean       preview_valid;      /* is the preview valid?          */
};

struct _GimpDrawableClass
{
  GimpItemClass  parent_class;

  void (* visibility_changed) (GimpDrawable *drawable);
  void (* alpha_changed)      (GimpDrawable *drawable);
};


/*  drawable access functions  */

GType           gimp_drawable_get_type           (void) G_GNUC_CONST;

void            gimp_drawable_configure          (GimpDrawable       *drawable,
						  GimpImage          *gimage,
                                                  gint                offset_x,
                                                  gint                offset_y,
						  gint                width,
						  gint                height,
						  GimpImageType       type,
						  const gchar        *name);

void            gimp_drawable_update             (GimpDrawable       *drawable,
						  gint                x,
						  gint                y,
						  gint                w,
						  gint                h);

void            gimp_drawable_push_undo          (GimpDrawable       *drawable,
                                                  const gchar        *undo_desc,
						  gint                x1,
						  gint                y1,
						  gint                x2,
						  gint                y2, 
						  TileManager        *tiles,
						  gboolean            sparse);

void            gimp_drawable_merge_shadow       (GimpDrawable       *drawable,
						  gboolean            push_undo,
                                                  const gchar        *undo_desc);

void            gimp_drawable_fill               (GimpDrawable       *drawable,
						  const GimpRGB      *color);
void            gimp_drawable_fill_by_type       (GimpDrawable       *drawable,
						  GimpContext        *context,
						  GimpFillType        fill_type);

gboolean        gimp_drawable_mask_bounds        (GimpDrawable       *drawable,
						  gint               *x1,
						  gint               *y1,
						  gint               *x2,
						  gint               *y2);

gboolean        gimp_drawable_has_alpha          (const GimpDrawable *drawable);
GimpImageType   gimp_drawable_type               (const GimpDrawable *drawable);
GimpImageType   gimp_drawable_type_with_alpha    (const GimpDrawable *drawable);
gboolean        gimp_drawable_is_rgb             (const GimpDrawable *drawable);
gboolean        gimp_drawable_is_gray            (const GimpDrawable *drawable);
gboolean        gimp_drawable_is_indexed         (const GimpDrawable *drawable);
TileManager   * gimp_drawable_data               (const GimpDrawable *drawable);
TileManager   * gimp_drawable_shadow             (GimpDrawable       *drawable);
gint            gimp_drawable_bytes              (const GimpDrawable *drawable);
gint            gimp_drawable_bytes_with_alpha   (const GimpDrawable *drawable);

gboolean	gimp_drawable_get_visible        (const GimpDrawable *drawable);
void            gimp_drawable_set_visible        (GimpDrawable       *drawable,
                                                  gboolean            visible,
                                                  gboolean            push_undo);

void            gimp_drawable_alpha_changed      (GimpDrawable       *drawable);

guchar        * gimp_drawable_cmap               (const GimpDrawable *drawable);

guchar        * gimp_drawable_get_color_at       (GimpDrawable       *drawable,
						  gint                x,
						  gint                y);


#endif /* __GIMP_DRAWABLE_H__ */
