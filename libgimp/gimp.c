/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimp.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef WAIT_ANY
#define WAIT_ANY -1
#endif

#include <glib.h>

#ifndef G_OS_WIN32
#include "libgimpbase/gimpsignal.h"
#else
#include <signal.h>
#endif

#ifdef HAVE_IPC_H
#include <sys/ipc.h>
#endif

#ifdef HAVE_SHM_H
#include <sys/shm.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if defined(G_OS_WIN32) || defined(G_WITH_CYGWIN)
#  define STRICT
#  include <windows.h>
#  undef RGB
#endif

#ifdef __EMX__
#  include <fcntl.h>
#endif

#include <libintl.h>
#include <locale.h>

#ifndef LIBGIMP_COMPILATION
#define LIBGIMP_COMPILATION
#endif

#include "libgimpbase/gimpbasetypes.h"

#include "libgimpbase/gimpenv.h"
#include "libgimpbase/gimpprotocol.h"
#include "libgimpbase/gimpwire.h"

#include "gimp.h"


/* Maybe this should go in a public header if we add other things to it */
typedef enum
{
  GIMP_DEBUG_PID                = 1 << 0,
  GIMP_DEBUG_FATAL_WARNINGS     = 1 << 1,
  GIMP_DEBUG_QUERY              = 1 << 2,
  GIMP_DEBUG_INIT               = 1 << 3,
  GIMP_DEBUG_RUN                = 1 << 4,
  GIMP_DEBUG_QUIT               = 1 << 5,

  GIMP_DEBUG_DEFAULT            = (GIMP_DEBUG_RUN | GIMP_DEBUG_FATAL_WARNINGS)
} GimpDebugFlag;

#define WRITE_BUFFER_SIZE  1024

void gimp_read_expect_msg   (WireMessage *msg,
			     gint         type);


static void       gimp_close                   (void);
static void       gimp_debug_stop              (void);
static void       gimp_message_func            (const gchar    *log_domain,
						GLogLevelFlags  log_level,
						const gchar    *message,
						gpointer        data);
#ifndef G_OS_WIN32
static void       gimp_plugin_sigfatal_handler (gint            sig_num);
#endif
static gboolean   gimp_plugin_io_error_handler (GIOChannel     *channel,
						GIOCondition    cond,
						gpointer        data);
static gboolean   gimp_write                   (GIOChannel     *channel,
						guint8         *buf,
						gulong          count,
                                                gpointer        user_data);
static gboolean   gimp_flush                   (GIOChannel     *channel,
                                                gpointer        user_data);
static void       gimp_loop                    (void);
static void       gimp_config                  (GPConfig       *config);
static void       gimp_proc_run                (GPProcRun      *proc_run);
static void       gimp_temp_proc_run           (GPProcRun      *proc_run);
static void       gimp_process_message         (WireMessage    *msg);
static void       gimp_single_message          (void);
static gboolean   gimp_extension_read          (GIOChannel     *channel,
                                                GIOCondition    condition,
                                                gpointer        data);


static GIOChannel *_readchannel  = NULL;
GIOChannel *_writechannel = NULL;

const guint gimp_major_version = GIMP_MAJOR_VERSION;
const guint gimp_minor_version = GIMP_MINOR_VERSION;
const guint gimp_micro_version = GIMP_MICRO_VERSION;

#if defined(G_OS_WIN32) || defined(G_WITH_CYGWIN)
static HANDLE shm_handle;
#endif

static gint         _tile_width        = -1;
static gint         _tile_height       = -1;
static gint         _shm_ID            = -1;
static guchar      *_shm_addr          = NULL;
static gdouble      _gamma_val         = 1.0;
static gboolean     _install_cmap      = FALSE;
static gboolean     _show_tool_tips    = TRUE;
static gint         _min_colors        = 144;
static gint         _gdisp_ID          = -1;
static gchar       *_wm_name           = NULL;
static gchar       *_wm_class          = NULL;
static gchar       *_display_name      = NULL;
static gint         _monitor_number    = 0;
static const gchar *progname           = NULL;

static guint8       write_buffer[WRITE_BUFFER_SIZE];
static guint        write_buffer_index = 0;

static GimpStackTraceMode stack_trace_mode = GIMP_STACK_TRACE_NEVER;

static GHashTable *temp_proc_ht = NULL;

static guint gimp_debug_flags = 0;

static const GDebugKey gimp_debug_keys[] =
{
  { "pid",            GIMP_DEBUG_PID            },
  { "fatal-warnings", GIMP_DEBUG_FATAL_WARNINGS },
  { "fw",             GIMP_DEBUG_FATAL_WARNINGS },
  { "query",          GIMP_DEBUG_QUERY          },
  { "init",           GIMP_DEBUG_INIT           },
  { "run",            GIMP_DEBUG_RUN            },
  { "quit",           GIMP_DEBUG_QUIT           },
  { "on",             GIMP_DEBUG_DEFAULT        }
};


static GimpPlugInInfo PLUG_IN_INFO;


/**
 * gimp_main:
 * @info: the PLUG_IN_INFO structure
 * @argc: the number of arguments
 * @argv: the arguments
 *
 * The main procedure that must be called with the PLUG_IN_INFO structure
 * and the 'argc' and 'argv' that are passed to "main".
 *
 * Return value:
 **/
gint
gimp_main (const GimpPlugInInfo *info,
           gint                  argc,
	   gchar                *argv[])
{
  gchar       *basename;
  const gchar *env_string;
  gchar       *debug_string;

#ifdef G_OS_WIN32
  gint i, j, k;

  /* Check for exe file name with spaces in the path having been split up
   * by buggy NT C runtime, or something. I don't know why this happens
   * on NT (including w2k), but not on w95/98.
   */

  for (i = 1; i < argc; i++)
    {
      k = strlen (argv[i]);

      if (k > 10)
	{
	  if (g_ascii_strcasecmp (argv[i] + k - 4, ".exe") == 0)
	    {
	      /* Found the end of the executable name, most probably.
	       * Splice the parts of the name back together.
	       */
	      GString *s;

	      s = g_string_new (argv[0]);

	      for (j = 1; j <= i; j++)
		{
		  s = g_string_append_c (s, ' ');
		  s = g_string_append (s, argv[j]);
		}

	      argv[0] = s->str;

	      /* Move rest of argv down */
	      for (j = 1; j < argc - i; j++)
		argv[j] = argv[j + i];

	      argv[argc - i] = NULL;
	      argc -= i;

	      break;
	  }
       }
    }
#endif

  g_assert (info != NULL);

  PLUG_IN_INFO = *info;

  if ((argc != 6) || (strcmp (argv[1], "-gimp") != 0))
    {
      g_printerr ("%s is a gimp plug-in and must be run by the gimp to be used\n", argv[0]);
      return 1;
    }

  progname = argv[0];

  basename = g_path_get_basename (progname);

  g_set_prgname (basename);

  env_string = g_getenv ("GIMP_PLUGIN_DEBUG");

  if (env_string)
    {
      debug_string = strchr (env_string, ',');

      if (debug_string)
        {
          gint len = debug_string - env_string;

	  if ((strlen (basename) == len) &&
              (strncmp (basename, env_string, len) == 0))
            {
              gimp_debug_flags =
                g_parse_debug_string (debug_string + 1,
                                      gimp_debug_keys,
                                      G_N_ELEMENTS (gimp_debug_keys));
            }
        }
      else if (strcmp (env_string, basename) == 0)
        {
          gimp_debug_flags = GIMP_DEBUG_DEFAULT;
        }
    }

  g_free (basename);

  stack_trace_mode = (GimpStackTraceMode) CLAMP (atoi (argv[5]),
						 GIMP_STACK_TRACE_NEVER,
						 GIMP_STACK_TRACE_ALWAYS);

#ifndef G_OS_WIN32
  /* No use catching these on Win32, the user won't get any meaningful
   * stack trace from glib anyhow. It's better to let Windows inform
   * about the program error, and offer debugging if the plug-in
   * has been built with MSVC, and the user has MSVC installed.
   */
  gimp_signal_private (SIGHUP,  gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGINT,  gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGQUIT, gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGBUS,  gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGSEGV, gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGTERM, gimp_plugin_sigfatal_handler, 0);
  gimp_signal_private (SIGFPE,  gimp_plugin_sigfatal_handler, 0);

  /* Ignore SIGPIPE from crashing Gimp */
  gimp_signal_private (SIGPIPE, SIG_IGN, 0);

  /* Restart syscalls interrupted by SIGCHLD */
  gimp_signal_private (SIGCHLD, SIG_DFL, SA_RESTART);
#endif

  _readchannel  = g_io_channel_unix_new (atoi (argv[2]));
  _writechannel = g_io_channel_unix_new (atoi (argv[3]));
#ifdef __EMX__
  setmode (g_io_channel_unix_get_fd (_readchannel), O_BINARY);
  setmode (g_io_channel_unix_get_fd (_writechannel), O_BINARY);
#endif

  g_io_channel_set_encoding (_readchannel, NULL, NULL);
  g_io_channel_set_encoding (_writechannel, NULL, NULL);

  g_io_channel_set_buffered (_readchannel, FALSE);
  g_io_channel_set_buffered (_writechannel, FALSE);

  g_io_channel_set_close_on_unref (_readchannel, TRUE);
  g_io_channel_set_close_on_unref (_writechannel, TRUE);

  gp_init ();
  wire_set_writer (gimp_write);
  wire_set_flusher (gimp_flush);


  /* initialize i18n support */

  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE"-libgimp", gimp_locale_directory ());
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE"-libgimp", "UTF-8");
#endif


  /* set handler both for the "LibGimp" and "" domains */

  g_log_set_handler (G_LOG_DOMAIN,
		     G_LOG_LEVEL_MESSAGE,
		     gimp_message_func,
		     NULL);
  g_log_set_handler (NULL,
		     G_LOG_LEVEL_MESSAGE,
		     gimp_message_func,
		     NULL);

  if (gimp_debug_flags & GIMP_DEBUG_FATAL_WARNINGS)
    {
      GLogLevelFlags fatal_mask;

      fatal_mask = g_log_set_always_fatal (G_LOG_FATAL_MASK);
      fatal_mask |= G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL;
      g_log_set_always_fatal (fatal_mask);
    }

  if (strcmp (argv[4], "-query") == 0)
    {
      if (PLUG_IN_INFO.init_proc)
        gp_has_init_write (_writechannel, NULL);

      if (gimp_debug_flags & GIMP_DEBUG_QUERY)
        gimp_debug_stop ();

      if (PLUG_IN_INFO.query_proc)
	(* PLUG_IN_INFO.query_proc) ();

      gimp_close ();
      return 0;
    }

  if (strcmp (argv[4], "-init") == 0)
    {
      if (gimp_debug_flags & GIMP_DEBUG_INIT)
        gimp_debug_stop ();

      if (PLUG_IN_INFO.init_proc)
	(* PLUG_IN_INFO.init_proc) ();

      gimp_close ();
      return 0;
    }

  if (gimp_debug_flags & GIMP_DEBUG_RUN)
    gimp_debug_stop ();
  else if (gimp_debug_flags & GIMP_DEBUG_PID)
    g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Here I am!");

  temp_proc_ht = g_hash_table_new (g_str_hash, g_str_equal);

  g_io_add_watch (_readchannel,
		  G_IO_ERR | G_IO_HUP,
		  gimp_plugin_io_error_handler,
		  NULL);

  gimp_loop ();
  return 0;
}

/**
 * gimp_quit:
 *
 * Forcefully causes the gimp library to exit and close down its
 * connection to main gimp application. This function never returns.
 **/
void
gimp_quit (void)
{
  gimp_close ();
  exit (0);
}

/**
 * gimp_install_procedure:
 * @name:          the procedure's name.
 * @blurb:         a short text describing what the procedure does.
 * @help:          the help text for the procedure (usually considerably
 *                 longer than @blurb).
 * @author:        the procedure's author(s).
 * @copyright:     the procedure's copyright.
 * @date:          the date the procedure was added.
 * @menu_path:     the procedure's menu path, or #NULL if the procedure has
 *                 no menu entry.
 * @image_types:   the drawable types the procedure can handle.
 * @type:          the type of the procedure.
 * @n_params:      the number of parameters the procedure takes.
 * @n_return_vals: the number of return values the procedure returns.
 * @params:        the procedure's parameters.
 * @return_vals:   the procedure's return values.
 *
 * Installs a new procedure with the PDB (procedural database).
 *
 * Call this function from within your Plug-In's query() function for
 * each procedure your Plug-In implements.
 *
 * The @name parameter is mandatory and should be unique, or it will
 * overwrite an already existing procedure (overwrite procedures only
 * if you know what you're doing).
 *
 * The @blurb, @help, @author, @copyright and @date parameters are
 * optional but then you shouldn't write procedures without proper
 * documentation, should you.
 *
 * @menu_path can be #NULL, or a string in the form
 * "&lt;Domain&gt;/Path/To/My/Menu"
 * (e.g. "&lt;Image&gt;/Filters/Render/Useless") if you want your
 * procedure to be available via a menu.  Allowed prefixes are
 * &lt;Toolbox&gt;, &lt;Image&gt;, &lt;Load&gt; and &lt;Save&gt;
 *
 * @type must be one of #GIMP_PLUGIN or #GIMP_EXTENSION. Note that
 * temporary procedures must be installed using
 * gimp_install_temp_proc().
 *
 * NOTE: Unlike the GIMP 1.2 API, #GIMP_EXTENSION no longer means
 * that the procedure's menu prefix is &lt;Toolbox&gt;, but that
 * it will install temporary procedures. Therefore, the GIMP core
 * will wait until the #GIMP_EXTENSION procedure has called
 * gimp_extension_ack(), which means that the procedure has done
 * its initialization, installed its temporary procedures and is
 * ready to run.
 *
 * <emphasis>Not calling gimp_extension_ack() from a #GIMP_EXTENSION
 * procedure will cause th GIMP core to lock up.</emphasis>
 *
 * Additionally, a #GIMP_EXTENSION procedure with no parameters
 * (@n_params == 0 and @params == #NULL) is an "automatic" extension
 * that will be automatically started on each GIMP startup.
 **/
void
gimp_install_procedure (const gchar        *name,
			const gchar        *blurb,
			const gchar        *help,
			const gchar        *author,
			const gchar        *copyright,
			const gchar        *date,
			const gchar        *menu_path,
			const gchar        *image_types,
			GimpPDBProcType     type,
			gint                n_params,
			gint                n_return_vals,
			const GimpParamDef *params,
			const GimpParamDef *return_vals)
{
  GPProcInstall proc_install;

  g_return_if_fail (name != NULL);
  g_return_if_fail (type != GIMP_INTERNAL);
  g_return_if_fail ((n_params == 0 && params == NULL) ||
                    (n_params > 0  && params != NULL));
  g_return_if_fail ((n_return_vals == 0 && return_vals == NULL) ||
                    (n_return_vals > 0  && return_vals != NULL));

  proc_install.name         = (gchar *) name;
  proc_install.blurb        = (gchar *) blurb;
  proc_install.help         = (gchar *) help;
  proc_install.author       = (gchar *) author;
  proc_install.copyright    = (gchar *) copyright;
  proc_install.date         = (gchar *) date;
  proc_install.menu_path    = (gchar *) menu_path;
  proc_install.image_types  = (gchar *) image_types;
  proc_install.type         = type;
  proc_install.nparams      = n_params;
  proc_install.nreturn_vals = n_return_vals;
  proc_install.params       = (GPParamDef *) params;
  proc_install.return_vals  = (GPParamDef *) return_vals;

  if (! gp_proc_install_write (_writechannel, &proc_install, NULL))
    gimp_quit ();
}

/**
 * gimp_install_temp_proc:
 * @name:          the procedure's name.
 * @blurb:         a short text describing what the procedure does.
 * @help:          the help text for the procedure (usually considerably
 *                 longer than @blurb).
 * @author:        the procedure's author(s).
 * @copyright:     the procedure's copyright.
 * @date:          the date the procedure was added.
 * @menu_path:     the procedure's menu path, or #NULL if the procedure has
 *                 no menu entry.
 * @image_types:   the drawable types the procedure can handle.
 * @type:          the type of the procedure.
 * @n_params:      the number of parameters the procedure takes.
 * @n_return_vals: the number of return values the procedure returns.
 * @params:        the procedure's parameters.
 * @return_vals:   the procedure's return values.
 * @run_proc:      the function to call for executing the procedure.
 *
 * Installs a new temporary procedure with the PDB (procedural database).
 *
 * A temporary procedure is a procedure which is only available while
 * one of your Plug-In's "real" procedures is running.
 *
 * See gimp_install_procedure() for most details.
 *
 * @type <emphasis>must</emphasis> be #GIMP_TEMPORARY or the function
 * will fail.
 *
 * @run_proc is the function which will be called to execute the
 * procedure.
 *
 * NOTE: Normally, Plug-In communication is triggered by the Plug-In
 * and the GIMP core only responds to the Plug-In's requests. You must
 * explicitely enable receiving of temporary procedure run requests
 * using either gimp_extension_enable() or
 * gimp_extension_process(). See this functions' documentation for
 * details.
 **/
void
gimp_install_temp_proc (const gchar        *name,
			const gchar        *blurb,
			const gchar        *help,
			const gchar        *author,
			const gchar        *copyright,
			const gchar        *date,
			const gchar        *menu_path,
			const gchar        *image_types,
			GimpPDBProcType     type,
			gint                n_params,
			gint                n_return_vals,
			const GimpParamDef *params,
			const GimpParamDef *return_vals,
			GimpRunProc         run_proc)
{
  g_return_if_fail (name != NULL);
  g_return_if_fail ((n_params == 0 && params == NULL) ||
                    (n_params > 0  && params != NULL));
  g_return_if_fail ((n_return_vals == 0 && return_vals == NULL) ||
                    (n_return_vals > 0  && return_vals != NULL));
  g_return_if_fail (type == GIMP_TEMPORARY);
  g_return_if_fail (run_proc != NULL);

  gimp_install_procedure (name,
			  blurb, help,
			  author, copyright, date,
			  menu_path,
			  image_types,
			  type,
			  n_params, n_return_vals,
			  params, return_vals);

  /*  Insert the temp proc run function into the hash table  */
  g_hash_table_insert (temp_proc_ht, g_strdup (name), (gpointer) run_proc);
}

/**
 * gimp_uninstall_temp_proc:
 * @name: the procedure's name
 *
 * Uninstalls a temporary procedure which has previously been
 * installed using gimp_install_temp_proc().
 **/
void
gimp_uninstall_temp_proc (const gchar *name)
{
  GPProcUninstall proc_uninstall;
  gpointer        hash_name;
  gboolean        found;

  g_return_if_fail (name != NULL);

  proc_uninstall.name = (gchar *) name;

  if (! gp_proc_uninstall_write (_writechannel, &proc_uninstall, NULL))
    gimp_quit ();

  found = g_hash_table_lookup_extended (temp_proc_ht, name, &hash_name, NULL);
  if (found)
    {
      g_hash_table_remove (temp_proc_ht, (gpointer) name);
      g_free (hash_name);
    }
}

/**
 * gimp_run_procedure:
 * @name:          the name of the procedure to run
 * @n_return_vals: return location for the number of return values
 * @Varargs:       list of procedure parameters
 *
 * This function calls a GIMP procedure and returns its return values.
 *
 * The procedure's parameters are given by a va_list in the format
 * (type, value, type, value) and must be terminated by #GIMP_PDB_END.
 *
 * This function converts the va_list of parameters into an array
 * and passes them to gimp_run_procedure2().
 *
 * Return value: the procedure's return values.
 **/
GimpParam *
gimp_run_procedure (const gchar *name,
		    gint        *n_return_vals,
		    ...)
{
  GimpPDBArgType  param_type;
  GimpParam      *return_vals;
  GimpParam      *params   = NULL;
  gint            n_params = 0;
  va_list         args;
  gint            i;

  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (n_return_vals != NULL, NULL);

  va_start (args, n_return_vals);
  param_type = va_arg (args, GimpPDBArgType);

  while (param_type != GIMP_PDB_END)
    {
      switch (param_type)
	{
	case GIMP_PDB_INT32:
        case GIMP_PDB_DISPLAY:
        case GIMP_PDB_IMAGE:
        case GIMP_PDB_LAYER:
        case GIMP_PDB_CHANNEL:
        case GIMP_PDB_DRAWABLE:
        case GIMP_PDB_SELECTION:
        case GIMP_PDB_BOUNDARY:
        case GIMP_PDB_PATH:
        case GIMP_PDB_STATUS:
	  (void) va_arg (args, gint);
	  break;
	case GIMP_PDB_INT16:
	  (void) va_arg (args, gint);
	  break;
	case GIMP_PDB_INT8:
	  (void) va_arg (args, gint);
	  break;
        case GIMP_PDB_FLOAT:
          (void) va_arg (args, gdouble);
          break;
        case GIMP_PDB_STRING:
          (void) va_arg (args, gchar *);
          break;
        case GIMP_PDB_INT32ARRAY:
          (void) va_arg (args, gint32 *);
          break;
        case GIMP_PDB_INT16ARRAY:
          (void) va_arg (args, gint16 *);
          break;
        case GIMP_PDB_INT8ARRAY:
          (void) va_arg (args, gint8 *);
          break;
        case GIMP_PDB_FLOATARRAY:
          (void) va_arg (args, gdouble *);
          break;
        case GIMP_PDB_STRINGARRAY:
          (void) va_arg (args, gchar **);
          break;
        case GIMP_PDB_COLOR:
	  (void) va_arg (args, GimpRGB *);
          break;
        case GIMP_PDB_PARASITE:
          (void) va_arg (args, GimpParasite *);
          break;
        case GIMP_PDB_REGION:
          break;
	case GIMP_PDB_END:
	  break;
	}

      n_params++;

      param_type = va_arg (args, GimpPDBArgType);
    }

  va_end (args);

  params = g_new0 (GimpParam, n_params);

  va_start (args, n_return_vals);

  for (i = 0; i < n_params; i++)
    {
      params[i].type = va_arg (args, GimpPDBArgType);

      switch (params[i].type)
	{
	case GIMP_PDB_INT32:
	  params[i].data.d_int32 = (gint32) va_arg (args, gint);
	  break;
	case GIMP_PDB_INT16:
	  params[i].data.d_int16 = (gint16) va_arg (args, gint);
	  break;
	case GIMP_PDB_INT8:
	  params[i].data.d_int8 = (gint8) va_arg (args, gint);
	  break;
        case GIMP_PDB_FLOAT:
          params[i].data.d_float = (gdouble) va_arg (args, gdouble);
          break;
        case GIMP_PDB_STRING:
          params[i].data.d_string = va_arg (args, gchar *);
          break;
        case GIMP_PDB_INT32ARRAY:
          params[i].data.d_int32array = va_arg (args, gint32 *);
          break;
        case GIMP_PDB_INT16ARRAY:
          params[i].data.d_int16array = va_arg (args, gint16 *);
          break;
        case GIMP_PDB_INT8ARRAY:
          params[i].data.d_int8array = va_arg (args, gint8 *);
          break;
        case GIMP_PDB_FLOATARRAY:
          params[i].data.d_floatarray = va_arg (args, gdouble *);
          break;
        case GIMP_PDB_STRINGARRAY:
          params[i].data.d_stringarray = va_arg (args, gchar **);
          break;
        case GIMP_PDB_COLOR:
	  params[i].data.d_color = *va_arg (args, GimpRGB *);
          break;
        case GIMP_PDB_REGION:
          break;
        case GIMP_PDB_DISPLAY:
	  params[i].data.d_display = va_arg (args, gint32);
          break;
        case GIMP_PDB_IMAGE:
	  params[i].data.d_image = va_arg (args, gint32);
          break;
        case GIMP_PDB_LAYER:
	  params[i].data.d_layer = va_arg (args, gint32);
          break;
        case GIMP_PDB_CHANNEL:
	  params[i].data.d_channel = va_arg (args, gint32);
          break;
        case GIMP_PDB_DRAWABLE:
	  params[i].data.d_drawable = va_arg (args, gint32);
          break;
        case GIMP_PDB_SELECTION:
	  params[i].data.d_selection = va_arg (args, gint32);
          break;
        case GIMP_PDB_BOUNDARY:
	  params[i].data.d_boundary = va_arg (args, gint32);
          break;
        case GIMP_PDB_PATH:
	  params[i].data.d_path = va_arg (args, gint32);
          break;
        case GIMP_PDB_PARASITE:
          {
            GimpParasite *parasite = va_arg (args, GimpParasite *);

            if (parasite == NULL)
              {
                params[i].data.d_parasite.name = NULL;
                params[i].data.d_parasite.data = NULL;
              }
            else
              {
                params[i].data.d_parasite.name  = parasite->name;
                params[i].data.d_parasite.flags = parasite->flags;
                params[i].data.d_parasite.size  = parasite->size;
                params[i].data.d_parasite.data  = parasite->data;
              }
          }
          break;
        case GIMP_PDB_STATUS:
          params[i].data.d_status = va_arg (args, gint32);
          break;
	case GIMP_PDB_END:
	  break;
	}
    }

  va_end (args);

  return_vals = gimp_run_procedure2 (name, n_return_vals,
                                     n_params, params);

  g_free (params);

  return return_vals;
}

void
gimp_read_expect_msg (WireMessage *msg,
		      gint         type)
{
  while (TRUE)
    {
      if (! wire_read_msg (_readchannel, msg, NULL))
	gimp_quit ();

      if (msg->type != type)
	{
	  if (msg->type == GP_TEMP_PROC_RUN || msg->type == GP_QUIT)
	    {
	      gimp_process_message (msg);
	      continue;
	    }
	  else
	    g_error ("unexpected message: %d", msg->type);
	}
      else
	break;
    }
}


/**
 * gimp_run_procedure2:
 * @name:          the name of the procedure to run
 * @n_return_vals: return location for the number of return values
 * @n_params:      the number of parameters the procedure takes.
 * @params:        the procedure's parameters array.
 *
 * This function calls a GIMP procedure and returns its return values.
 *
 * Return value: the procedure's return values.
 **/
GimpParam *
gimp_run_procedure2 (const gchar     *name,
		     gint            *n_return_vals,
		     gint             n_params,
		     const GimpParam *params)
{
  GPProcRun     proc_run;
  GPProcReturn *proc_return;
  WireMessage   msg;
  GimpParam    *return_vals;

  g_return_val_if_fail (name != NULL, NULL);
  g_return_val_if_fail (n_return_vals != NULL, NULL);

  proc_run.name    = (gchar *) name;
  proc_run.nparams = n_params;
  proc_run.params  = (GPParam *) params;

  if (! gp_proc_run_write (_writechannel, &proc_run, NULL))
    gimp_quit ();

  gimp_read_expect_msg (&msg, GP_PROC_RETURN);

  proc_return = msg.data;
  *n_return_vals = proc_return->nparams;
  return_vals = (GimpParam *) proc_return->params;

  switch (return_vals[0].data.d_status)
    {
    case GIMP_PDB_EXECUTION_ERROR:
      /*g_warning ("an execution error occured while trying to run: \"%s\"", name);*/
      break;
    case GIMP_PDB_CALLING_ERROR:
      g_warning ("a calling error occured while trying to run: \"%s\"", name);
      break;
    default:
      break;
    }

  g_free (proc_return->name);
  g_free (proc_return);

  return return_vals;
}

/**
 * gimp_destroy_params:
 * @params:   the #GimpParam array to destroy
 * @n_params: the number of elements in the array
 *
 * Destroys a #GimpParam array as returned by gimp_run_procedure()
 **/
void
gimp_destroy_params (GimpParam *params,
		     gint       n_params)
{
  extern void _gp_params_destroy (GPParam *params, gint n_params);

  _gp_params_destroy ((GPParam*) params, n_params);
}

/**
 * gimp_destroy_paramdefs:
 * @paramdefs: the #GimpParamDef array to destroy
 * @n_params:  the number of elements in the array
 *
 * Destroys a #GimpParamDef array as returned by gimp_query_procedure()
 **/
void
gimp_destroy_paramdefs (GimpParamDef *paramdefs,
			gint          n_params)
{
  while (n_params--)
    {
      g_free (paramdefs[n_params].name);
      g_free (paramdefs[n_params].description);
    }

  g_free (paramdefs);
}

/**
 * gimp_tile_width:
 *
 * Returns the tile_width the GIMP is using. This is a constant value
 * given at Plug-In config time.
 *
 * Return value: the tile_width
 **/
guint
gimp_tile_width (void)
{
  return _tile_width;
}

/**
 * gimp_tile_height:
 *
 * Returns the tile_height the GIMP is using. This is a constant value
 * given at Plug-In config time.
 *
 * Return value: the tile_height
 **/
guint
gimp_tile_height (void)
{
  return _tile_height;
}

/**
 * gimp_shm_ID:
 *
 * Returns the shared memory ID used for passing tile data between the GIMP
 * core and the Plug-In. This is a constant value
 * given at Plug-In config time.
 *
 * Return value: the shared memory ID
 **/
gint
gimp_shm_ID (void)
{
  return _shm_ID;
}

/**
 * gimp_shm_addr:
 *
 * Returns the address of the shared memory segment used for passing
 * tile data between the GIMP core and the Plug-In. This is a constant
 * value given at Plug-In config time.
 *
 * Return value: the shared memory address
 **/
guchar *
gimp_shm_addr (void)
{
  return _shm_addr;
}

/**
 * gimp_gamma:
 *
 * Returns the global gamma value the GIMP and all its Plug-Ins should
 * use. This is a constant value given at Plug-In config time.
 *
 * NOTE: this feature is unimplemented.
 *
 * Return value: the gamma value
 **/
gdouble
gimp_gamma (void)
{
  return _gamma_val;
}

/**
 * gimp_install_cmap:
 *
 * Returns whether or not the Plug-In should allocate an own colormap
 * when running on an 8 bit display. This is a constant value given at
 * Plug-In config time.
 *
 * See also: gimp_min_colors()
 *
 * Return value: the install_cmap boolean
 **/
gboolean
gimp_install_cmap (void)
{
  return _install_cmap;
}

/**
 * gimp_min_colors:
 *
 * Returns the minimum number of colors to use when allocating an own
 * colormap on 8 bit displays. This is a constant value given at
 * Plug-In config time.
 *
 * See also: gimp_install_cmap()
 *
 * Return value: the minimum number of colors to allocate
 **/
gint
gimp_min_colors (void)
{
  return _min_colors;
}

/**
 * gimp_show_tool_tips:
 *
 * Returns whether or not the Plug-In should show tooltips. This is a
 * constant value given at Plug-In config time.
 *
 * Return value: the show_tool_tips boolean
 **/
gboolean
gimp_show_tool_tips (void)
{
  return _show_tool_tips;
}

/**
 * gimp_default_display:
 *
 * Returns the default display ID. This corresponds to the display the
 * running procedure's menu entry was invoked from. This is a
 * constant value given at Plug-In config time.
 *
 * Return value: the default display ID
 **/
gint32
gimp_default_display (void)
{
  return _gdisp_ID;
}

/**
 * gimp_wm_name:
 *
 * Returns the window manager name to be used for plug-in windows.
 * This is a constant value given at Plug-In config time.
 *
 * Return value: the window manager name
 **/
const gchar *
gimp_wm_name (void)
{
  return (const gchar *) _wm_name;
}

/**
 * gimp_wm_class:
 *
 * Returns the window manager class to be used for plug-in windows.
 * This is a constant value given at Plug-In config time.
 *
 * Return value: the window manager class
 **/
const gchar *
gimp_wm_class (void)
{
  return (const gchar *) _wm_class;
}

/**
 * gimp_display_name:
 *
 * Returns the display to be used for plug-in windows.
 * This is a constant value given at Plug-In config time.
 *
 * Return value: the display name
 **/
const gchar *
gimp_display_name (void)
{
  return (const gchar *) _display_name;
}

/**
 * gimp_monitor_number:
 *
 * Returns the monitor number to be used for plug-in windows.
 * This is a constant value given at Plug-In config time.
 *
 * Return value: the monitor number
 **/
gint
gimp_monitor_number (void)
{
  return _monitor_number;
}

/**
 * gimp_get_progname:
 *
 * Returns the Plug-In's executable name.
 *
 * Return value: the executable name
 **/
const gchar *
gimp_get_progname (void)
{
  return progname;
}

/**
 * gimp_extension_ack:
 *
 * Notify the main GIMP application that the extension has been properly
 * initialized and is ready to run.
 *
 * This function <emphasis>must</emphasis> be called from every
 * procedure that was registered as #GIMP_EXTENSION.
 *
 * Subsequently, extensions can process temporary procedure run
 * requests using either gimp_extension_enable() or
 * gimp_extension_process().
 *
 * See also: gimp_install_procedure(), gimp_install_temp_proc()
 **/
void
gimp_extension_ack (void)
{
  if (! gp_extension_ack_write (_writechannel, NULL))
    gimp_quit ();
}

/**
 * gimp_extension_enable:
 *
 * Enables asnychronous processing of messages from the main GIMP
 * application.
 *
 * Normally, a plug-in is not called by the GIMP except for the call
 * to the procedure it implements. All subsequent communication is
 * triggered by the plug-in and all messages sent from the GIMP to the
 * plug-in are just answers to requests the plug-in made.
 *
 * If the plug-in however registered temporary procedures using
 * gimp_install_temp_proc(), it needs to be able to receive requests
 * to execute them. Usually this will be done by running
 * gimp_extension_process() in an endless loop.
 *
 * If the plug-in cannot use gimp_extension_process(), i.e. if it has
 * a GUI and is hanging around in a #GMainLoop, it must call
 * gimp_extension_enable().
 *
 * Note that the plug-in does not need to be a #GIMP_EXTENSION to
 * register temporary procedures.
 *
 * See also: gimp_install_procedure(), gimp_install_temp_proc()
 **/
void
gimp_extension_enable (void)
{
  static gboolean callback_added = FALSE;

  if (! callback_added)
    {
      g_io_add_watch (_readchannel, G_IO_IN | G_IO_PRI, gimp_extension_read,
                      NULL);

      callback_added = TRUE;
    }
}

/**
 * gimp_extension_process:
 * @timeout: The timeout (in ms) to use for the select() call.
 *
 * Processes one message sent by the GIMP and returns.
 *
 * Call this function in an endless loop after calling
 * gimp_extension_ack() to process requests for running temporary
 * procedures.
 *
 * See gimp_extension_enable() for an asynchronous way of doing the
 * same if running an endless loop is not an option.
 *
 * See also: gimp_install_procedure(), gimp_install_temp_proc()
 **/
void
gimp_extension_process (guint timeout)
{
#ifndef G_OS_WIN32
  fd_set readfds;
  gint   select_val;
  struct timeval  tv;
  struct timeval *tvp;

  if (timeout)
    {
      tv.tv_sec  = timeout / 1000;
      tv.tv_usec = (timeout % 1000) * 1000;
      tvp = &tv;
    }
  else
    tvp = NULL;

  FD_ZERO (&readfds);
  FD_SET (g_io_channel_unix_get_fd (_readchannel), &readfds);

  if ((select_val = select (FD_SETSIZE, &readfds, NULL, NULL, tvp)) > 0)
    {
      gimp_single_message ();
    }
  else if (select_val == -1)
    {
      perror ("gimp_extension_process");
      gimp_quit ();
    }
#else
  /* Zero means infinite wait for us, but g_poll and
   * g_io_channel_win32_poll use -1 to indicate
   * infinite wait.
   */
  GPollFD pollfd;

  if (timeout == 0)
    timeout = -1;

  g_io_channel_win32_make_pollfd (_readchannel, G_IO_IN, &pollfd);

  if (g_io_channel_win32_poll (&pollfd, 1, timeout) == 1)
    gimp_single_message ();
#endif
}

/**
 * gimp_attach_new_parasite:
 * @name: the name of the #GimpParasite to create and attach.
 * @flags: the flags set on the #GimpParasite.
 * @size: the size of the parasite data in bytes.
 * @data: a pointer to the data attached with the #GimpParasite.
 *
 * Convenience function that creates a parasite and attaches it
 * to the GIMP.
 *
 * See Also: gimp_parasite_attach()
 */
void
gimp_attach_new_parasite (const gchar   *name,
			  gint           flags,
			  gint           size,
			  gconstpointer  data)
{
  GimpParasite *parasite = gimp_parasite_new (name, flags, size, data);

  gimp_parasite_attach (parasite);

  gimp_parasite_free (parasite);
}


/*  private functions  */

static void
gimp_close (void)
{
  if (gimp_debug_flags & GIMP_DEBUG_QUIT)
    gimp_debug_stop ();

  if (PLUG_IN_INFO.quit_proc)
    (* PLUG_IN_INFO.quit_proc) ();

#if defined(G_OS_WIN32) || defined(G_WITH_CYGWIN)
  CloseHandle (shm_handle);
#else
#ifdef HAVE_SHM_H
  if ((_shm_ID != -1) && _shm_addr)
    shmdt ((char*) _shm_addr);
#endif
#endif

  gp_quit_write (_writechannel, NULL);
}

static void
gimp_debug_stop (void)
{
#ifndef G_OS_WIN32
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Waiting for debugger...");
  raise (SIGSTOP);
#else
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Debugging not implemented on Win32");
#endif
}

static void
gimp_message_func (const gchar    *log_domain,
		   GLogLevelFlags  log_level,
		   const gchar    *message,
		   gpointer        data)
{
  gimp_message ((gchar *) message);
}

#ifndef G_OS_WIN32
static void
gimp_plugin_sigfatal_handler (gint sig_num)
{
  switch (sig_num)
    {
    case SIGHUP:
    case SIGINT:
    case SIGQUIT:
    case SIGABRT:
    case SIGTERM:
      g_printerr ("%s terminated: %s\n", progname, g_strsignal (sig_num));
      break;

    case SIGBUS:
    case SIGSEGV:
    case SIGFPE:
    case SIGPIPE:
    default:
      g_printerr ("%s: fatal error: %s\n", progname, g_strsignal (sig_num));
      switch (stack_trace_mode)
	{
	case GIMP_STACK_TRACE_NEVER:
	  break;

	case GIMP_STACK_TRACE_QUERY:
	  {
	    sigset_t sigset;

	    sigemptyset (&sigset);
	    sigprocmask (SIG_SETMASK, &sigset, NULL);
	    g_on_error_query (progname);
	  }
	  break;

	case GIMP_STACK_TRACE_ALWAYS:
	  {
	    sigset_t sigset;

	    sigemptyset (&sigset);
	    sigprocmask (SIG_SETMASK, &sigset, NULL);
	    g_on_error_stack_trace (progname);
	  }
	  break;
	}
      break;
    }

  gimp_quit ();
}
#endif

static gboolean
gimp_plugin_io_error_handler (GIOChannel   *channel,
			      GIOCondition  cond,
			      gpointer      data)
{
  g_printerr ("%s: fatal error: GIMP crashed\n", progname);
  gimp_quit ();

  /* never reached */
  return TRUE;
}

static gboolean
gimp_write (GIOChannel *channel,
	    guint8     *buf,
	    gulong      count,
            gpointer    user_data)
{
  gulong bytes;

  while (count > 0)
    {
      if ((write_buffer_index + count) >= WRITE_BUFFER_SIZE)
	{
	  bytes = WRITE_BUFFER_SIZE - write_buffer_index;
	  memcpy (&write_buffer[write_buffer_index], buf, bytes);
	  write_buffer_index += bytes;
	  if (! wire_flush (channel, NULL))
	    return FALSE;
	}
      else
	{
	  bytes = count;
	  memcpy (&write_buffer[write_buffer_index], buf, bytes);
	  write_buffer_index += bytes;
	}

      buf += bytes;
      count -= bytes;
    }

  return TRUE;
}

static gboolean
gimp_flush (GIOChannel *channel,
            gpointer    user_data)
{
  GIOStatus  status;
  GError    *error = NULL;
  gsize      count;
  gsize      bytes;

  if (write_buffer_index > 0)
    {
      count = 0;
      while (count != write_buffer_index)
        {
	  do
	    {
	      bytes = 0;
	      status = g_io_channel_write_chars (channel,
						 &write_buffer[count],
						 (write_buffer_index - count),
						 &bytes,
						 &error);
	    }
	  while (status == G_IO_STATUS_AGAIN);

	  if (status != G_IO_STATUS_NORMAL)
	    {
	      if (error)
		{
		  g_warning ("%s: gimp_flush(): error: %s",
			     g_get_prgname (), error->message);
		  g_error_free (error);
		}
	      else
		{
		  g_warning ("%s: gimp_flush(): error", g_get_prgname ());
		}

	      return FALSE;
	    }

          count += bytes;
        }

      write_buffer_index = 0;
    }

  return TRUE;
}

static void
gimp_loop (void)
{
  WireMessage msg;

  while (TRUE)
    {
      if (! wire_read_msg (_readchannel, &msg, NULL))
        {
	  gimp_close ();
          return;
        }

      switch (msg.type)
	{
	case GP_QUIT:
	  gimp_close ();
	  return;
	case GP_CONFIG:
	  gimp_config (msg.data);
	  break;
	case GP_TILE_REQ:
	case GP_TILE_ACK:
	case GP_TILE_DATA:
	  g_warning ("unexpected tile message received (should not happen)");
	  break;
	case GP_PROC_RUN:
	  gimp_proc_run (msg.data);
	  gimp_close ();
          return;
	case GP_PROC_RETURN:
	  g_warning ("unexpected proc return message received (should not happen)");
	  break;
	case GP_TEMP_PROC_RUN:
	  g_warning ("unexpected temp proc run message received (should not happen");
	  break;
	case GP_TEMP_PROC_RETURN:
	  g_warning ("unexpected temp proc return message received (should not happen");
	  break;
	case GP_PROC_INSTALL:
	  g_warning ("unexpected proc install message received (should not happen)");
	  break;
	case GP_HAS_INIT:
	  g_warning ("unexpected has init message received (should not happen)");
	  break;
	}

      wire_destroy (&msg);
    }
}

static void
gimp_config (GPConfig *config)
{
  if (config->version < GIMP_PROTOCOL_VERSION)
    {
      g_message ("Could not execute plug-in \"%s\"\n(%s)\n"
		 "because the GIMP is using an older version of the "
		 "plug-in protocol.",
		 g_get_prgname (), progname);
      gimp_quit ();
    }
  else if (config->version > GIMP_PROTOCOL_VERSION)
    {
      g_message ("Could not execute plug-in \"%s\"\n(%s)\n"
		 "because it uses an obsolete version of the "
		 "plug-in protocol.",
		 g_get_prgname (), progname);
      gimp_quit ();
    }

  _tile_width     = config->tile_width;
  _tile_height    = config->tile_height;
  _shm_ID         = config->shm_ID;
  _gamma_val      = config->gamma;
  _install_cmap   = config->install_cmap;
  _show_tool_tips = config->show_tool_tips;
  _min_colors     = config->min_colors;
  _gdisp_ID       = config->gdisp_ID;
  _wm_name        = g_strdup (config->wm_name);
  _wm_class       = g_strdup (config->wm_class);
  _display_name   = g_strdup (config->display_name);
  _monitor_number = config->monitor_number;

  if (_shm_ID != -1)
    {
#if defined(G_OS_WIN32) || defined(G_WITH_CYGWIN)
      /*
       * Use Win32 shared memory mechanisms for
       * transfering tile data
       */
      gchar fileMapName[128];
      gint  tileByteSize = _tile_width * _tile_height * 4;

      /* From the id, derive the file map name */
      g_snprintf (fileMapName, sizeof (fileMapName), "GIMP%d.SHM", _shm_ID);

      /* Open the file mapping */
      shm_handle = OpenFileMapping (FILE_MAP_ALL_ACCESS,
				    0, fileMapName);
      if (shm_handle)
	{
	  /* Map the shared memory into our address space for use */
	  _shm_addr = (guchar *) MapViewOfFile (shm_handle,
						FILE_MAP_ALL_ACCESS,
						0, 0, tileByteSize);

	  /* Verify that we mapped our view */
	  if (!_shm_addr)
	    {
	      g_warning ("MapViewOfFile error: %d... disabling shared memory transport",
			 GetLastError());
	    }
	}
      else
	{
	  g_warning ("OpenFileMapping error: %d... disabling shared memory transport",
		     GetLastError());
	}
#else
#ifdef HAVE_SHM_H
      _shm_addr = (guchar*) shmat (_shm_ID, 0, 0);

      if (_shm_addr == (guchar*) -1)
	g_error ("could not attach to gimp shared memory segment");
#endif
#endif
    }
}

static void
gimp_proc_run (GPProcRun *proc_run)
{
  if (PLUG_IN_INFO.run_proc)
    {
      GPProcReturn  proc_return;
      GimpParam    *return_vals;
      gint          n_return_vals;

      (* PLUG_IN_INFO.run_proc) (proc_run->name,
				 proc_run->nparams,
				 (GimpParam*) proc_run->params,
				 &n_return_vals,
				 &return_vals);

      proc_return.name    = proc_run->name;
      proc_return.nparams = n_return_vals;
      proc_return.params  = (GPParam *) return_vals;

      if (! gp_proc_return_write (_writechannel, &proc_return, NULL))
	gimp_quit ();
    }
}

static void
gimp_temp_proc_run (GPProcRun *proc_run)
{
  GimpRunProc run_proc;

  run_proc = (GimpRunProc) g_hash_table_lookup (temp_proc_ht,
						(gpointer) proc_run->name);

  if (run_proc)
    {
#ifdef ENABLE_TEMP_RETURN
      GPProcReturn  proc_return;
#endif
      GimpParam    *return_vals;
      gint          n_return_vals;

      (* run_proc) (proc_run->name,
		    proc_run->nparams,
		    (GimpParam*) proc_run->params,
		    &n_return_vals,
		    &return_vals);

#ifdef ENABLE_TEMP_RETURN
      proc_return.name    = proc_run->name;
      proc_return.nparams = n_return_vals;
      proc_return.params  = (GPParam *) return_vals;

      if (! gp_temp_proc_return_write (_writechannel, &proc_return, NULL))
        gimp_quit ();
#endif
    }
}

static void
gimp_process_message (WireMessage *msg)
{
  switch (msg->type)
    {
    case GP_QUIT:
      gimp_quit ();
      break;
    case GP_CONFIG:
      gimp_config (msg->data);
      break;
    case GP_TILE_REQ:
    case GP_TILE_ACK:
    case GP_TILE_DATA:
      g_warning ("unexpected tile message received (should not happen)");
      break;
    case GP_PROC_RUN:
      g_warning ("unexpected proc run message received (should not happen)");
      break;
    case GP_PROC_RETURN:
      g_warning ("unexpected proc return message received (should not happen)");
      break;
    case GP_TEMP_PROC_RUN:
      gimp_temp_proc_run (msg->data);
      break;
    case GP_TEMP_PROC_RETURN:
      g_warning ("unexpected temp proc return message received (should not happen)");
      break;
    case GP_PROC_INSTALL:
      g_warning ("unexpected proc install message received (should not happen)");
      break;
    case GP_HAS_INIT:
      g_warning ("unexpected has init message received (should not happen)");
      break;
    }
}

static void
gimp_single_message (void)
{
  WireMessage msg;

  /* Run a temp function */
  if (! wire_read_msg (_readchannel, &msg, NULL))
    gimp_quit ();

  gimp_process_message (&msg);

  wire_destroy (&msg);
}

static gboolean
gimp_extension_read (GIOChannel  *channel,
                     GIOCondition condition,
                     gpointer     data)
{
  gimp_single_message ();

  return TRUE;
}
