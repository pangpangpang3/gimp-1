/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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

#include "procedural_db.h"

#include "gimphelp.h"

static ProcRecord help_proc;

void
register_gimphelp_procs (void)
{
  procedural_db_register (&help_proc);
}

static Argument *
help_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *path;

  path = (gchar *) args[0].value.pdb_pointer;
  if (path == NULL)
    success = FALSE;

  if (success)
    gimp_help (path);

  return procedural_db_return_args (&help_proc, success);
}

static ProcArg help_inargs[] =
{
  {
    PDB_STRING,
    "path",
    "The location of the help page"
  }
};

static ProcRecord help_proc =
{
  "gimp_help",
  "Load a help page.",
  "This procedure loads the specified help page into the helpbrowser or what ever is configured as help viewer. The location of the jelp page is given relative to the help rootdir.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "1999",
  PDB_INTERNAL,
  1,
  help_inargs,
  0,
  NULL,
  { { help_invoker } }
};
