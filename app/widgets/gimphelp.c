/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimphelp.c
 * Copyright (C) 1999-2004 Michael Natterer <mitch@gimp.org>
 *                         Henrik Brix Andersen <brix@gimp.org>
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

#include "config.h"

#include <string.h>

#include <gtk/gtk.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "widgets-types.h"

#include "config/gimpguiconfig.h"

#include "core/gimp.h"
#include "core/gimp-utils.h"

#include "pdb/procedural_db.h"

#include "plug-in/plug-ins.h"
#include "plug-in/plug-in-run.h"

#include "gimphelp.h"
#include "gimphelp-ids.h"

#include "gimp-intl.h"


/*  #define GIMP_HELP_DEBUG  */


typedef struct _GimpIdleHelp GimpIdleHelp;

struct _GimpIdleHelp
{
  Gimp  *gimp;
  gchar *help_domain;
  gchar *help_locales;
  gchar *help_id;
};


/*  local function prototypes  */

static gint      gimp_idle_help      (gpointer     data);
static gboolean  gimp_help_internal  (Gimp        *gimp);
static void      gimp_help_call      (Gimp        *gimp,
                                      const gchar *procedure,
                                      const gchar *help_domain,
                                      const gchar *help_locales,
                                      const gchar *help_id);


/*  public functions  */

void
gimp_help_show (Gimp        *gimp,
                const gchar *help_domain,
                const gchar *help_id)
{
  GimpGuiConfig *config;

  g_return_if_fail (GIMP_IS_GIMP (gimp));

  config = GIMP_GUI_CONFIG (gimp->config);

  if (config->use_help)
    {
      GimpIdleHelp *idle_help = g_new0 (GimpIdleHelp, 1);

      idle_help->gimp = gimp;

      if (help_domain && strlen (help_domain))
	idle_help->help_domain = g_strdup (help_domain);

      if (config->help_locales && strlen (config->help_locales))
        idle_help->help_locales = g_strdup (config->help_locales);
      else
        idle_help->help_locales = gimp_get_default_language ("LC_MESSAGES");

      if (help_id && strlen (help_id))
	idle_help->help_id = g_strdup (help_id);

      g_idle_add (gimp_idle_help, idle_help);

      if (gimp->be_verbose)
        g_print ("HELP: request for help-id '%s' from help-domain '%s'\n",
                 help_id     ? help_id      : "(null)",
                 help_domain ? help_domain  : "(null)");
    }
}


/*  private functions  */

static gboolean
gimp_idle_help (gpointer data)
{
  GimpIdleHelp *idle_help = data;
  const gchar  *procedure = NULL;

#ifdef GIMP_HELP_DEBUG
  g_printerr ("Help Domain: %s\n",
              idle_help->help_domain ? idle_help->help_domain : "NULL");
  g_printerr ("Help ID: %s\n\n",
              idle_help->help_id     ? idle_help->help_id     : "NULL");
#endif

  switch (GIMP_GUI_CONFIG (idle_help->gimp->config)->help_browser)
    {
    case GIMP_HELP_BROWSER_GIMP:
      if (gimp_help_internal (idle_help->gimp))
        {
          procedure = "extension_gimp_help_browser_temp";
          break;
        }

    case GIMP_HELP_BROWSER_WEB_BROWSER:
      /*  FIXME: should check for procedure availability  */
      procedure = "plug_in_web_browser";
      break;
    }

  gimp_help_call (idle_help->gimp,
                  procedure,
                  idle_help->help_domain,
                  idle_help->help_locales,
                  idle_help->help_id);

  g_free (idle_help->help_domain);
  g_free (idle_help->help_locales);
  g_free (idle_help->help_id);
  g_free (idle_help);

  return FALSE;
}

static void
gimp_help_internal_not_found_callback (GtkWidget *widget,
				       gboolean   use_web_browser,
				       gpointer   data)
{
  Gimp *gimp = GIMP (data);

  if (use_web_browser)
    g_object_set (gimp->config,
		  "help-browser", GIMP_HELP_BROWSER_WEB_BROWSER,
		  NULL);

  gtk_main_quit ();
}

static gboolean
gimp_help_internal (Gimp *gimp)
{
  ProcRecord *proc_rec;

  static gboolean busy = FALSE;

  if (busy)
    return TRUE;

  busy = TRUE;

  /*  Check if a help browser is already running  */
  proc_rec = procedural_db_lookup (gimp, "extension_gimp_help_browser_temp");

  if (proc_rec == NULL)
    {
      Argument *args = NULL;

      proc_rec = procedural_db_lookup (gimp, "extension_gimp_help_browser");

      if (proc_rec == NULL)
	{
	  GtkWidget *not_found =
	    gimp_query_boolean_box (_("Could not find GIMP Help Browser"),
				    NULL, NULL, NULL, FALSE,
				    _("Could not find the GIMP Help Browser "
                                      "procedure. It probably was not compiled "
                                      "because you don't have GtkHtml2 "
                                      "installed."),
				    _("Use web browser instead"),
				    GTK_STOCK_CANCEL,
				    NULL, NULL,
				    gimp_help_internal_not_found_callback,
				    gimp);
	  gtk_widget_show (not_found);
	  gtk_main ();

          busy = FALSE;

	  return (GIMP_GUI_CONFIG (gimp->config)->help_browser
		  != GIMP_HELP_BROWSER_WEB_BROWSER);
	}

      args = g_new (Argument, 1);

      args[0].arg_type      = GIMP_PDB_INT32;
      args[0].value.pdb_int = GIMP_RUN_INTERACTIVE;

      plug_in_run (gimp, gimp_get_user_context (gimp),
                   proc_rec, args, 1, FALSE, TRUE, -1);

      procedural_db_destroy_args (args, 1);
    }

  /*  Check if the help browser started properly  */
  proc_rec = procedural_db_lookup (gimp, "extension_gimp_help_browser_temp");

  if (proc_rec == NULL)
    {
      GtkWidget *not_found =
        gimp_query_boolean_box (_("Could not start GIMP Help Browser"),
                                NULL, NULL, NULL, FALSE,
                                _("Could not start the GIMP Help Browser."),
                                _("Use web browser instead"),
                                GTK_STOCK_CANCEL,
                                NULL, NULL,
                                gimp_help_internal_not_found_callback,
                                gimp);
      gtk_widget_show (not_found);
      gtk_main ();

      busy = FALSE;

      return (GIMP_GUI_CONFIG (gimp->config)->help_browser
              != GIMP_HELP_BROWSER_WEB_BROWSER);
    }

  busy = FALSE;

  return TRUE;
}

static void
gimp_help_call (Gimp        *gimp,
                const gchar *procedure,
                const gchar *help_domain,
                const gchar *help_locales,
                const gchar *help_id)
{
  ProcRecord *proc_rec;

  /*  Check if a help parser is already running  */
  proc_rec = procedural_db_lookup (gimp, "extension_gimp_help_temp");

  if (proc_rec == NULL)
    {
      Argument  *args         = NULL;
      gint       n_domains    = 0;
      gchar    **help_domains = NULL;
      gchar    **help_uris    = NULL;

      proc_rec = procedural_db_lookup (gimp, "extension_gimp_help");

      if (proc_rec == NULL)
        {
          /*  FIXME: error msg  */
          return;
	}

      n_domains = plug_ins_help_domains (gimp, &help_domains, &help_uris);

      args = g_new (Argument, 4);

      args[0].arg_type          = GIMP_PDB_INT32;
      args[0].value.pdb_int     = n_domains;
      args[1].arg_type          = GIMP_PDB_STRINGARRAY;
      args[1].value.pdb_pointer = help_domains;
      args[2].arg_type          = GIMP_PDB_INT32;
      args[2].value.pdb_int     = n_domains;
      args[3].arg_type          = GIMP_PDB_STRINGARRAY;
      args[3].value.pdb_pointer = help_uris;

      plug_in_run (gimp, gimp_get_user_context (gimp),
                   proc_rec, args, 4, FALSE, TRUE, -1);

      procedural_db_destroy_args (args, 4);
    }

  /*  Check if the help parser started properly  */
  proc_rec = procedural_db_lookup (gimp, "extension_gimp_help_temp");

  if (proc_rec == NULL)
    {
      /*  FIXME: error msg  */
      return;
    }
  else
    {
      Argument *return_vals;
      gint      n_return_vals;

#ifdef GIMP_HELP_DEBUG
      g_printerr ("Calling help via %s: %s %s %s\n",
                  procedure,
                  help_domain  ? help_domain  : "(null)",
                  help_locales ? help_locales : "(null)",
                  help_id      ? help_id      : "(null)");
#endif

      return_vals =
        procedural_db_run_proc (gimp,
                                gimp_get_user_context (gimp),
				"extension_gimp_help_temp",
                                &n_return_vals,
                                GIMP_PDB_STRING, procedure,
				GIMP_PDB_STRING, help_domain,
				GIMP_PDB_STRING, help_locales,
                                GIMP_PDB_STRING, help_id,
                                GIMP_PDB_END);

      procedural_db_destroy_args (return_vals, n_return_vals);
    }
}
