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

#include "core/gimp.h"
#include "plug-in/plug-in.h"
#include "plug-in/plug-ins.h"

static ProcRecord help_proc;

void
register_help_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &help_proc);
}

static Argument *
help_invoker (Gimp         *gimp,
              GimpContext  *context,
              GimpProgress *progress,
              Argument     *args)
{
  gboolean success = TRUE;
  gchar *help_domain;
  gchar *help_id;

  help_domain = (gchar *) args[0].value.pdb_pointer;
  if (help_domain && !g_utf8_validate (help_domain, -1, NULL))
    success = FALSE;

  help_id = (gchar *) args[1].value.pdb_pointer;
  if (help_id == NULL || !g_utf8_validate (help_id, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (! help_domain && gimp->current_plug_in)
        help_domain = (gchar *)
          plug_ins_help_domain (gimp, gimp->current_plug_in->prog, NULL);

      gimp_help (gimp, help_domain, help_id);
    }

  return procedural_db_return_args (&help_proc, success);
}

static ProcArg help_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "help_domain",
    "The help domain in which help_id is registered"
  },
  {
    GIMP_PDB_STRING,
    "help_id",
    "The help page's ID"
  }
};

static ProcRecord help_proc =
{
  "gimp_help",
  "Load a help page.",
  "This procedure loads the specified help page into the helpbrowser or what ever is configured as help viewer. The help page is identified by its domain and ID: if help_domain is NULL, we use the help_domain which was registered using the gimp-plugin-help-register procedure. If help_domain is NULL and no help domain was registered, the help domain of the main GIMP installation is used.",
  "Michael Natterer <mitch@gimp.org>",
  "Michael Natterer <mitch@gimp.org>",
  "2000",
  NULL,
  GIMP_INTERNAL,
  2,
  help_inargs,
  0,
  NULL,
  { { help_invoker } }
};
