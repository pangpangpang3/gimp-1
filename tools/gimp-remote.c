/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimp-remote.c
 * Copyright (C) 2000-2004  Sven Neumann <sven@gimp.org>
 *                          Simon Budig <simon@gimp.org>
 *
 * Tells a running gimp to open files by creating a synthetic drop-event.
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

/* Disclaimer:
 *
 * It is a really bad idea to use Drag'n'Drop for inter-client
 * communication. Dont even think about doing this in your own newly
 * created application. We do this *only*, because we are in a
 * feature freeze for Gimp 1.2 and adding a completely new communication
 * infrastructure for remote controlling Gimp is definitely a new
 * feature...
 *                                                Simon
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <locale.h>

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include <X11/Xmu/WinUtil.h>

#include "libgimpbase/gimpversion.h"

#include <glib/gi18n.h>


#define GIMP_BINARY "gimp-" GIMP_APP_VERSION


static void  start_new_gimp (GdkScreen   *screen,
                             const gchar *argv0,
                             const gchar *startup_id,
                             GString     *file_list) G_GNUC_NORETURN;

static void  show_version   (void) G_GNUC_NORETURN;


static gboolean      existing  = FALSE;
static gboolean      query     = FALSE;
static gboolean      no_splash = FALSE;
static const gchar **filenames = NULL;

static const GOptionEntry main_entries[] =
{
  { "version", 'v', 0,
    G_OPTION_ARG_CALLBACK, (GOptionArgFunc) show_version,
    N_("Show version information and exit"), NULL
  },
  {
    "existing", 'e', 0,
    G_OPTION_ARG_NONE, &existing,
    N_("Use a running GIMP only, never start a new one"), NULL
  },
  {
    "query", 'q', 0,
    G_OPTION_ARG_NONE, &query,
    N_("Only check if GIMP is running, then quit"), NULL
  },
  {
    "no-splash", 's', 0,
    G_OPTION_ARG_NONE, &no_splash,
    N_("Start GIMP without showing the startup window"), NULL
  },
  {
    G_OPTION_REMAINING, 0, 0,
    G_OPTION_ARG_FILENAME_ARRAY, &filenames,
    NULL, NULL
  },
  { NULL }
};


static GdkWindow *
gimp_remote_find_window (GdkDisplay *display,
                         GdkScreen  *screen)
{
  GdkWindow  *result = NULL;
  Display    *xdisplay;
  Window      root, parent;
  Window     *children;
  Atom        role_atom;
  Atom        string_atom;
  guint       nchildren;
  gint        i;

  GdkWindow  *root_window = gdk_screen_get_root_window (screen);

  xdisplay = gdk_x11_display_get_xdisplay (display);

  if (XQueryTree (xdisplay, GDK_WINDOW_XID (root_window),
                  &root, &parent, &children, &nchildren) == 0)
    return NULL;

  if (! (children && nchildren))
    return NULL;

  role_atom   = XInternAtom (xdisplay, "WM_WINDOW_ROLE", TRUE);
  string_atom = XInternAtom (xdisplay, "STRING",         TRUE);

  for (i = nchildren - 1; i >= 0; i--)
    {
      Window  window;
      Atom    ret_type;
      gint    ret_format;
      gulong  bytes_after;
      gulong  nitems;
      guchar *data;

      /*  The XmuClientWindow() function finds a window at or below the
       *  specified window, that has a WM_STATE property. If such a
       *  window is found, it is returned; otherwise the argument window
       *  is returned.
       */

      window = XmuClientWindow (xdisplay, children[i]);

      /*  We are searching for the Gimp toolbox: Its WM_WINDOW_ROLE Property
       *  (as set by gtk_window_set_role ()) has the value "gimp-toolbox".
       *  This is pretty reliable, since ask for a special property,
       *  explicitly set by the gimp. See below... :-)
       */

      if (XGetWindowProperty (xdisplay, window,
                              role_atom,
                              0, 32,
                              FALSE,
                              string_atom,
                              &ret_type, &ret_format, &nitems, &bytes_after,
                              &data) == Success &&
          ret_type)
        {
          if (nitems > 11 && strcmp (data, "gimp-toolbox") == 0)
            {
              XFree (data);
              result = gdk_window_foreign_new_for_display (display, window);
              break;
            }

          XFree (data);
        }
    }

  XFree (children);

  return result;
}

static void
source_selection_get (GtkWidget        *widget,
		      GtkSelectionData *selection_data,
		      guint             info,
		      guint             time,
		      const gchar      *uri)
{
  gtk_selection_data_set (selection_data,
                          selection_data->target,
                          8, uri, strlen (uri));
  gtk_main_quit ();
}

static gboolean
toolbox_hidden (gpointer data)
{
  g_printerr ("%s\n%s\n",
              _("Could not connect to GIMP."),
              _("Make sure that the Toolbox is visible!"));
  gtk_main_quit ();

  return FALSE;
}

static void
start_new_gimp (GdkScreen   *screen,
                const gchar *argv0,
                const gchar *startup_id,
                GString     *file_list)
{
  gchar        *display_name;
  gchar       **argv;
  gchar        *gimp, *path, *name, *pwd;
  const gchar  *spath;
  gint          i;

  if (startup_id)
    putenv (g_strdup_printf ("DESKTOP_STARTUP_ID=%s", startup_id));

  if (file_list->len > 0)
    file_list = g_string_prepend (file_list, "\n");

  display_name = gdk_screen_make_display_name (screen);
  file_list = g_string_prepend (file_list, display_name);
  file_list = g_string_prepend (file_list, "--display\n");
  g_free (display_name);

  if (no_splash)
    file_list = g_string_prepend (file_list, "--no-splash\n");

  file_list = g_string_prepend (file_list, "gimp\n");

  argv = g_strsplit (file_list->str, "\n", 0);

  /* We are searching for the path the gimp-remote executable lives in */

  /*
   * the "_" environment variable usually gets set by the sh-family of
   * shells. We have to sanity-check it. If we do not find anything
   * usable in it try argv[0], then fall back to search the path.
   */

  gimp  = NULL;
  spath = NULL;

  for (i = 0; i < 2; i++)
    {
      if (i == 0)
        {
          spath = g_getenv ("_");
        }
      else if (i == 1)
        {
          spath = argv0;
        }

      if (spath)
        {
          name = g_path_get_basename (spath);

          if (!strncmp (name, "gimp-remote", 11))
            {
              path = g_path_get_dirname (spath);

              if (g_path_is_absolute (spath))
                {
                  gimp = g_build_filename (path, GIMP_BINARY, NULL);
                }
              else
                {
                  pwd = g_get_current_dir ();
                  gimp = g_build_filename (pwd, path, GIMP_BINARY, NULL);
                  g_free (pwd);
                }

              g_free (path);
            }

          g_free (name);
        }

      if (gimp)
        break;
    }

  /* We must ensure that gimp is started with a different PID.
     Otherwise it could happen that (when it opens it's display) it sends
     the same auth token again (because that one is uniquified with PID
     and time()), which the server would deny.  */
  switch (fork ())
    {
    case -1:
      exit (EXIT_FAILURE);

    case 0: /* child */
      execv (gimp, argv);
      execvp (GIMP_BINARY, argv);

      /*  if execv and execvp return, there was an error  */
      g_printerr (_("Couldn't start '%s': %s"),
                  GIMP_BINARY, g_strerror (errno));
      g_printerr ("\n");

      exit (EXIT_FAILURE);

    default: /* parent */
      break;
    }

  exit (EXIT_SUCCESS);
}

static void
show_version (void)
{
  g_print (_("%s version %s"), "gimp-remote", GIMP_VERSION);
  g_print ("\n");

  exit (EXIT_SUCCESS);
}

static void
init_i18n (void)
{
  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

  textdomain (GETTEXT_PACKAGE);
}

gint
main (gint    argc,
      gchar **argv)
{
  GOptionContext *context;
  GError         *error = NULL;
  GdkDisplay     *display;
  GdkScreen      *screen;
  GdkWindow      *gimp_window;
  const gchar    *startup_id;
  gchar          *desktop_startup_id = NULL;
  GString        *file_list          = g_string_new (NULL);

  init_i18n ();

  /* we save the startup_id before calling gtk_init()
     because GTK+ will unset it  */

  startup_id = g_getenv ("DESKTOP_STARTUP_ID");
  if (startup_id && *startup_id)
    desktop_startup_id = g_strdup (startup_id);

  /* parse the command-line options */
  context = g_option_context_new ("[FILE|URI...]");
  g_option_context_add_main_entries (context, main_entries, NULL);

#ifdef __GNUC__
#warning FIXME: add this code as soon as we depend on gtk+-2.6
#endif
  /* g_option_context_add_group (context, gtk_get_option_group (TRUE));
   */

  if (! g_option_context_parse (context, &argc, &argv, &error))
    {
      g_printerr ("%s\n", error->message);
      g_error_free (error);

      exit (EXIT_FAILURE);
    }

  gtk_init (&argc, &argv);

  if (filenames)
    {
      gchar *cwd = g_get_current_dir ();
      gint   i;

      for (i = 0; filenames[i]; i++)
        {
          const gchar *name = filenames[i];
          gchar       *uri;

          /* If not already a valid URI */
          if (g_ascii_strncasecmp ("file:",  name, 5) &&
              g_ascii_strncasecmp ("ftp:",   name, 4) &&
              g_ascii_strncasecmp ("http:",  name, 5) &&
              g_ascii_strncasecmp ("https:", name, 6))
            {
              if (g_path_is_absolute (name))
                {
                  uri = g_filename_to_uri (name, NULL, NULL);
                }
              else
                {
                  gchar *abs = g_build_filename (cwd, name, NULL);

                  uri = g_filename_to_uri (abs, NULL, NULL);
                  g_free (abs);
                }
            }
          else
            {
              uri = g_strdup (name);
            }

          if (uri)
            {
              if (file_list->len > 0)
                file_list = g_string_append_c (file_list, '\n');

              file_list = g_string_append (file_list, uri);
              g_free (uri);
            }
        }

      g_free (cwd);
    }

  display = gdk_display_get_default ();
  screen  = gdk_screen_get_default ();

  /*  if called without any filenames, always start a new GIMP  */
  if (file_list->len == 0 && !query && !existing)
    {
      start_new_gimp (screen, argv[0], desktop_startup_id, file_list);
    }

  gimp_window = gimp_remote_find_window (display, screen);

  if (! query)
    {
      if (gimp_window)
        {
          GdkDragContext  *context;
          GdkDragProtocol  protocol;
          GtkWidget       *source;
          GdkAtom          sel_type;
          GdkAtom          sel_id;
          GList           *targetlist;
          guint            timeout;

          gdk_drag_get_protocol_for_display (display,
                                             GDK_WINDOW_XID (gimp_window),
                                             &protocol);
          if (protocol != GDK_DRAG_PROTO_XDND)
            {
              g_printerr ("Gimp Window doesnt use Xdnd-Protocol - huh?\n");
              return EXIT_FAILURE;
            }

          /*  Problem: If the Toolbox is hidden via Tab (gtk_widget_hide)
           *  it does not accept DnD-Operations and gtk_main() will not be
           *  terminated. If the Toolbox is simply unmapped (by the WM)
           *  DnD works. But in both cases gdk_window_is_visible() returns
           *  FALSE. To work around this we add a timeout and abort after
           *  1.5 seconds.
           */

          timeout = g_timeout_add (1500, toolbox_hidden, NULL);

          /*  set up an DND-source  */
          source = gtk_window_new (GTK_WINDOW_TOPLEVEL);
          g_signal_connect (source, "selection_get",
                            G_CALLBACK (source_selection_get),
                            file_list->str);
          gtk_widget_realize (source);


          /*  specify the id and the content-type of the selection used to
           *  pass the URIs to Gimp.
           */
          sel_id   = gdk_atom_intern ("XdndSelection", FALSE);
          sel_type = gdk_atom_intern ("text/uri-list", FALSE);
          targetlist = g_list_prepend (NULL, GUINT_TO_POINTER (sel_type));

          /*  assign the selection to our DnD-source  */
          gtk_selection_owner_set (source, sel_id, GDK_CURRENT_TIME);
          gtk_selection_add_target (source, sel_id, sel_type, 0);

          /*  drag_begin/motion/drop  */
          context = gdk_drag_begin (source->window, targetlist);

          gdk_drag_motion (context, gimp_window, protocol, 0, 0,
                           GDK_ACTION_COPY, GDK_ACTION_COPY, GDK_CURRENT_TIME);

          gdk_drag_drop (context, GDK_CURRENT_TIME);

          /*  finally enter the mainloop to handle the events  */
          gtk_main ();

          g_source_remove (timeout);
        }
      else if (! existing)
        {
          start_new_gimp (screen, argv[0], desktop_startup_id, file_list);
        }
    }

  gdk_notify_startup_complete ();

  g_option_context_free (context);

  g_string_free (file_list, TRUE);
  g_free (desktop_startup_id);

  return (gimp_window ? EXIT_SUCCESS : EXIT_FAILURE);
}
