/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1997 Spencer Kimball and Peter Mattis
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

#ifndef __GIMP_VIEWABLE_H__
#define __GIMP_VIEWABLE_H__


#include "gimpobject.h"


#define GIMP_TYPE_VIEWABLE            (gimp_viewable_get_type ())
#define GIMP_VIEWABLE(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_VIEWABLE, GimpViewable))
#define GIMP_VIEWABLE_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_VIEWABLE, GimpViewableClass))
#define GIMP_IS_VIEWABLE(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_VIEWABLE))
#define GIMP_IS_VIEWABLE_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_VIEWABLE))


typedef struct _GimpViewableClass GimpViewableClass;

struct _GimpViewable
{
  GimpObject  parent_instance;
};

struct _GimpViewableClass
{
  GimpObjectClass  parent_class;

  void      (* invalidate_preview) (GimpViewable *viewable);
  TempBuf * (* preview)            (GimpViewable *viewable,
				    gint          width,
				    gint          height);
  TempBuf * (* preview_new)        (GimpViewable *viewable,
				    gint          width,
				    gint          height);
};


GtkType   gimp_viewable_get_type           (void);

void      gimp_viewable_invalidate_preview (GimpViewable *viewable);
TempBuf * gimp_viewable_preview            (GimpViewable *viewable,
					    gint          width,
					    gint          height);
TempBuf * gimp_viewable_preview_new        (GimpViewable *viewable,
					    gint          width,
					    gint          height);


#endif  /* __GIMP_VIEWABLE_H__ */
