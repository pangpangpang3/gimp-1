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

#include "display-types.h"

#include "config/gimpdisplayconfig.h"

#include "core/gimp.h"
#include "core/gimpimage.h"

#include "gimpdisplay.h"
#include "gimpdisplayshell.h"
#include "gimpdisplayshell-callbacks.h"
#include "gimpdisplayshell-handlers.h"
#include "gimpdisplayshell-scale.h"
#include "gimpdisplayshell-selection.h"
#include "gimpdisplayshell-title.h"
#include "gimpstatusbar.h"


/*  local function prototypes  */

static void   gimp_display_shell_clean_dirty_handler        (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_undo_event_handler         (GimpImage        *gimage,
                                                             GimpUndoEvent     event,
                                                             GimpUndo         *undo,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_name_changed_handler       (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_selection_control_handler  (GimpImage        *gimage,
                                                             GimpSelectionControl control,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_size_changed_handler       (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_resolution_changed_handler (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_unit_changed_handler       (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_qmask_changed_handler      (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_update_guide_handler       (GimpImage        *gimage,
                                                             GimpGuide        *guide,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_invalidate_preview_handler (GimpImage        *gimage,
                                                             GimpDisplayShell *shell);

static void   gimp_display_shell_check_notify_handler       (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_title_notify_handler       (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_nav_size_notify_handler    (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_monitor_res_notify_handler (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_padding_notify_handler     (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);
static void   gimp_display_shell_ants_speed_notify_handler  (GObject          *config,
                                                             GParamSpec       *param_spec,
                                                             GimpDisplayShell *shell);

static gboolean   gimp_display_shell_idle_update_icon       (gpointer          data);


/*  public functions  */

void
gimp_display_shell_connect (GimpDisplayShell *shell)
{
  GimpDisplayConfig *display_config;
  GimpImage         *gimage;

  g_return_if_fail (GIMP_IS_DISPLAY_SHELL (shell));
  g_return_if_fail (GIMP_IS_DISPLAY (shell->gdisp));
  g_return_if_fail (GIMP_IS_IMAGE (shell->gdisp->gimage));

  gimage = shell->gdisp->gimage;

  display_config = GIMP_DISPLAY_CONFIG (gimage->gimp->config);

  shell->monitor_xres  = display_config->monitor_xres;
  shell->monitor_yres  = display_config->monitor_yres;
  shell->padding_mode  = display_config->canvas_padding_mode;
  shell->padding_color = display_config->canvas_padding_color;

  g_signal_connect (gimage, "clean",
                    G_CALLBACK (gimp_display_shell_clean_dirty_handler),
                    shell);
  g_signal_connect (gimage, "dirty",
                    G_CALLBACK (gimp_display_shell_clean_dirty_handler),
                    shell);
  g_signal_connect (gimage, "undo_event",
                    G_CALLBACK (gimp_display_shell_undo_event_handler),
                    shell);
  g_signal_connect (gimage, "name_changed",
                    G_CALLBACK (gimp_display_shell_name_changed_handler),
                    shell);
  g_signal_connect (gimage, "selection_control",
                    G_CALLBACK (gimp_display_shell_selection_control_handler),
                    shell);
  g_signal_connect (gimage, "size_changed",
                    G_CALLBACK (gimp_display_shell_size_changed_handler),
                    shell);
  g_signal_connect (gimage, "resolution_changed",
                    G_CALLBACK (gimp_display_shell_resolution_changed_handler),
                    shell);
  g_signal_connect (gimage, "unit_changed",
                    G_CALLBACK (gimp_display_shell_unit_changed_handler),
                    shell);
  g_signal_connect (gimage, "qmask_changed",
                    G_CALLBACK (gimp_display_shell_qmask_changed_handler),
                    shell);
  g_signal_connect (gimage, "update_guide",
                    G_CALLBACK (gimp_display_shell_update_guide_handler),
                    shell);
  g_signal_connect (gimage, "invalidate_preview",
		    G_CALLBACK (gimp_display_shell_invalidate_preview_handler),
		    shell);

  g_signal_connect (gimage->gimp->config,
                    "notify::transparency-size",
                    G_CALLBACK (gimp_display_shell_check_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::transparency-type",
                    G_CALLBACK (gimp_display_shell_check_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::image-title-format",
                    G_CALLBACK (gimp_display_shell_title_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::image-status-format",
                    G_CALLBACK (gimp_display_shell_title_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::navigation-preview-size",
                    G_CALLBACK (gimp_display_shell_nav_size_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::monitor-xresolution",
                    G_CALLBACK (gimp_display_shell_monitor_res_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::monitor-yresolution",
                    G_CALLBACK (gimp_display_shell_monitor_res_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::canvas-padding-mode",
                    G_CALLBACK (gimp_display_shell_padding_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::canvas-padding-color",
                    G_CALLBACK (gimp_display_shell_padding_notify_handler),
                    shell);
  g_signal_connect (gimage->gimp->config,
                    "notify::marching-ants-speed",
                    G_CALLBACK (gimp_display_shell_ants_speed_notify_handler),
                    shell);

  gimp_display_shell_invalidate_preview_handler (gimage, shell);
  gimp_display_shell_qmask_changed_handler (gimage, shell);
}

void
gimp_display_shell_disconnect (GimpDisplayShell *shell)
{
  GimpImage *gimage;

  g_return_if_fail (GIMP_IS_DISPLAY_SHELL (shell));
  g_return_if_fail (GIMP_IS_DISPLAY (shell->gdisp));
  g_return_if_fail (GIMP_IS_IMAGE (shell->gdisp->gimage));

  gimage = shell->gdisp->gimage;

  if (shell->icon_idle_id)
    {
      g_source_remove (shell->icon_idle_id);
      shell->icon_idle_id = 0;
    }

  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_ants_speed_notify_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_padding_notify_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_monitor_res_notify_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_nav_size_notify_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_title_notify_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage->gimp->config,
                                        gimp_display_shell_check_notify_handler,
                                        shell);

  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_invalidate_preview_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_update_guide_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_qmask_changed_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_unit_changed_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_resolution_changed_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_size_changed_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_selection_control_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_name_changed_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_undo_event_handler,
                                        shell);
  g_signal_handlers_disconnect_by_func (gimage,
                                        gimp_display_shell_clean_dirty_handler,
                                        shell);
}


/*  private functions  */

static void
gimp_display_shell_clean_dirty_handler (GimpImage        *gimage,
                                        GimpDisplayShell *shell)
{
  gimp_display_shell_update_title (shell);
}

static void
gimp_display_shell_undo_event_handler (GimpImage        *gimage,
                                       GimpUndoEvent     event,
                                       GimpUndo         *undo,
                                       GimpDisplayShell *shell)
{
  gimp_display_shell_update_title (shell);
}

static void
gimp_display_shell_name_changed_handler (GimpImage        *gimage,
                                         GimpDisplayShell *shell)
{
  gimp_display_shell_update_title (shell);
}

static void
gimp_display_shell_selection_control_handler (GimpImage            *gimage,
                                              GimpSelectionControl  control,
                                              GimpDisplayShell     *shell)
{
  gimp_display_shell_selection_visibility (shell, control);
}

static void
gimp_display_shell_size_changed_handler (GimpImage        *gimage,
                                         GimpDisplayShell *shell)
{
  gimp_statusbar_resize_cursor (GIMP_STATUSBAR (shell->statusbar));

  gimp_display_shell_scale_resize (shell,
				   GIMP_DISPLAY_CONFIG (gimage->gimp->config)->resize_windows_on_resize,
				   TRUE);
}

static void
gimp_display_shell_resolution_changed_handler (GimpImage        *gimage,
                                               GimpDisplayShell *shell)
{
  if (! shell->dot_for_dot)
    gimp_display_shell_scale_setup (shell);

  gimp_statusbar_resize_cursor (GIMP_STATUSBAR (shell->statusbar));
}

static void
gimp_display_shell_unit_changed_handler (GimpImage        *gimage,
                                         GimpDisplayShell *shell)
{
  if (! shell->dot_for_dot)
    gimp_display_shell_scale_setup (shell);

  gimp_statusbar_resize_cursor (GIMP_STATUSBAR (shell->statusbar));
}

static void
gimp_display_shell_qmask_changed_handler (GimpImage        *gimage,
                                          GimpDisplayShell *shell)
{
  GtkImage *image;

  image = GTK_IMAGE (GTK_BIN (shell->qmask)->child);

  g_signal_handlers_block_by_func (shell->qmask,
                                   gimp_display_shell_qmask_toggled,
                                   shell);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (shell->qmask),
                                shell->gdisp->gimage->qmask_state);

  if (shell->gdisp->gimage->qmask_state)
    gtk_image_set_from_stock (image, GIMP_STOCK_QMASK_ON, GTK_ICON_SIZE_MENU);
  else
    gtk_image_set_from_stock (image, GIMP_STOCK_QMASK_OFF, GTK_ICON_SIZE_MENU);

  g_signal_handlers_unblock_by_func (shell->qmask,
                                     gimp_display_shell_qmask_toggled,
                                     shell);
}

static void
gimp_display_shell_update_guide_handler (GimpImage        *gimage,
                                         GimpGuide        *guide,
                                         GimpDisplayShell *shell)
{
  gimp_display_shell_expose_guide (shell, guide);
}

static void
gimp_display_shell_invalidate_preview_handler (GimpImage        *gimage,
                                               GimpDisplayShell *shell)
{
  if (shell->icon_idle_id)
    g_source_remove (shell->icon_idle_id);

  shell->icon_idle_id = g_idle_add_full (G_PRIORITY_LOW,
                                         gimp_display_shell_idle_update_icon,
                                         shell,
                                         NULL);
}

static void
gimp_display_shell_check_notify_handler (GObject          *config,
                                         GParamSpec       *param_spec,
                                         GimpDisplayShell *shell)
{
  switch (shell->padding_mode)
    {
    case GIMP_DISPLAY_PADDING_MODE_LIGHT_CHECK:
    case GIMP_DISPLAY_PADDING_MODE_DARK_CHECK:
      gimp_display_shell_set_padding (shell,
                                      shell->padding_mode,
                                      &shell->padding_color);
      break;

    default:
      break;
    }

  gimp_display_shell_expose_full (shell);
}

static void
gimp_display_shell_title_notify_handler (GObject          *config,
                                         GParamSpec       *param_spec,
                                         GimpDisplayShell *shell)
{
  gimp_display_shell_update_title (shell);
}

static void
gimp_display_shell_nav_size_notify_handler (GObject          *config,
                                            GParamSpec       *param_spec,
                                            GimpDisplayShell *shell)
{
  if (shell->nav_popup)
    {
      gtk_widget_destroy (shell->nav_popup);
      shell->nav_popup = NULL;
    }
}

static void
gimp_display_shell_monitor_res_notify_handler (GObject          *config,
                                               GParamSpec       *param_spec,
                                               GimpDisplayShell *shell)
{
  shell->monitor_xres = GIMP_DISPLAY_CONFIG (config)->monitor_xres;
  shell->monitor_yres = GIMP_DISPLAY_CONFIG (config)->monitor_yres;

  if (! shell->dot_for_dot)
    {
      gimp_display_shell_scale_setup (shell);

      gimp_statusbar_resize_cursor (GIMP_STATUSBAR (shell->statusbar));

      gimp_display_shell_expose_full (shell);
    }
}

static void
gimp_display_shell_padding_notify_handler (GObject          *config,
                                           GParamSpec       *param_spec,
                                           GimpDisplayShell *shell)
{
  /*  if the user did not set the padding mode for this display explicitely  */
  if (! shell->padding_mode_set)
    {
      GimpDisplayConfig *display_config;

      display_config = GIMP_DISPLAY_CONFIG (config);

      gimp_display_shell_set_padding (shell,
                                      display_config->canvas_padding_mode,
                                      &display_config->canvas_padding_color);
    }
}

static void
gimp_display_shell_ants_speed_notify_handler (GObject          *config,
                                              GParamSpec       *param_spec,
                                              GimpDisplayShell *shell)
{
  gimp_display_shell_selection_pause (shell->select);
  gimp_display_shell_selection_resume (shell->select);
}

static gboolean
gimp_display_shell_idle_update_icon (gpointer data)
{
  GimpDisplayShell *shell;

  shell = GIMP_DISPLAY_SHELL (data);

  shell->icon_idle_id = 0;

  gimp_display_shell_update_icon (shell);

  return FALSE;
}
