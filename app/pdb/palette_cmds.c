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

#include "libgimpcolor/gimpcolor.h"

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "core/gimppalette.h"

static ProcRecord palette_new_proc;
static ProcRecord palette_duplicate_proc;
static ProcRecord palette_rename_proc;
static ProcRecord palette_delete_proc;
static ProcRecord palette_get_info_proc;
static ProcRecord palette_add_entry_proc;
static ProcRecord palette_delete_entry_proc;
static ProcRecord palette_entry_get_color_proc;
static ProcRecord palette_entry_set_color_proc;
static ProcRecord palette_entry_get_name_proc;
static ProcRecord palette_entry_set_name_proc;

void
register_palette_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &palette_new_proc);
  procedural_db_register (gimp, &palette_duplicate_proc);
  procedural_db_register (gimp, &palette_rename_proc);
  procedural_db_register (gimp, &palette_delete_proc);
  procedural_db_register (gimp, &palette_get_info_proc);
  procedural_db_register (gimp, &palette_add_entry_proc);
  procedural_db_register (gimp, &palette_delete_entry_proc);
  procedural_db_register (gimp, &palette_entry_get_color_proc);
  procedural_db_register (gimp, &palette_entry_set_color_proc);
  procedural_db_register (gimp, &palette_entry_get_name_proc);
  procedural_db_register (gimp, &palette_entry_set_name_proc);
}

static Argument *
palette_new_invoker (Gimp         *gimp,
                     GimpContext  *context,
                     GimpProgress *progress,
                     Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  GimpData *data = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (strlen (name))
        data = gimp_data_factory_data_new (gimp->palette_factory, name);

      success = (data != NULL);
    }

  return_args = procedural_db_return_args (&palette_new_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (data)->name);

  return return_args;
}

static ProcArg palette_new_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The requested name of the new palette"
  }
};

static ProcArg palette_new_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The actual new palette name"
  }
};

static ProcRecord palette_new_proc =
{
  "gimp_palette_new",
  "Creates a new palette",
  "This procedure creates a new, uninitialized palette",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  1,
  palette_new_inargs,
  1,
  palette_new_outargs,
  { { palette_new_invoker } }
};

static Argument *
palette_duplicate_invoker (Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  GimpPalette *palette = NULL;
  GimpPalette *palette_copy = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette)
        {
          palette_copy = (GimpPalette *)
            gimp_data_factory_data_duplicate (gimp->palette_factory,
                                              GIMP_DATA (palette));

          success = (palette_copy != NULL);
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palette_duplicate_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (palette_copy)->name);

  return return_args;
}

static ProcArg palette_duplicate_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  }
};

static ProcArg palette_duplicate_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the palette's copy"
  }
};

static ProcRecord palette_duplicate_proc =
{
  "gimp_palette_duplicate",
  "Duplicates a palette",
  "This procedure creates an identical palette by a different name",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  1,
  palette_duplicate_inargs,
  1,
  palette_duplicate_outargs,
  { { palette_duplicate_invoker } }
};

static Argument *
palette_rename_invoker (Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gchar *new_name;
  GimpPalette *palette = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  new_name = (gchar *) args[1].value.pdb_pointer;
  if (new_name == NULL || !g_utf8_validate (new_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->writable)
        gimp_object_set_name (GIMP_OBJECT (palette), new_name);
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palette_rename_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (palette)->name);

  return return_args;
}

static ProcArg palette_rename_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_STRING,
    "new_name",
    "The new name of the palette"
  }
};

static ProcArg palette_rename_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The actual new name of the palette"
  }
};

static ProcRecord palette_rename_proc =
{
  "gimp_palette_rename",
  "Rename a palette",
  "This procedure renames a palette",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  2,
  palette_rename_inargs,
  1,
  palette_rename_outargs,
  { { palette_rename_invoker } }
};

static Argument *
palette_delete_invoker (Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gboolean success = TRUE;
  gchar *name;
  GimpPalette *palette = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->deletable)
        {
          GError *error = NULL;

          success = gimp_data_factory_data_delete (gimp->palette_factory,
                                                   GIMP_DATA (palette),
                                                   TRUE, &error);

          if (! success)
            {
              g_message (error->message);
              g_clear_error (&error);
            }
        }
      else
        success = FALSE;
    }

  return procedural_db_return_args (&palette_delete_proc, success);
}

static ProcArg palette_delete_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  }
};

static ProcRecord palette_delete_proc =
{
  "gimp_palette_delete",
  "Deletes a palette",
  "This procedure deletes a palette",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  1,
  palette_delete_inargs,
  0,
  NULL,
  { { palette_delete_invoker } }
};

static Argument *
palette_get_info_invoker (Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  GimpPalette *palette = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      success = (palette != NULL);
    }

  return_args = procedural_db_return_args (&palette_get_info_proc, success);

  if (success)
    return_args[1].value.pdb_int = palette->n_colors;

  return return_args;
}

static ProcArg palette_get_info_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  }
};

static ProcArg palette_get_info_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_colors",
    "The number of colors in the palette"
  }
};

static ProcRecord palette_get_info_proc =
{
  "gimp_palette_get_info",
  "Retrieve information about the specified palette.",
  "This procedure retrieves information about the specified palette. This includes the name, and the number of colors.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  1,
  palette_get_info_inargs,
  1,
  palette_get_info_outargs,
  { { palette_get_info_invoker } }
};

static Argument *
palette_add_entry_invoker (Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gchar *entry_name;
  GimpRGB color;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_name = (gchar *) args[1].value.pdb_pointer;
  if (entry_name && !g_utf8_validate (entry_name, -1, NULL))
    success = FALSE;

  color = args[2].value.pdb_color;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->writable)
        entry = gimp_palette_add_entry (palette, entry_name, &color);
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palette_add_entry_proc, success);

  if (success)
    return_args[1].value.pdb_int = entry->position;

  return return_args;
}

static ProcArg palette_add_entry_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_STRING,
    "entry_name",
    "The name of the entry"
  },
  {
    GIMP_PDB_COLOR,
    "color",
    "The new entry's color color"
  }
};

static ProcArg palette_add_entry_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The index of the added entry"
  }
};

static ProcRecord palette_add_entry_proc =
{
  "gimp_palette_add_entry",
  "Adds a palette entry to the specified palette.",
  "This procedure adds an entry to the specifed palette. It returns an error if the entry palette does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  3,
  palette_add_entry_inargs,
  1,
  palette_add_entry_outargs,
  { { palette_add_entry_invoker } }
};

static Argument *
palette_delete_entry_invoker (Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  gchar *name;
  gint32 entry_num;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->writable)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            {
              entry = g_list_nth_data (palette->colors, entry_num);

              gimp_palette_delete_entry (palette, entry);
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return procedural_db_return_args (&palette_delete_entry_proc, success);
}

static ProcArg palette_delete_entry_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The index of the added entry"
  }
};

static ProcRecord palette_delete_entry_proc =
{
  "gimp_palette_delete_entry",
  "Deletes a palette entry from the specified palette.",
  "This procedure deletes an entry from the specifed palette. It returns an error if the entry palette does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  2,
  palette_delete_entry_inargs,
  0,
  NULL,
  { { palette_delete_entry_invoker } }
};

static Argument *
palette_entry_get_color_invoker (Gimp         *gimp,
                                 GimpContext  *context,
                                 GimpProgress *progress,
                                 Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gint32 entry_num;
  GimpRGB color;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            {
              entry = g_list_nth_data (palette->colors, entry_num);

              color = entry->color;
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palette_entry_get_color_proc, success);

  if (success)
    return_args[1].value.pdb_color = color;

  return return_args;
}

static ProcArg palette_entry_get_color_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The entry to retrieve"
  }
};

static ProcArg palette_entry_get_color_outargs[] =
{
  {
    GIMP_PDB_COLOR,
    "color",
    "The color requested"
  }
};

static ProcRecord palette_entry_get_color_proc =
{
  "gimp_palette_entry_get_color",
  "Gets the specified palette entry from the specified palette.",
  "This procedure retrieves the color of the zero-based entry specifed for the specified palette. It returns an error if the entry does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  2,
  palette_entry_get_color_inargs,
  1,
  palette_entry_get_color_outargs,
  { { palette_entry_get_color_invoker } }
};

static Argument *
palette_entry_set_color_invoker (Gimp         *gimp,
                                 GimpContext  *context,
                                 GimpProgress *progress,
                                 Argument     *args)
{
  gboolean success = TRUE;
  gchar *name;
  gint32 entry_num;
  GimpRGB color;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  color = args[2].value.pdb_color;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->writable)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            {
              entry = g_list_nth_data (palette->colors, entry_num);

              entry->color = color;

              gimp_data_dirty (GIMP_DATA (palette));
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return procedural_db_return_args (&palette_entry_set_color_proc, success);
}

static ProcArg palette_entry_set_color_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The entry to retrieve"
  },
  {
    GIMP_PDB_COLOR,
    "color",
    "The new color"
  }
};

static ProcRecord palette_entry_set_color_proc =
{
  "gimp_palette_entry_set_color",
  "Sets the specified palette entry in the specified palette.",
  "This procedure sets the color of the zero-based entry specifed for the specified palette. It returns an error if the entry does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  3,
  palette_entry_set_color_inargs,
  0,
  NULL,
  { { palette_entry_set_color_invoker } }
};

static Argument *
palette_entry_get_name_invoker (Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gint32 entry_num;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            entry = g_list_nth_data (palette->colors, entry_num);
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palette_entry_get_name_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (entry->name);

  return return_args;
}

static ProcArg palette_entry_get_name_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The entry to retrieve"
  }
};

static ProcArg palette_entry_get_name_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "entry_name",
    "The name requested"
  }
};

static ProcRecord palette_entry_get_name_proc =
{
  "gimp_palette_entry_get_name",
  "Gets the specified palette entry from the specified palette.",
  "This procedure retrieves the name of the zero-based entry specifed for the specified palette. It returns an error if the entry does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  2,
  palette_entry_get_name_inargs,
  1,
  palette_entry_get_name_outargs,
  { { palette_entry_get_name_invoker } }
};

static Argument *
palette_entry_set_name_invoker (Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  gchar *name;
  gint32 entry_num;
  gchar *entry_name;
  GimpPalette *palette = NULL;
  GimpPaletteEntry *entry = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  entry_name = (gchar *) args[2].value.pdb_pointer;
  if (entry_name && !g_utf8_validate (entry_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      palette = (GimpPalette *)
        gimp_container_get_child_by_name (gimp->palette_factory->container, name);

      if (palette && GIMP_DATA (palette)->writable)
        {
          if (entry_num >= 0 && entry_num < palette->n_colors)
            {
              entry = g_list_nth_data (palette->colors, entry_num);

              g_free (entry->name);
              entry->name = g_strdup (entry_name);

              gimp_data_dirty (GIMP_DATA (palette));
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return procedural_db_return_args (&palette_entry_set_name_proc, success);
}

static ProcArg palette_entry_set_name_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name."
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The entry to retrieve"
  },
  {
    GIMP_PDB_STRING,
    "entry_name",
    "The new name"
  }
};

static ProcRecord palette_entry_set_name_proc =
{
  "gimp_palette_entry_set_name",
  "Sets the specified palette entry in the specified palette.",
  "This procedure sets the name of the zero-based entry specifed for the specified palette. It returns an error if the entry does not exist.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2004",
  NULL,
  GIMP_INTERNAL,
  3,
  palette_entry_set_name_inargs,
  0,
  NULL,
  { { palette_entry_set_name_invoker } }
};
