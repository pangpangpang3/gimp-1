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
#include "core/gimpgradient.h"

static ProcRecord gradients_popup_proc;
static ProcRecord gradients_close_popup_proc;
static ProcRecord gradients_set_popup_proc;

void
register_gradient_select_procs (Gimp *gimp)
{
  ProcRecord *procedure;

  /*
   * gradients_popup
   */
  procedure = procedural_db_init_proc (&gradients_popup_proc, 4, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("gradient-callback",
                                                      "gradient callback",
                                                      "The callback PDB proc to call when gradient selection is made",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("popup-title",
                                                      "popup title",
                                                      "Title to give the gradient popup window",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("initial-gradient",
                                                      "initial gradient",
                                                      "The name of the pattern to set as the first selected",
                                                      FALSE, TRUE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_INT32,
                              g_param_spec_int ("sample-size",
                                                "sample size",
                                                "Size of the sample to return when the gradient is changed: (1 <= sample_size <= 10000)",
                                                1, 10000, 1,
                                                GIMP_PARAM_READWRITE | GIMP_PARAM_NO_VALIDATE));
  procedural_db_register (gimp, procedure);

  /*
   * gradients_close_popup
   */
  procedure = procedural_db_init_proc (&gradients_close_popup_proc, 1, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("gradient-callback",
                                                      "gradient callback",
                                                      "The name of the callback registered for this popup",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

  /*
   * gradients_set_popup
   */
  procedure = procedural_db_init_proc (&gradients_set_popup_proc, 2, 0);
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("gradient-callback",
                                                      "gradient callback",
                                                      "The name of the callback registered for this popup",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_add_argument (procedure,
                              GIMP_PDB_STRING,
                              gimp_param_spec_string ("gradient-name",
                                                      "gradient name",
                                                      "The name of the gradient to set as selected",
                                                      FALSE, FALSE,
                                                      NULL,
                                                      GIMP_PARAM_READWRITE));
  procedural_db_register (gimp, procedure);

}

static Argument *
gradients_popup_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  gchar *gradient_callback;
  gchar *popup_title;
  gchar *initial_gradient;
  gint32 sample_size;

  gradient_callback = (gchar *) args[0].value.pdb_pointer;
  if (gradient_callback == NULL || !g_utf8_validate (gradient_callback, -1, NULL))
    success = FALSE;

  popup_title = (gchar *) args[1].value.pdb_pointer;
  if (popup_title == NULL || !g_utf8_validate (popup_title, -1, NULL))
    success = FALSE;

  initial_gradient = (gchar *) args[2].value.pdb_pointer;
  if (initial_gradient && !g_utf8_validate (initial_gradient, -1, NULL))
    success = FALSE;

  sample_size = args[3].value.pdb_int;

  if (success)
    {
      if (sample_size < 1 || sample_size > 10000)
        sample_size = GIMP_GRADIENT_DEFAULT_SAMPLE_SIZE;

      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, gradient_callback) ||
          ! gimp_pdb_dialog_new (gimp, context, gimp->gradient_factory->container,
                                 popup_title, gradient_callback, initial_gradient,
                                 "sample-size", sample_size,
                                 NULL))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord gradients_popup_proc =
{
  "gimp-gradients-popup",
  "gimp-gradients-popup",
  "Invokes the Gimp gradients selection.",
  "This procedure popups the gradients selection dialog.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { gradients_popup_invoker } }
};

static Argument *
gradients_close_popup_invoker (ProcRecord   *proc_record,
                               Gimp         *gimp,
                               GimpContext  *context,
                               GimpProgress *progress,
                               Argument     *args)
{
  gboolean success = TRUE;
  gchar *gradient_callback;

  gradient_callback = (gchar *) args[0].value.pdb_pointer;
  if (gradient_callback == NULL || !g_utf8_validate (gradient_callback, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, gradient_callback) ||
          ! gimp_pdb_dialog_close (gimp, gimp->gradient_factory->container,
                                   gradient_callback))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord gradients_close_popup_proc =
{
  "gimp-gradients-close-popup",
  "gimp-gradients-close-popup",
  "Popdown the Gimp gradient selection.",
  "This procedure closes an opened gradient selection dialog.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { gradients_close_popup_invoker } }
};

static Argument *
gradients_set_popup_invoker (ProcRecord   *proc_record,
                             Gimp         *gimp,
                             GimpContext  *context,
                             GimpProgress *progress,
                             Argument     *args)
{
  gboolean success = TRUE;
  gchar *gradient_callback;
  gchar *gradient_name;

  gradient_callback = (gchar *) args[0].value.pdb_pointer;
  if (gradient_callback == NULL || !g_utf8_validate (gradient_callback, -1, NULL))
    success = FALSE;

  gradient_name = (gchar *) args[1].value.pdb_pointer;
  if (gradient_name == NULL || !g_utf8_validate (gradient_name, -1, NULL))
    success = FALSE;

  if (success)
    {
      if (gimp->no_interface ||
          ! procedural_db_lookup (gimp, gradient_callback) ||
          ! gimp_pdb_dialog_set (gimp, gimp->gradient_factory->container,
                                 gradient_callback, gradient_name,
                                 NULL))
        success = FALSE;
    }

  return procedural_db_return_values (proc_record, success);
}

static ProcRecord gradients_set_popup_proc =
{
  "gimp-gradients-set-popup",
  "gimp-gradients-set-popup",
  "Sets the current gradient selection in a popup.",
  "Sets the current gradient selection in a popup.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  NULL,
  GIMP_INTERNAL,
  0, NULL, 0, NULL,
  { { gradients_set_popup_invoker } }
};
