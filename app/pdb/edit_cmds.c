/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp-edit.h"
#include "core/gimp.h"
#include "core/gimpchannel.h"
#include "core/gimpcontainer.h"
#include "core/gimpdrawable-blend.h"
#include "core/gimpdrawable-bucket-fill.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimplayer.h"
#include "core/gimpprogress.h"
#include "core/gimpstrokedesc.h"
#include "gimp-intl.h"

static ProcRecord edit_cut_proc;
static ProcRecord edit_copy_proc;
static ProcRecord edit_copy_visible_proc;
static ProcRecord edit_paste_proc;
static ProcRecord edit_paste_as_new_proc;
static ProcRecord edit_named_cut_proc;
static ProcRecord edit_named_copy_proc;
static ProcRecord edit_named_copy_visible_proc;
static ProcRecord edit_named_paste_proc;
static ProcRecord edit_named_paste_as_new_proc;
static ProcRecord edit_clear_proc;
static ProcRecord edit_fill_proc;
static ProcRecord edit_bucket_fill_proc;
static ProcRecord edit_blend_proc;
static ProcRecord edit_stroke_proc;

void
register_edit_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &edit_cut_proc);
  procedural_db_register (gimp, &edit_copy_proc);
  procedural_db_register (gimp, &edit_copy_visible_proc);
  procedural_db_register (gimp, &edit_paste_proc);
  procedural_db_register (gimp, &edit_paste_as_new_proc);
  procedural_db_register (gimp, &edit_named_cut_proc);
  procedural_db_register (gimp, &edit_named_copy_proc);
  procedural_db_register (gimp, &edit_named_copy_visible_proc);
  procedural_db_register (gimp, &edit_named_paste_proc);
  procedural_db_register (gimp, &edit_named_paste_as_new_proc);
  procedural_db_register (gimp, &edit_clear_proc);
  procedural_db_register (gimp, &edit_fill_proc);
  procedural_db_register (gimp, &edit_bucket_fill_proc);
  procedural_db_register (gimp, &edit_blend_proc);
  procedural_db_register (gimp, &edit_stroke_proc);
}

static Argument *
edit_cut_invoker (Gimp         *gimp,
                  GimpContext  *context,
                  GimpProgress *progress,
                  Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gboolean non_empty = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

          non_empty = gimp_edit_cut (image, drawable, context) != NULL;
        }
    }

  return_args = procedural_db_return_args (&edit_cut_proc, success);

  if (success)
    return_args[1].value.pdb_int = non_empty;

  return return_args;
}

static ProcArg edit_cut_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to cut from"
  }
};

static ProcArg edit_cut_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "non-empty",
    "TRUE if the cut was successful, FALSE if the selection contained only transparent pixels"
  }
};

static ProcRecord edit_cut_proc =
{
  "gimp-edit-cut",
  "gimp-edit-cut",
  "Cut from the specified drawable.",
  "If there is a selection in the image, then the area specified by the selection is cut from the specified drawable and placed in an internal GIMP edit buffer. It can subsequently be retrieved using the 'gimp-edit-paste' command. If there is no selection, then the specified drawable will be removed and its contents stored in the internal GIMP edit buffer.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_cut_inargs,
  1,
  edit_cut_outargs,
  { { edit_cut_invoker } }
};

static Argument *
edit_copy_invoker (Gimp         *gimp,
                   GimpContext  *context,
                   GimpProgress *progress,
                   Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gboolean non_empty = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

          non_empty = gimp_edit_copy (image, drawable, context) != NULL;
        }
    }

  return_args = procedural_db_return_args (&edit_copy_proc, success);

  if (success)
    return_args[1].value.pdb_int = non_empty;

  return return_args;
}

static ProcArg edit_copy_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to copy from"
  }
};

static ProcArg edit_copy_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "non-empty",
    "TRUE if the copy was successful, FALSE if the selection contained only transparent pixels"
  }
};

static ProcRecord edit_copy_proc =
{
  "gimp-edit-copy",
  "gimp-edit-copy",
  "Copy from the specified drawable.",
  "If there is a selection in the image, then the area specified by the selection is copied from the specified drawable and placed in an internal GIMP edit buffer. It can subsequently be retrieved using the 'gimp-edit-paste' command. If there is no selection, then the specified drawable's contents will be stored in the internal GIMP edit buffer.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_copy_inargs,
  1,
  edit_copy_outargs,
  { { edit_copy_invoker } }
};

static Argument *
edit_copy_visible_invoker (Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gboolean non_empty = FALSE;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      non_empty = gimp_edit_copy_visible (image, context) != NULL;
    }

  return_args = procedural_db_return_args (&edit_copy_visible_proc, success);

  if (success)
    return_args[1].value.pdb_int = non_empty;

  return return_args;
}

static ProcArg edit_copy_visible_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to copy from"
  }
};

static ProcArg edit_copy_visible_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "non-empty",
    "TRUE if the copy was successful, FALSE if the selection contained only transparent pixels"
  }
};

static ProcRecord edit_copy_visible_proc =
{
  "gimp-edit-copy-visible",
  "gimp-edit-copy-visible",
  "Copy from the projection.",
  "If there is a selection in the image, then the area specified by the selection is copied from the projection and placed in an internal GIMP edit buffer. It can subsequently be retrieved using the 'gimp-edit-paste' command. If there is no selection, then the projection's contents will be stored in the internal GIMP edit buffer.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2004",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_copy_visible_inargs,
  1,
  edit_copy_visible_outargs,
  { { edit_copy_visible_invoker } }
};

static Argument *
edit_paste_invoker (Gimp         *gimp,
                    GimpContext  *context,
                    GimpProgress *progress,
                    Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gboolean paste_into;
  GimpLayer *layer = NULL;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  paste_into = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = (gimp_item_is_attached (GIMP_ITEM (drawable)) &&
                 gimp->global_buffer != NULL);

      if (success)
        {
          layer = gimp_edit_paste (gimp_item_get_image (GIMP_ITEM (drawable)),
                                   drawable, gimp->global_buffer,
                                   paste_into, -1, -1, -1, -1);
          if (! layer)
            success = FALSE;
        }
    }

  return_args = procedural_db_return_args (&edit_paste_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (layer));

  return return_args;
}

static ProcArg edit_paste_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to paste to"
  },
  {
    GIMP_PDB_INT32,
    "paste-into",
    "Clear selection, or paste behind it?"
  }
};

static ProcArg edit_paste_outargs[] =
{
  {
    GIMP_PDB_LAYER,
    "floating-sel",
    "The new floating selection"
  }
};

static ProcRecord edit_paste_proc =
{
  "gimp-edit-paste",
  "gimp-edit-paste",
  "Paste buffer to the specified drawable.",
  "This procedure pastes a copy of the internal GIMP edit buffer to the specified drawable. The GIMP edit buffer will be empty unless a call was previously made to either 'gimp-edit-cut' or 'gimp-edit-copy'. The \"paste_into\" option specifies whether to clear the current image selection, or to paste the buffer \"behind\" the selection. This allows the selection to act as a mask for the pasted buffer. Anywhere that the selection mask is non-zero, the pasted buffer will show through. The pasted buffer will be a new layer in the image which is designated as the image floating selection. If the image has a floating selection at the time of pasting, the old floating selection will be anchored to it's drawable before the new floating selection is added. This procedure returns the new floating layer. The resulting floating selection will already be attached to the specified drawable, and a subsequent call to floating_sel_attach is not needed.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  2,
  edit_paste_inargs,
  1,
  edit_paste_outargs,
  { { edit_paste_invoker } }
};

static Argument *
edit_paste_as_new_invoker (Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image = NULL;

  success = (gimp->global_buffer != NULL);

  if (success)
    {
      image = gimp_edit_paste_as_new (gimp, NULL, gimp->global_buffer);
      if (! image)
        success = FALSE;
    }

  return_args = procedural_db_return_args (&edit_paste_as_new_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_image_get_ID (image);

  return return_args;
}

static ProcArg edit_paste_as_new_outargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The new image"
  }
};

static ProcRecord edit_paste_as_new_proc =
{
  "gimp-edit-paste-as-new",
  "gimp-edit-paste-as-new",
  "Paste buffer to a new image.",
  "This procedure pastes a copy of the internal GIMP edit buffer to a new image. The GIMP edit buffer will be empty unless a call was previously made to either 'gimp-edit-cut' or 'gimp-edit-copy'. This procedure returns the new image.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  0,
  NULL,
  1,
  edit_paste_as_new_outargs,
  { { edit_paste_as_new_invoker } }
};

static Argument *
edit_named_cut_invoker (Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gchar *buffer_name;
  gchar *real_name = NULL;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  buffer_name = (gchar *) args[1].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
       success = (strlen (buffer_name) > 0 &&
                  gimp_item_is_attached (GIMP_ITEM (drawable)));

       if (success)
         {
            GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

            real_name = (gchar *) gimp_edit_named_cut (image, buffer_name,
                                                       drawable, context);

            if (real_name)
              real_name = g_strdup (real_name);
            else
              success = FALSE;
         }
    }

  return_args = procedural_db_return_args (&edit_named_cut_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = real_name;

  return return_args;
}

static ProcArg edit_named_cut_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to cut from"
  },
  {
    GIMP_PDB_STRING,
    "buffer-name",
    "The name of the buffer to create"
  }
};

static ProcArg edit_named_cut_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "real-name",
    "The real name given to the buffer, or NULL if the selection contained only transparent pixels"
  }
};

static ProcRecord edit_named_cut_proc =
{
  "gimp-edit-named-cut",
  "gimp-edit-named-cut",
  "Cut into a named buffer.",
  "This procedure works like gimp-edit-cut, but additionally stores the cut buffer into a named buffer that will stay available for later pasting, regardless of any intermediate copy or cut operations.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  2,
  edit_named_cut_inargs,
  1,
  edit_named_cut_outargs,
  { { edit_named_cut_invoker } }
};

static Argument *
edit_named_copy_invoker (Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gchar *buffer_name;
  gchar *real_name = NULL;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  buffer_name = (gchar *) args[1].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
       success = (strlen (buffer_name) > 0 &&
                  gimp_item_is_attached (GIMP_ITEM (drawable)));

       if (success)
         {
            GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

            real_name = (gchar *) gimp_edit_named_copy (image, buffer_name,
                                                        drawable, context);

            if (real_name)
              real_name = g_strdup (real_name);
            else
              success = FALSE;
         }
    }

  return_args = procedural_db_return_args (&edit_named_copy_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = real_name;

  return return_args;
}

static ProcArg edit_named_copy_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to copy from"
  },
  {
    GIMP_PDB_STRING,
    "buffer-name",
    "The name of the buffer to create"
  }
};

static ProcArg edit_named_copy_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "real-name",
    "The real name given to the buffer, or NULL if the selection contained only transparent pixels"
  }
};

static ProcRecord edit_named_copy_proc =
{
  "gimp-edit-named-copy",
  "gimp-edit-named-copy",
  "Copy into a named buffer.",
  "This procedure works like gimp-edit-copy, but additionally stores the copied buffer into a named buffer that will stay available for later pasting, regardless of any intermediate copy or cut operations.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  2,
  edit_named_copy_inargs,
  1,
  edit_named_copy_outargs,
  { { edit_named_copy_invoker } }
};

static Argument *
edit_named_copy_visible_invoker (Gimp         *gimp,
                                 GimpContext  *context,
                                 GimpProgress *progress,
                                 Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gchar *buffer_name;
  gchar *real_name = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  buffer_name = (gchar *) args[1].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
       success = (strlen (buffer_name) > 0);

       if (success)
         {
           real_name = (gchar *) gimp_edit_named_copy_visible (image, buffer_name,
                                                               context);

           if (real_name)
             real_name = g_strdup (real_name);
           else
             success = FALSE;
         }
    }

  return_args = procedural_db_return_args (&edit_named_copy_visible_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = real_name;

  return return_args;
}

static ProcArg edit_named_copy_visible_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to copy from"
  },
  {
    GIMP_PDB_STRING,
    "buffer-name",
    "The name of the buffer to create"
  }
};

static ProcArg edit_named_copy_visible_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "real-name",
    "The real name given to the buffer"
  }
};

static ProcRecord edit_named_copy_visible_proc =
{
  "gimp-edit-named-copy-visible",
  "gimp-edit-named-copy-visible",
  "Copy from the projection into a named buffer.",
  "This procedure works like gimp-edit-copy-visible, but additionally stores the copied buffer into a named buffer that will stay available for later pasting, regardless of any intermediate copy or cut operations.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  2,
  edit_named_copy_visible_inargs,
  1,
  edit_named_copy_visible_outargs,
  { { edit_named_copy_visible_invoker } }
};

static Argument *
edit_named_paste_invoker (Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gchar *buffer_name;
  gboolean paste_into;
  GimpLayer *layer = NULL;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  buffer_name = (gchar *) args[1].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  paste_into = args[2].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      GimpBuffer *buffer;

      buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      success = (buffer != NULL && gimp_item_is_attached (GIMP_ITEM (drawable)));

      if (success)
        {
          layer = gimp_edit_paste (gimp_item_get_image (GIMP_ITEM (drawable)),
                                   drawable, buffer,
                                   paste_into, -1, -1, -1, -1);
          if (! layer)
            success = FALSE;
        }
    }

  return_args = procedural_db_return_args (&edit_named_paste_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (layer));

  return return_args;
}

static ProcArg edit_named_paste_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to paste to"
  },
  {
    GIMP_PDB_STRING,
    "buffer-name",
    "The name of the buffer to paste"
  },
  {
    GIMP_PDB_INT32,
    "paste-into",
    "Clear selection, or paste behind it?"
  }
};

static ProcArg edit_named_paste_outargs[] =
{
  {
    GIMP_PDB_LAYER,
    "floating-sel",
    "The new floating selection"
  }
};

static ProcRecord edit_named_paste_proc =
{
  "gimp-edit-named-paste",
  "gimp-edit-named-paste",
  "Paste named buffer to the specified drawable.",
  "This procedure works like gimp-edit-paste but pastes a named buffer instead of the global buffer.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  3,
  edit_named_paste_inargs,
  1,
  edit_named_paste_outargs,
  { { edit_named_paste_invoker } }
};

static Argument *
edit_named_paste_as_new_invoker (Gimp         *gimp,
                                 GimpContext  *context,
                                 GimpProgress *progress,
                                 Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *buffer_name;
  GimpImage *image = NULL;

  buffer_name = (gchar *) args[0].value.pdb_pointer;
  if (buffer_name == NULL || !g_utf8_validate (buffer_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpBuffer *buffer;

      buffer = (GimpBuffer *)
        gimp_container_get_child_by_name (gimp->named_buffers, buffer_name);

      success = (buffer != NULL);

      if (success)
        {
          image = gimp_edit_paste_as_new (gimp, NULL, buffer);
          if (! image)
            success = FALSE;
        }
    }

  return_args = procedural_db_return_args (&edit_named_paste_as_new_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_image_get_ID (image);

  return return_args;
}

static ProcArg edit_named_paste_as_new_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "buffer-name",
    "The name of the buffer to paste"
  }
};

static ProcArg edit_named_paste_as_new_outargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The new image"
  }
};

static ProcRecord edit_named_paste_as_new_proc =
{
  "gimp-edit-named-paste-as-new",
  "gimp-edit-named-paste-as-new",
  "Paste named buffer to a new image.",
  "This procedure works like gimp-edit-paste-as-new but pastes a named buffer instead of the global buffer.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2005",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_named_paste_as_new_inargs,
  1,
  edit_named_paste_as_new_outargs,
  { { edit_named_paste_as_new_invoker } }
};

static Argument *
edit_clear_invoker (Gimp         *gimp,
                    GimpContext  *context,
                    GimpProgress *progress,
                    Argument     *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

          success = gimp_edit_clear (image, drawable, context);
        }
    }

  return procedural_db_return_args (&edit_clear_proc, success);
}

static ProcArg edit_clear_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to clear from"
  }
};

static ProcRecord edit_clear_proc =
{
  "gimp-edit-clear",
  "gimp-edit-clear",
  "Clear selected area of drawable.",
  "This procedure clears the specified drawable. If the drawable has an alpha channel, the cleared pixels will become transparent. If the drawable does not have an alpha channel, cleared pixels will be set to the background color. This procedure only affects regions within a selection if there is a selection active.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_clear_inargs,
  0,
  NULL,
  { { edit_clear_invoker } }
};

static Argument *
edit_fill_invoker (Gimp         *gimp,
                   GimpContext  *context,
                   GimpProgress *progress,
                   Argument     *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 fill_type;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  fill_type = args[1].value.pdb_int;
  if (fill_type < GIMP_FOREGROUND_FILL || fill_type > GIMP_PATTERN_FILL)
    success = FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));

          success = gimp_edit_fill (image, drawable, context, (GimpFillType) fill_type);
        }
    }

  return procedural_db_return_args (&edit_fill_proc, success);
}

static ProcArg edit_fill_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to fill to"
  },
  {
    GIMP_PDB_INT32,
    "fill-type",
    "The type of fill: GIMP_FOREGROUND_FILL (0), GIMP_BACKGROUND_FILL (1), GIMP_WHITE_FILL (2), GIMP_TRANSPARENT_FILL (3), GIMP_PATTERN_FILL (4)"
  }
};

static ProcRecord edit_fill_proc =
{
  "gimp-edit-fill",
  "gimp-edit-fill",
  "Fill selected area of drawable.",
  "This procedure fills the specified drawable with the fill mode. If the fill mode is foreground, the current foreground color is used. If the fill mode is background, the current background color is used. Other fill modes should not be used. This procedure only affects regions within a selection if there is a selection active. If you want to fill the whole drawable, regardless of the selection, use gimp_drawable_fill().",
  "Spencer Kimball & Peter Mattis & Raphael Quinet",
  "Spencer Kimball & Peter Mattis",
  "1995-2000",
  NULL,
  GIMP_INTERNAL,
  2,
  edit_fill_inargs,
  0,
  NULL,
  { { edit_fill_invoker } }
};

static Argument *
edit_bucket_fill_invoker (Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 fill_mode;
  gint32 paint_mode;
  gdouble opacity;
  gdouble threshold;
  gboolean sample_merged;
  gdouble x;
  gdouble y;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  fill_mode = args[1].value.pdb_int;
  if (fill_mode < GIMP_FG_BUCKET_FILL || fill_mode > GIMP_PATTERN_BUCKET_FILL)
    success = FALSE;

  paint_mode = args[2].value.pdb_int;
  if (paint_mode < GIMP_NORMAL_MODE || paint_mode > GIMP_COLOR_ERASE_MODE)
    success = FALSE;

  opacity = args[3].value.pdb_float;
  if (opacity < 0.0 || opacity > 100.0)
    success = FALSE;

  threshold = args[4].value.pdb_float;
  if (threshold < 0.0 || threshold > 255.0)
    success = FALSE;

  sample_merged = args[5].value.pdb_int ? TRUE : FALSE;

  x = args[6].value.pdb_float;

  y = args[7].value.pdb_float;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage *image = gimp_item_get_image (GIMP_ITEM (drawable));
          gboolean   do_seed_fill;

          do_seed_fill = gimp_channel_is_empty (gimp_image_get_mask (image));

          gimp_drawable_bucket_fill (drawable, context, fill_mode,
                                     paint_mode, opacity / 100.0,
                                     do_seed_fill,
                                     FALSE /* don't fill transparent */,
                                     threshold, sample_merged, x, y);
        }
    }

  return procedural_db_return_args (&edit_bucket_fill_proc, success);
}

static ProcArg edit_bucket_fill_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_INT32,
    "fill-mode",
    "The type of fill: { GIMP_FG_BUCKET_FILL (0), GIMP_BG_BUCKET_FILL (1), GIMP_PATTERN_BUCKET_FILL (2) }"
  },
  {
    GIMP_PDB_INT32,
    "paint-mode",
    "The paint application mode: { GIMP_NORMAL_MODE (0), GIMP_DISSOLVE_MODE (1), GIMP_BEHIND_MODE (2), GIMP_MULTIPLY_MODE (3), GIMP_SCREEN_MODE (4), GIMP_OVERLAY_MODE (5), GIMP_DIFFERENCE_MODE (6), GIMP_ADDITION_MODE (7), GIMP_SUBTRACT_MODE (8), GIMP_DARKEN_ONLY_MODE (9), GIMP_LIGHTEN_ONLY_MODE (10), GIMP_HUE_MODE (11), GIMP_SATURATION_MODE (12), GIMP_COLOR_MODE (13), GIMP_VALUE_MODE (14), GIMP_DIVIDE_MODE (15), GIMP_DODGE_MODE (16), GIMP_BURN_MODE (17), GIMP_HARDLIGHT_MODE (18), GIMP_SOFTLIGHT_MODE (19), GIMP_GRAIN_EXTRACT_MODE (20), GIMP_GRAIN_MERGE_MODE (21), GIMP_COLOR_ERASE_MODE (22) }"
  },
  {
    GIMP_PDB_FLOAT,
    "opacity",
    "The opacity of the final bucket fill (0 <= opacity <= 100)"
  },
  {
    GIMP_PDB_FLOAT,
    "threshold",
    "The threshold determines how extensive the seed fill will be. It's value is specified in terms of intensity levels (0 <= threshold <= 255). This parameter is only valid when there is no selection in the specified image."
  },
  {
    GIMP_PDB_INT32,
    "sample-merged",
    "Use the composite image, not the drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "x",
    "The x coordinate of this bucket fill's application. This parameter is only valid when there is no selection in the specified image."
  },
  {
    GIMP_PDB_FLOAT,
    "y",
    "The y coordinate of this bucket fill's application. This parameter is only valid when there is no selection in the specified image."
  }
};

static ProcRecord edit_bucket_fill_proc =
{
  "gimp-edit-bucket-fill",
  "gimp-edit-bucket-fill",
  "Fill the area specified either by the current selection if there is one, or by a seed fill starting at the specified coordinates.",
  "This tool requires information on the paint application mode, and the fill mode, which can either be in the foreground color, or in the currently active pattern. If there is no selection, a seed fill is executed at the specified coordinates and extends outward in keeping with the threshold parameter. If there is a selection in the target image, the threshold, sample merged, x, and y arguments are unused. If the sample_merged parameter is non-zero, the data of the composite image will be used instead of that for the specified drawable. This is equivalent to sampling for colors after merging all visible layers. In the case of merged sampling, the x and y coordinates are relative to the image's origin; otherwise, they are relative to the drawable's origin.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  8,
  edit_bucket_fill_inargs,
  0,
  NULL,
  { { edit_bucket_fill_invoker } }
};

static Argument *
edit_blend_invoker (Gimp         *gimp,
                    GimpContext  *context,
                    GimpProgress *progress,
                    Argument     *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 blend_mode;
  gint32 paint_mode;
  gint32 gradient_type;
  gdouble opacity;
  gdouble offset;
  gint32 repeat;
  gboolean reverse;
  gboolean supersample;
  gint32 max_depth;
  gdouble threshold;
  gboolean dither;
  gdouble x1;
  gdouble y1;
  gdouble x2;
  gdouble y2;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  blend_mode = args[1].value.pdb_int;
  if (blend_mode < GIMP_FG_BG_RGB_MODE || blend_mode > GIMP_CUSTOM_MODE)
    success = FALSE;

  paint_mode = args[2].value.pdb_int;
  if (paint_mode < GIMP_NORMAL_MODE || paint_mode > GIMP_COLOR_ERASE_MODE)
    success = FALSE;

  gradient_type = args[3].value.pdb_int;
  if (gradient_type < GIMP_GRADIENT_LINEAR || gradient_type > GIMP_GRADIENT_SPIRAL_ANTICLOCKWISE)
    success = FALSE;

  opacity = args[4].value.pdb_float;
  if (opacity < 0.0 || opacity > 100.0)
    success = FALSE;

  offset = args[5].value.pdb_float;
  if (offset < 0.0)
    success = FALSE;

  repeat = args[6].value.pdb_int;
  if (repeat < GIMP_REPEAT_NONE || repeat > GIMP_REPEAT_TRIANGULAR)
    success = FALSE;

  reverse = args[7].value.pdb_int ? TRUE : FALSE;

  supersample = args[8].value.pdb_int ? TRUE : FALSE;

  max_depth = args[9].value.pdb_int;
  if (supersample && (max_depth < 1 || max_depth > 9))
    success = FALSE;

  threshold = args[10].value.pdb_float;
  if (supersample && (threshold < 0.0 || threshold > 4.0))
    success = FALSE;

  dither = args[11].value.pdb_int ? TRUE : FALSE;

  x1 = args[12].value.pdb_float;

  y1 = args[13].value.pdb_float;

  x2 = args[14].value.pdb_float;

  y2 = args[15].value.pdb_float;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          if (progress)
            gimp_progress_start (progress, _("Blending"), FALSE);

          gimp_drawable_blend (drawable,
                               context,
                               blend_mode,
                               paint_mode,
                               gradient_type,
                               opacity / 100.0,
                               offset, repeat, reverse,
                               supersample, max_depth,
                               threshold, dither,
                               x1, y1, x2, y2,
                               progress);

          if (progress)
            gimp_progress_end (progress);
        }
    }

  return procedural_db_return_args (&edit_blend_proc, success);
}

static ProcArg edit_blend_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_INT32,
    "blend-mode",
    "The type of blend: { GIMP_FG_BG_RGB_MODE (0), GIMP_FG_BG_HSV_MODE (1), GIMP_FG_TRANSPARENT_MODE (2), GIMP_CUSTOM_MODE (3) }"
  },
  {
    GIMP_PDB_INT32,
    "paint-mode",
    "The paint application mode: { GIMP_NORMAL_MODE (0), GIMP_DISSOLVE_MODE (1), GIMP_BEHIND_MODE (2), GIMP_MULTIPLY_MODE (3), GIMP_SCREEN_MODE (4), GIMP_OVERLAY_MODE (5), GIMP_DIFFERENCE_MODE (6), GIMP_ADDITION_MODE (7), GIMP_SUBTRACT_MODE (8), GIMP_DARKEN_ONLY_MODE (9), GIMP_LIGHTEN_ONLY_MODE (10), GIMP_HUE_MODE (11), GIMP_SATURATION_MODE (12), GIMP_COLOR_MODE (13), GIMP_VALUE_MODE (14), GIMP_DIVIDE_MODE (15), GIMP_DODGE_MODE (16), GIMP_BURN_MODE (17), GIMP_HARDLIGHT_MODE (18), GIMP_SOFTLIGHT_MODE (19), GIMP_GRAIN_EXTRACT_MODE (20), GIMP_GRAIN_MERGE_MODE (21), GIMP_COLOR_ERASE_MODE (22) }"
  },
  {
    GIMP_PDB_INT32,
    "gradient-type",
    "The type of gradient: { GIMP_GRADIENT_LINEAR (0), GIMP_GRADIENT_BILINEAR (1), GIMP_GRADIENT_RADIAL (2), GIMP_GRADIENT_SQUARE (3), GIMP_GRADIENT_CONICAL_SYMMETRIC (4), GIMP_GRADIENT_CONICAL_ASYMMETRIC (5), GIMP_GRADIENT_SHAPEBURST_ANGULAR (6), GIMP_GRADIENT_SHAPEBURST_SPHERICAL (7), GIMP_GRADIENT_SHAPEBURST_DIMPLED (8), GIMP_GRADIENT_SPIRAL_CLOCKWISE (9), GIMP_GRADIENT_SPIRAL_ANTICLOCKWISE (10) }"
  },
  {
    GIMP_PDB_FLOAT,
    "opacity",
    "The opacity of the final blend (0 <= opacity <= 100)"
  },
  {
    GIMP_PDB_FLOAT,
    "offset",
    "Offset relates to the starting and ending coordinates specified for the blend. This parameter is mode dependent (0 <= offset)"
  },
  {
    GIMP_PDB_INT32,
    "repeat",
    "Repeat mode: { GIMP_REPEAT_NONE (0), GIMP_REPEAT_SAWTOOTH (1), GIMP_REPEAT_TRIANGULAR (2) }"
  },
  {
    GIMP_PDB_INT32,
    "reverse",
    "Use the reverse gradient (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Do adaptive supersampling (TRUE or FALSE)"
  },
  {
    GIMP_PDB_INT32,
    "max-depth",
    "Maximum recursion levels for supersampling"
  },
  {
    GIMP_PDB_FLOAT,
    "threshold",
    "Supersampling threshold"
  },
  {
    GIMP_PDB_INT32,
    "dither",
    "Use dithering to reduce banding (TRUE or FALSE)"
  },
  {
    GIMP_PDB_FLOAT,
    "x1",
    "The x coordinate of this blend's starting point"
  },
  {
    GIMP_PDB_FLOAT,
    "y1",
    "The y coordinate of this blend's starting point"
  },
  {
    GIMP_PDB_FLOAT,
    "x2",
    "The x coordinate of this blend's ending point"
  },
  {
    GIMP_PDB_FLOAT,
    "y2",
    "The y coordinate of this blend's ending point"
  }
};

static ProcRecord edit_blend_proc =
{
  "gimp-edit-blend",
  "gimp-edit-blend",
  "Blend between the starting and ending coordinates with the specified blend mode and gradient type.",
  "This tool requires information on the paint application mode, the blend mode, and the gradient type. It creates the specified variety of blend using the starting and ending coordinates as defined for each gradient type.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  16,
  edit_blend_inargs,
  0,
  NULL,
  { { edit_blend_invoker } }
};

static Argument *
edit_stroke_invoker (Gimp         *gimp,
                     GimpContext  *context,
                     GimpProgress *progress,
                     Argument     *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpImage      *image = gimp_item_get_image (GIMP_ITEM (drawable));
          GimpStrokeDesc *desc  = gimp_stroke_desc_new (gimp, context);

          g_object_set (desc, "method", GIMP_STROKE_METHOD_PAINT_CORE, NULL);

          success = gimp_item_stroke (GIMP_ITEM (gimp_image_get_mask (image)),
                                      drawable, context, desc, TRUE);

          g_object_unref (desc);
        }
    }

  return procedural_db_return_args (&edit_stroke_proc, success);
}

static ProcArg edit_stroke_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The drawable to stroke to"
  }
};

static ProcRecord edit_stroke_proc =
{
  "gimp-edit-stroke",
  "gimp-edit-stroke",
  "Stroke the current selection",
  "This procedure strokes the current selection, painting along the selection boundary with the active brush and foreground color. The paint is applied to the specified drawable regardless of the active selection.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  edit_stroke_inargs,
  0,
  NULL,
  { { edit_stroke_invoker } }
};
