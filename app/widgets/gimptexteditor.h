/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GimpTextEditor
 * Copyright (C) 2002-2003  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_TEXT_EDITOR_H__
#define __GIMP_TEXT_EDITOR_H__


#define GIMP_TYPE_TEXT_EDITOR    (gimp_text_editor_get_type ())
#define GIMP_TEXT_EDITOR(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_TEXT_EDITOR, GimpTextEditor))
#define GIMP_IS_TEXT_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_TEXT_EDITOR))


typedef struct _GimpTextEditorClass  GimpTextEditorClass;

struct _GimpTextEditor
{
  GimpDialog         parent_instance;

  GimpTextDirection  base_dir;

  /*<  private  >*/
  GtkTextBuffer     *buffer;
  GtkWidget         *group;
  GtkWidget         *view;
  GtkWidget         *filesel;
};

struct _GimpTextEditorClass
{
  GimpDialogClass   parent_class;

  void (* dir_changed) (GimpTextEditor *editor);
};


GType        gimp_text_editor_get_type      (void) G_GNUC_CONST;
GtkWidget  * gimp_text_editor_new           (const gchar       *title,
                                             GtkTextBuffer     *buffer);
void         gimp_text_editor_set_direction (GimpTextEditor    *editor,
                                             GimpTextDirection  base_dir);


#endif  /* __GIMP_TEXT_EDITOR_H__ */
