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

#ifndef __GIMP_DISPLAY_AREA_H__
#define __GIMP_DISPLAY_AREA_H__


typedef struct _GimpArea GimpArea;

struct _GimpArea
{
  gint x1, y1, x2, y2;   /*  area bounds  */
};


GSList * gimp_display_area_list_process (GSList   *list,
                                         GimpArea *area);
GSList * gimp_display_area_list_free    (GSList   *list);


#endif /*  __GIMP_DISPLAY_AREA_H__  */
