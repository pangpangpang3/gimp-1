/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
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

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "actions-types.h"

#include "core/gimp.h"
#include "core/gimpcontext.h"
#include "core/gimplist.h"
#include "core/gimptoolinfo.h"

#include "widgets/gimpactiongroup.h"
#include "widgets/gimphelp-ids.h"

#include "tool-options-actions.h"
#include "tool-options-commands.h"

#include "gimp-intl.h"


/*  local function prototypes  */

static void tool_options_actions_update_presets (GimpActionGroup *group,
                                                 const gchar     *action_prefix,
                                                 GCallback        callback,
                                                 const gchar     *stock_id,
                                                 const gchar     *help_id,
                                                 GimpContainer   *presets);


/*  global variables  */

static GimpActionEntry tool_options_actions[] =
{
  { "tool-options-popup", GIMP_STOCK_TOOL_OPTIONS,  N_("Tool Options Menu") },

  { "tool-options-save-menu",    NULL, N_("_Save Options to")      },
  { "tool-options-restore-menu", NULL, N_("_Restore Options from") },
  { "tool-options-rename-menu",  NULL, N_("Re_name Saved Options") },
  { "tool-options-delete-menu",  NULL, N_("_Delete Saved Options") },

  { "tool-options-save-new", GTK_STOCK_NEW,
    N_("_New Entry..."), "", NULL,
    G_CALLBACK (tool_options_save_new_cmd_callback),
    GIMP_HELP_TOOL_OPTIONS_SAVE },

  { "tool-options-reset", GIMP_STOCK_RESET,
    N_("R_eset Tool Options"), "", NULL,
    G_CALLBACK (tool_options_reset_cmd_callback),
    GIMP_HELP_TOOL_OPTIONS_RESET },

  { "tool-options-reset-all", GIMP_STOCK_RESET,
    N_("Reset _all Tool Options..."), "", NULL,
    G_CALLBACK (tool_options_reset_all_cmd_callback),
    GIMP_HELP_TOOL_OPTIONS_RESET }
};


/*  public functions  */

#define SET_VISIBLE(action,condition) \
        gimp_action_group_set_action_visible (group, action, (condition) != 0)
#define SET_IMPORTANT(action,condition) \
        gimp_action_group_set_action_important (group, action, (condition) != 0)

void
tool_options_actions_setup (GimpActionGroup *group)
{
  gimp_action_group_add_actions (group,
                                 tool_options_actions,
                                 G_N_ELEMENTS (tool_options_actions));

  SET_IMPORTANT ("tool-options-restore-menu", TRUE);
  SET_IMPORTANT ("tool-options-rename-menu",  TRUE);
  SET_IMPORTANT ("tool-options-delete-menu",  TRUE);
}

void
tool_options_actions_update (GimpActionGroup *group,
                             gpointer         data)
{
  GimpContext  *context;
  GimpToolInfo *tool_info;

  context   = gimp_get_user_context (group->gimp);
  tool_info = gimp_context_get_tool (context);

  SET_VISIBLE ("tool-options-save-menu",    tool_info->options_presets);
  SET_VISIBLE ("tool-options-restore-menu", tool_info->options_presets);
  SET_VISIBLE ("tool-options-rename-menu",  tool_info->options_presets);
  SET_VISIBLE ("tool-options-delete-menu",  tool_info->options_presets);

  tool_options_actions_update_presets (group, "tool-options-save-",
                                       G_CALLBACK (tool_options_save_to_cmd_callback),
                                       GTK_STOCK_SAVE,
                                       GIMP_HELP_TOOL_OPTIONS_SAVE,
                                       tool_info->options_presets);

  tool_options_actions_update_presets (group, "tool-options-restore-",
                                       G_CALLBACK (tool_options_restore_from_cmd_callback),
                                       GTK_STOCK_REVERT_TO_SAVED,
                                       GIMP_HELP_TOOL_OPTIONS_RESTORE,
                                       tool_info->options_presets);

  tool_options_actions_update_presets (group, "tool-options-rename-",
                                       G_CALLBACK (tool_options_rename_saved_cmd_callback),
                                       GIMP_STOCK_EDIT,
                                       GIMP_HELP_TOOL_OPTIONS_RENAME,
                                       tool_info->options_presets);

  tool_options_actions_update_presets (group, "tool-options-delete-",
                                       G_CALLBACK (tool_options_delete_saved_cmd_callback),
                                       GTK_STOCK_DELETE,
                                       GIMP_HELP_TOOL_OPTIONS_DELETE,
                                       tool_info->options_presets);
}


/*  privat function  */

static void
tool_options_actions_update_presets (GimpActionGroup *group,
                                     const gchar     *action_prefix,
                                     GCallback        callback,
                                     const gchar     *stock_id,
                                     const gchar     *help_id,
                                     GimpContainer   *presets)
{
  GList *list;
  gint   n_children = 0;
  gint   i;

  for (i = 0; ; i++)
    {
      gchar     *action_name;
      GtkAction *action;

      action_name = g_strdup_printf ("%s%03d", action_prefix, i);
      action = gtk_action_group_get_action (GTK_ACTION_GROUP (group),
                                            action_name);
      g_free (action_name);

      if (! action)
        break;

      gtk_action_group_remove_action (GTK_ACTION_GROUP (group), action);
    }

  if (presets)
    n_children = gimp_container_num_children (presets);

  if (n_children > 0)
    {
      GimpEnumActionEntry entry;

      entry.name        = NULL;
      entry.stock_id    = stock_id;
      entry.label       = NULL;
      entry.accelerator = "";
      entry.tooltip     = NULL;
      entry.value       = 0;
      entry.help_id     = help_id;

      for (list = GIMP_LIST (presets)->list, i = 0;
           list;
           list = g_list_next (list), i++)
        {
          GimpToolOptions *options = list->data;

          entry.name  = g_strdup_printf ("%s%03d", action_prefix, i);
          entry.label = gimp_object_get_name (GIMP_OBJECT (options));
          entry.value = i;

          gimp_action_group_add_enum_actions (group, &entry, 1, callback);

          g_free ((gchar *) entry.name);
        }
    }
}

#undef SET_VISIBLE
#undef SET_IMPORTANT
