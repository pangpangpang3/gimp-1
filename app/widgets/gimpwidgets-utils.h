/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpwidgets-utils.h
 * Copyright (C) 1999-2003 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_WIDGETS_UTILS_H__
#define __GIMP_WIDGETS_UTILS_H__


void          gimp_menu_position         (GtkMenu         *menu,
                                          gint            *x,
                                          gint            *y);
void          gimp_button_menu_position  (GtkWidget       *button,
                                          GtkMenu         *menu,
                                          GtkPositionType  position,
                                          gint            *x,
                                          gint            *y);

void          gimp_table_attach_stock    (GtkTable        *table,
					  gint             row,
					  const gchar     *label_text,
					  gdouble          yalign,
					  GtkWidget       *widget,
					  gint             colspan,
					  const gchar     *stock_id);
void          gimp_enum_radio_frame_add  (GtkFrame        *frame,
                                          GtkWidget       *widget,
                                          gint             enum_value);

GtkIconSize   gimp_get_icon_size         (GtkWidget       *widget,
                                          const gchar     *stock_id,
                                          GtkIconSize      max_size,
                                          gint             width,
                                          gint             height);

const gchar * gimp_get_mod_name_shift    (void);
const gchar * gimp_get_mod_name_control  (void);
const gchar * gimp_get_mod_name_alt      (void);
const gchar * gimp_get_mod_separator     (void);
const gchar * gimp_get_mod_string        (GdkModifierType  modifiers);
gchar       * gimp_get_accel_string      (guint            key,
                                          GdkModifierType  modifiers);

void          gimp_get_screen_resolution (GdkScreen       *screen,
                                          gdouble         *xres,
                                          gdouble         *yres);

void          gimp_rgb_get_gdk_color     (const GimpRGB   *rgb,
                                          GdkColor        *gdk_color);
void          gimp_rgb_set_gdk_color     (GimpRGB         *rgb,
                                          const GdkColor  *gdk_color);

void          gimp_window_set_hint       (GtkWindow       *window,
                                          GimpWindowHint   hint);

void          gimp_dialog_set_sensitive  (GtkDialog       *dialog,
                                          gboolean         sensitive);

gboolean      gimp_text_buffer_save      (GtkTextBuffer   *buffer,
                                          const gchar     *filename,
                                          gboolean         selection_only,
                                          GError         **error);


#endif /* __GIMP_WIDGETS_UTILS_H__ */
