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


#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"
#include "core/gimpdatafactory.h"

static ProcRecord palettes_popup_proc;
static ProcRecord palettes_close_popup_proc;
static ProcRecord palettes_set_popup_proc;

void
register_palette_select_procs (Gimp *gimp)
{
  ProcRecord *procedure;

  /*
   * palettes_popup
   */
  procedure = procedural_db_init_proc (&palettes_popup_proc, 3, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("palette-callback",
                                                      "palette callback",
                                                      "The callback PDB proc to call when palette selection is made",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("popup-title",
                                                      "popup title",
                                                      "Title to give the palette popup window",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("initial-palette",
                                                      "initial palette",
                                                      "The name of the palette to set as the first selected",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * palettes_close_popup
   */
  procedure = procedural_db_init_proc (&palettes_close_popup_proc, 1, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("palette-callback",
                                                      "palette callback",
                                                      "The name of the callback registered for this popup",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * palettes_set_popup
   */
  procedure = procedural_db_init_proc (&palettes_set_popup_proc, 2, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("palette-callback",
                                                      "palette callback",
                                                      "The name of the callback registered for this popup",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("palette-name",
                                                      "palette name",
                                                      "The name of the palette to set as selected",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

}

static Argument *
palettes_popup_invoker (ProcRecord   *proc_record,
                        Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gboolean success = TRUE;
  gchar *palette_callback;
  gchar *popup_title;
  gchar *initial_palette;

  palette_callback = (gchar *) args[0].value.pdb_pointer;
  if (palette_callback == NULL || !g_utf8_validate (palette_callback, -1, NULL))
    success = FALSE;

  popup_title = (gchar *) args[1].value.pdb_pointer;
  if (popup_title == NULL || !g_utf8_validate (popup_title, -1, NULL))
    success = FALSE;

  initial_palette = (gchar *) args[2].value.pdb_pointer;
  if (initial_palette && !g_utf8_validate (initial_palette, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, palette_callback) ||
          ! gimp_pdb_dialog_new (gimp, context, gimp->palette_factory->container,
                                 popup_title, palette_callback, initial_palette,
                                 NULL))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord palettes_popup_proc =
{
  "gimp-palettes-popup",
  "gimp-palettes-popup",
  "Invokes the Gimp palette selection.",
  "This procedure popups the palette selection dialog.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2002",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_popup_invoker } }
};

static Argument *
palettes_close_popup_invoker (ProcRecord   *proc_record,
                              Gimp         *gimp,
                              GimpContext  *context,
                              GimpProgress *progress,
                              Argument     *args)
{
  gboolean success = TRUE;
  gchar *palette_callback;

  palette_callback = (gchar *) args[0].value.pdb_pointer;
  if (palette_callback == NULL || !g_utf8_validate (palette_callback, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, palette_callback) ||
          ! gimp_pdb_dialog_close (gimp, gimp->palette_factory->container,
                                   palette_callback))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord palettes_close_popup_proc =
{
  "gimp-palettes-close-popup",
  "gimp-palettes-close-popup",
  "Popdown the Gimp palette selection.",
  "This procedure closes an opened palette selection dialog.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2002",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_close_popup_invoker } }
};

static Argument *
palettes_set_popup_invoker (ProcRecord   *proc_record,
                            Gimp         *gimp,
                            GimpContext  *context,
                            GimpProgress *progress,
                            Argument     *args)
{
  gboolean success = TRUE;
  gchar *palette_callback;
  gchar *palette_name;

  palette_callback = (gchar *) args[0].value.pdb_pointer;
  if (palette_callback == NULL || !g_utf8_validate (palette_callback, -1, NULL))
    success = FALSE;

  palette_name = (gchar *) args[1].value.pdb_pointer;
  if (palette_name == NULL || !g_utf8_validate (palette_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, palette_callback) ||
          ! gimp_pdb_dialog_set (gimp, gimp->palette_factory->container,
                                 palette_callback, palette_name,
                                 NULL))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord palettes_set_popup_proc =
{
  "gimp-palettes-set-popup",
  "gimp-palettes-set-popup",
  "Sets the current palette selection in a popup.",
  "Sets the current palette selection in a popup.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer",
  "2002",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { palettes_set_popup_invoker } }
};
