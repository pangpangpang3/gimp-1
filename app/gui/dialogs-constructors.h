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

#ifndef __DIALOGS_CONSTRUCTORS_H__
#define __DIALOGS_CONSTRUCTORS_H__


GtkWidget * dialogs_toolbox_get            (GimpDialogFactory *factory);
GtkWidget * dialogs_lc_get                 (GimpDialogFactory *factory);
GtkWidget * dialogs_tool_options_get       (GimpDialogFactory *factory);
GtkWidget * dialogs_device_status_get      (GimpDialogFactory *factory);
GtkWidget * dialogs_brush_select_get       (GimpDialogFactory *factory);
GtkWidget * dialogs_pattern_select_get     (GimpDialogFactory *factory);
GtkWidget * dialogs_gradient_select_get    (GimpDialogFactory *factory);
GtkWidget * dialogs_palette_get            (GimpDialogFactory *factory);
GtkWidget * dialogs_error_console_get      (GimpDialogFactory *factory);
GtkWidget * dialogs_document_index_get     (GimpDialogFactory *factory);

GtkWidget * dialogs_image_list_view_new    (GimpDialogFactory *factory);
GtkWidget * dialogs_brush_list_view_new    (GimpDialogFactory *factory);
GtkWidget * dialogs_pattern_list_view_new  (GimpDialogFactory *factory);
GtkWidget * dialogs_gradient_list_view_new (GimpDialogFactory *factory);
GtkWidget * dialogs_palette_list_view_new  (GimpDialogFactory *factory);
GtkWidget * dialogs_tool_list_view_new     (GimpDialogFactory *factory);

GtkWidget * dialogs_image_grid_view_new    (GimpDialogFactory *factory);
GtkWidget * dialogs_brush_grid_view_new    (GimpDialogFactory *factory);
GtkWidget * dialogs_pattern_grid_view_new  (GimpDialogFactory *factory);
GtkWidget * dialogs_gradient_grid_view_new (GimpDialogFactory *factory);
GtkWidget * dialogs_palette_grid_view_new  (GimpDialogFactory *factory);
GtkWidget * dialogs_tool_grid_view_new     (GimpDialogFactory *factory);


#endif /* __DIALOGS_CONSTRUCTORS_H__ */
