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
#include "core/gimpcontainer-filter.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "core/gimppalette.h"

static ProcRecord palettes_refresh_proc;
static ProcRecord palettes_get_list_proc;
static ProcRecord palettes_get_palette_proc;
static ProcRecord palettes_get_palette_entry_proc;

void
register_palettes_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &palettes_refresh_proc);
  procedural_db_register (gimp, &palettes_get_list_proc);
  procedural_db_register (gimp, &palettes_get_palette_proc);
  procedural_db_register (gimp, &palettes_get_palette_entry_proc);
}

static Argument *
palettes_refresh_invoker (Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gimp_data_factory_data_save (gimp->palette_factory);
  gimp_data_factory_data_init (gimp->palette_factory, FALSE);
  return procedural_db_return_args (&palettes_refresh_proc, TRUE);
}

static ProcRecord palettes_refresh_proc =
{
  "gimp_palettes_refresh",
  "Refreshes current palettes. This function always succeeds.",
  "This procedure retrieves all palettes currently in the user's palette path and updates the palette dialogs accordingly.",
  "Adrian Likins <adrian@gimp.org>",
  "Adrian Likins",
  "1998",
  NULL,
  GIMP_INTERNAL,
  0,
  NULL,
  0,
  NULL,
  { { palettes_refresh_invoker } }
};

static Argument *
palettes_get_list_invoker (Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *filter;
  gint32 num_palettes;
  gchar **palette_list = NULL;

  filter = (gchar *) args[0].value.pdb_pointer;
  if (filter && !g_utf8_validate (filter, -1, NULL))
    success = FALSE;

  if (success)
    palette_list = gimp_container_get_filtered_name_array (gimp->palette_factory->container, filter, &num_palettes);

  return_args = procedural_db_return_args (&palettes_get_list_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = num_palettes;
      return_args[2].value.pdb_pointer = palette_list;
    }

  return return_args;
}

static ProcArg palettes_get_list_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "filter",
    "An optional regular expression used to filter the list"
  }
};

static ProcArg palettes_get_list_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "num_palettes",
    "The number of palettes in the list"
  },
  {
    GIMP_PDB_STRINGARRAY,
    "palette_list",
    "The list of palette names"
  }
};

static ProcRecord palettes_get_list_proc =
{
  "gimp_palettes_get_list",
  "Retrieves a list of all of the available palettes",
  "This procedure returns a complete listing of available palettes. Each name returned can be used as input to the command 'gimp_context_set_palette'.",
  "Nathan Summers <rock@gimp.org>",
  "Nathan Summers",
  "2001",
  NULL,
  GIMP_INTERNAL,
  1,
  palettes_get_list_inargs,
  2,
  palettes_get_list_outargs,
  { { palettes_get_list_invoker } }
};

static Argument *
palettes_get_palette_invoker (Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpPalette *palette;

  success = (palette = gimp_context_get_palette (context)) != NULL;

  return_args = procedural_db_return_args (&palettes_get_palette_proc, success);

  if (success)
    {
      return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (palette)->name);
      return_args[2].value.pdb_int = palette->n_colors;
    }

  return return_args;
}

static ProcArg palettes_get_palette_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name"
  },
  {
    GIMP_PDB_INT32,
    "num_colors",
    "The palette num_colors"
  }
};

static ProcRecord palettes_get_palette_proc =
{
  "gimp_palettes_get_palette",
  "This procedure is deprecated! Use 'gimp_context_get_palette' instead.",
  "This procedure is deprecated! Use 'gimp_context_get_palette' instead.",
  "",
  "",
  "",
  "gimp_context_get_palette",
  GIMP_INTERNAL,
  0,
  NULL,
  2,
  palettes_get_palette_outargs,
  { { palettes_get_palette_invoker } }
};

static Argument *
palettes_get_palette_entry_invoker (Gimp         *gimp,
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

  name = (gchar *) args[0].value.pdb_pointer;
  if (name && !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  entry_num = args[1].value.pdb_int;

  if (success)
    {
      if (name && strlen (name))
      {
        palette = (GimpPalette *)
          gimp_container_get_child_by_name (gimp->palette_factory->container,
                                            name);
      }
    else
      {
        palette = gimp_context_get_palette (context);
      }

      if (palette)
        {
          if (entry_num < 0 || entry_num >= palette->n_colors) 
            {
              success = FALSE;
            }
          else
            {
              GimpPaletteEntry *entry;

              entry = (GimpPaletteEntry *)
                g_list_nth_data (palette->colors, entry_num);

              color = entry->color;
            }
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (&palettes_get_palette_entry_proc, success);

  if (success)
    {
      return_args[1].value.pdb_pointer = g_strdup (GIMP_OBJECT (palette)->name);
      return_args[2].value.pdb_int = palette->n_colors;
      return_args[3].value.pdb_color = color;
    }

  return return_args;
}

static ProcArg palettes_get_palette_entry_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name (\"\" means currently active palette)"
  },
  {
    GIMP_PDB_INT32,
    "entry_num",
    "The entry to retrieve"
  }
};

static ProcArg palettes_get_palette_entry_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The palette name"
  },
  {
    GIMP_PDB_INT32,
    "num_colors",
    "The palette num_colors"
  },
  {
    GIMP_PDB_COLOR,
    "color",
    "The color requested"
  }
};

static ProcRecord palettes_get_palette_entry_proc =
{
  "gimp_palettes_get_palette_entry",
  "This procedure is deprecated! Use 'gimp_palette_entry_get_color' instead.",
  "This procedure is deprecated! Use 'gimp_palette_entry_get_color' instead.",
  "",
  "",
  "",
  "gimp_palette_entry_get_color",
  GIMP_INTERNAL,
  2,
  palettes_get_palette_entry_inargs,
  3,
  palettes_get_palette_entry_outargs,
  { { palettes_get_palette_entry_invoker } }
};
