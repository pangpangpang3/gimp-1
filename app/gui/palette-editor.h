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

#ifndef __PALETTE_EDITOR_H__
#define __PALETTE_EDITOR_H__


typedef enum
{
  GIMP_UPDATE_COLOR_STATE_NEW,
  GIMP_UPDATE_COLOR_STATE_UPDATE_NEW,
  GIMP_UPDATE_COLOR_STATE_UPDATE
} GimpUpdateColorState;


typedef struct _PaletteEditor PaletteEditor;


PaletteEditor * palette_editor_new          (Gimp          *gimp);

void            palette_editor_set_palette  (PaletteEditor *palette_editor,
                                             GimpPalette   *palette);
void            palette_editor_free         (PaletteEditor *palette_editor);

void            palette_editor_update_color (GimpContext          *context,
                                             const GimpRGB        *color,
                                             GimpUpdateColorState  state);


#endif /* __PALETTE_EDITOR_H__ */
