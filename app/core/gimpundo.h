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

#ifndef __GIMP_UNDO_H__
#define __GIMP_UNDO_H__


#include "gimpviewable.h"


#define GIMP_UNDO_PREVIEW_SIZE GIMP_PREVIEW_SIZE_EXTRA_LARGE


struct _GimpUndoAccumulator
{
  gboolean mode_changed;
  gboolean size_changed;
  gboolean resolution_changed;
  gboolean unit_changed;
  gboolean mask_changed;
  gboolean qmask_changed;
  gboolean alpha_changed;
};


#define GIMP_TYPE_UNDO            (gimp_undo_get_type ())
#define GIMP_UNDO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_UNDO, GimpUndo))
#define GIMP_UNDO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_UNDO, GimpUndoClass))
#define GIMP_IS_UNDO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_UNDO))
#define GIMP_IS_UNDO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_UNDO))
#define GIMP_UNDO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_UNDO, GimpUndoClass))


typedef struct _GimpUndoClass GimpUndoClass;

struct _GimpUndo
{
  GimpViewable      parent_instance;

  GimpImage        *gimage;         /* the image this undo is part of     */

  GimpUndoType      undo_type;      /* undo type                          */
  gpointer          data;           /* data to implement the undo         */
  gsize             size;           /* size of undo item                  */
  gboolean          dirties_image;  /* TRUE if undo mutates image         */

  GimpUndoPopFunc   pop_func;       /* function pointer to undo pop proc  */
  GimpUndoFreeFunc  free_func;      /* function pointer to free undo data */
  
  TempBuf          *preview;
  guint             preview_idle_id;
};

struct _GimpUndoClass
{
  GimpViewableClass  parent_class;

  void (* pop)  (GimpUndo            *undo,
                 GimpUndoMode         undo_mode,
                 GimpUndoAccumulator *accum);
  void (* free) (GimpUndo            *undo,
                 GimpUndoMode         undo_mode);
};


GType      gimp_undo_get_type       (void) G_GNUC_CONST;

GimpUndo * gimp_undo_new            (GimpImage           *gimage,
                                     GimpUndoType         undo_type,
                                     const gchar         *name,
                                     gpointer             data,
                                     gsize                size,
                                     gboolean             dirties_image,
                                     GimpUndoPopFunc      pop_func,
                                     GimpUndoFreeFunc     free_func);

void       gimp_undo_pop            (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode,
                                     GimpUndoAccumulator *accum);
void       gimp_undo_free           (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode);

void       gimp_undo_create_preview (GimpUndo            *undo,
                                     gboolean             create_now);


#endif /* __GIMP_UNDO_H__ */
