/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimppatterns_pdb.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by pdbgen.pl */

#ifndef __GIMP_PATTERNS_H__
#define __GIMP_PATTERNS_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


gchar* gimp_patterns_get_pattern_data (gchar   *name,
				       gint    *width,
				       gint    *height,
				       gint    *mask_bpp,
				       gint    *mask_data_size,
				       guint8 **mask_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GIMP_PATTERNS_H__ */
