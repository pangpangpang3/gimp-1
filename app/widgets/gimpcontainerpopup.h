/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpcontainerpopup.h
 * Copyright (C) 2003 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_CONTAINER_POPUP_H__
#define __GIMP_CONTAINER_POPUP_H__


#include <gtk/gtkwindow.h>


#define GIMP_TYPE_CONTAINER_POPUP            (gimp_container_popup_get_type ())
#define GIMP_CONTAINER_POPUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_CONTAINER_POPUP, GimpContainerPopup))
#define GIMP_CONTAINER_POPUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_CONTAINER_POPUP, GimpContainerPopupClass))
#define GIMP_IS_CONTAINER_POPUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_CONTAINER_POPUP))
#define GIMP_IS_CONTAINER_POPUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_CONTAINER_POPUP))
#define GIMP_CONTAINER_POPUP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_CONTAINER_POPUP, GimpContainerPopupClass))


typedef struct _GimpContainerPopupClass  GimpContainerPopupClass;

struct _GimpContainerPopup
{
  GtkWindow            parent_instance;

  GimpContainer       *container;
  GimpContext         *orig_context;
  GimpContext         *context;

  GimpContainerEditor *editor;
};

struct _GimpContainerPopupClass
{
  GtkWindowClass  parent_instance;

  void (* cancel)  (GimpContainerPopup *popup);
  void (* confirm) (GimpContainerPopup *popup);
};


GType       gimp_container_popup_get_type (void) G_GNUC_CONST;

GtkWidget * gimp_container_popup_new      (GimpContainer      *container,
                                           GimpContext        *context);
void        gimp_container_popup_show     (GimpContainerPopup *popup,
                                           GtkWidget          *widget);


#endif  /*  __GIMP_CONTAINER_POPUP_H__  */
