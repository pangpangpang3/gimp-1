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

#ifndef __GIMP_SELECTION_EDITOR_H__
#define __GIMP_SELECTION_EDITOR_H__


#include "gimpeditor.h"

#include "gui/gui-types.h" /* temp hack */


#define GIMP_TYPE_SELECTION_EDITOR            (gimp_selection_editor_get_type ())
#define GIMP_SELECTION_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_SELECTION_EDITOR, GimpSelectionEditor))
#define GIMP_SELECTION_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_SELECTION_EDITOR, GimpSelectionEditorClass))
#define GIMP_IS_SELECTION_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_SELECTION_EDITOR))
#define GIMP_IS_SELECTION_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_SELECTION_EDITOR))
#define GIMP_SELECTION_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_SELECTION_EDITOR, GimpSelectionEditorClass))


typedef struct _GimpSelectionEditorClass GimpSelectionEditorClass;

struct _GimpSelectionEditor
{
  GimpEditor  parent_instance;

  GimpImage  *gimage;

  GtkWidget  *preview;

  GtkWidget  *invert_button;
  GtkWidget  *all_button;
  GtkWidget  *none_button;
  GtkWidget  *save_button;

  guint       idle_render_id;    
};

struct _GimpSelectionEditorClass
{
  GimpEditorClass  parent_class;
};


GType       gimp_selection_editor_get_type  (void) G_GNUC_CONST;

GtkWidget * gimp_selection_editor_new       (GimpImage           *gimage);
void        gimp_selection_editor_set_image (GimpSelectionEditor *editor,
                                             GimpImage           *gimage);


#endif /* __GIMP_SELECTION_EDITOR_H__ */
