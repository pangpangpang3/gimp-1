/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GimpText
 * Copyright (C) 2002-2003  Sven Neumann <sven@gimp.org>
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

#include <stdlib.h>
#include <string.h>

#include <locale.h>

#include <glib-object.h>
#include <pango/pango.h>

#include "libgimpbase/gimplimits.h"
#include "libgimpcolor/gimpcolor.h"

#include "text/text-types.h"

#include "config/gimpconfig.h"
#include "config/gimpconfig-params.h"

#include "gimptext.h"

#include "gimp-intl.h"


enum
{
  PROP_0,
  PROP_TEXT,
  PROP_FONT,
  PROP_FONT_SIZE,
  PROP_FONT_SIZE_UNIT,
  PROP_HINTING,
  PROP_AUTOHINT,
  PROP_ANTIALIAS,
  PROP_LANGUAGE,
  PROP_BASE_DIR,
  PROP_COLOR,
  PROP_JUSTIFICATION,
  PROP_INDENTATION,
  PROP_LINE_SPACING,
  PROP_LETTER_SPACING,
  PROP_BOX_WIDTH,
  PROP_BORDER
};

static void    gimp_text_class_init           (GimpTextClass *klass);
static void    gimp_text_finalize             (GObject       *object);
static void    gimp_text_get_property         (GObject       *object,
                                               guint          property_id,
                                               GValue        *value,
                                               GParamSpec    *pspec);
static void    gimp_text_set_property         (GObject       *object,
                                               guint          property_id,
                                               const GValue  *value,
                                               GParamSpec    *pspec);
static gchar * gimp_text_get_default_language (void);


static GObjectClass *parent_class = NULL;


GType
gimp_text_get_type (void)
{
  static GType text_type = 0;

  if (! text_type)
    {
      static const GTypeInfo text_info =
      {
        sizeof (GimpTextClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_text_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpText),
	0,              /* n_preallocs    */
	NULL            /* instance_init  */
      };
      static const GInterfaceInfo text_iface_info = 
      {
        NULL,           /* iface_init     */
        NULL,           /* iface_finalize */ 
        NULL            /* iface_data     */
      };

      text_type = g_type_register_static (G_TYPE_OBJECT,
                                          "GimpText", &text_info, 0);

      g_type_add_interface_static (text_type,
                                   GIMP_TYPE_CONFIG_INTERFACE,
                                   &text_iface_info);
    }

  return text_type;
}

static void
gimp_text_class_init (GimpTextClass *klass)
{
  GObjectClass *object_class;
  GParamSpec   *param_spec;
  GimpRGB       black;
  gchar        *language;

  object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize     = gimp_text_finalize;
  object_class->get_property = gimp_text_get_property;
  object_class->set_property = gimp_text_set_property;

  gimp_rgba_set (&black, 0.0, 0.0, 0.0, GIMP_OPACITY_OPAQUE);

  language = gimp_text_get_default_language ();

  GIMP_CONFIG_INSTALL_PROP_STRING (object_class, PROP_TEXT,
				   "text", NULL,
				   NULL,
				   0);
  GIMP_CONFIG_INSTALL_PROP_STRING (object_class, PROP_FONT,
				   "font", NULL,
				   "Sans",
				   0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_FONT_SIZE,
				   "font-size", NULL,
				   0.0, 8192.0, 18.0,
				   0);
  GIMP_CONFIG_INSTALL_PROP_UNIT (object_class, PROP_FONT_SIZE_UNIT,
				 "font-size-unit", NULL,
				 TRUE, GIMP_UNIT_PIXEL,
				 0);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_HINTING,
                                    "hinting",
                                    N_("Hinting alters the font outline to "
                                       "produce a crisp bitmap at small sizes"),
                                    TRUE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_AUTOHINT,
                                    "autohint", NULL,
                                    FALSE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_ANTIALIAS,
                                    "antialias", NULL,
                                    TRUE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_STRING (object_class, PROP_LANGUAGE,
				   "language", NULL,
				   language,
				   0);
  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_BASE_DIR,
                                "base-direction",
                                 NULL,
                                 PANGO_TYPE_DIRECTION,
                                 PANGO_DIRECTION_LTR,
                                 0);
  GIMP_CONFIG_INSTALL_PROP_COLOR (object_class, PROP_COLOR,
				  "color", NULL,
				  &black,
				  0);
  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_JUSTIFICATION,
                                "justify",
                                 NULL,
                                 GIMP_TYPE_TEXT_JUSTIFICATION,
                                 GIMP_TEXT_JUSTIFY_LEFT,
                                 0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_INDENTATION,
				   "indent",
                                   N_("How many pixels the "
				      "first line should be shorter"),
				   -8192.0, 8192.0, 0.0,
				   0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_LINE_SPACING,
				   "line-spacing",
                                   N_("Additional line spacing (in pixels)"),
				   -8192.0, 8192.0, 0.0,
				   0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_LETTER_SPACING,
				   "letter-spacing", NULL,
                                    -8192.0, 8192.0, 0.0,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_INT (object_class, PROP_BOX_WIDTH,
                                "box-width", NULL,
                                0, GIMP_MAX_IMAGE_SIZE, 0,
                                0);

  /*  border does only exist to implement the old text API  */
  param_spec = g_param_spec_int ("border", NULL, NULL,
                                 0, GIMP_MAX_IMAGE_SIZE, 0,
                                 G_PARAM_CONSTRUCT | G_PARAM_WRITABLE);
  g_object_class_install_property (object_class, PROP_BORDER, param_spec);

  g_free (language);
}

static void
gimp_text_finalize (GObject *object)
{
  GimpText *text = GIMP_TEXT (object);

  if (text->text)
    {
      g_free (text->text);
      text->text = NULL;
    }
  if (text->font)
    {
      g_free (text->font);
      text->font = NULL;
    }
  if (text->language)
    {
      g_free (text->language);
      text->language = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_text_get_property (GObject      *object,
                        guint         property_id,
                        GValue       *value,
                        GParamSpec   *pspec)
{
  GimpText *text = GIMP_TEXT (object);

  switch (property_id)
    {
    case PROP_TEXT:
      g_value_set_string (value, text->text);
      break;
    case PROP_FONT:
      g_value_set_string (value, text->font);
      break;
    case PROP_FONT_SIZE:
      g_value_set_double (value, text->font_size);
      break;
    case PROP_FONT_SIZE_UNIT:
      g_value_set_int (value, text->font_size_unit);
      break;
    case PROP_HINTING:
      g_value_set_boolean (value, text->hinting);
      break;
    case PROP_AUTOHINT:
      g_value_set_boolean (value, text->autohint);
      break;
    case PROP_ANTIALIAS:
      g_value_set_boolean (value, text->antialias);
      break;
    case PROP_BASE_DIR:
      g_value_set_enum (value, text->base_dir);
      break;
    case PROP_LANGUAGE:
      g_value_set_string (value, text->language);
      break;
    case PROP_COLOR:
      g_value_set_boxed (value, &text->color);
      break;
    case PROP_JUSTIFICATION:
      g_value_set_enum (value, text->justify);
      break;
    case PROP_INDENTATION:
      g_value_set_double (value, text->indent);
      break;
    case PROP_LINE_SPACING:
      g_value_set_double (value, text->line_spacing);
      break;
    case PROP_LETTER_SPACING:
      g_value_set_double (value, text->letter_spacing);
      break;
    case PROP_BOX_WIDTH:
      g_value_set_int (value, text->box_width);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_text_set_property (GObject      *object,
                        guint         property_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  GimpText *text = GIMP_TEXT (object);
  GimpRGB  *color;

  switch (property_id)
    {
    case PROP_TEXT:
      g_free (text->text);
      text->text = g_value_dup_string (value);
      break;
    case PROP_FONT:
      g_free (text->font);
      text->font = g_value_dup_string (value);
      break;
    case PROP_FONT_SIZE:
      text->font_size = g_value_get_double (value);
      break;
    case PROP_FONT_SIZE_UNIT:
      text->font_size_unit = g_value_get_int (value);
      break;
    case PROP_HINTING:
      text->hinting = g_value_get_boolean (value);
      break;
    case PROP_AUTOHINT:
      text->autohint = g_value_get_boolean (value);
      break;
    case PROP_ANTIALIAS:
      text->antialias = g_value_get_boolean (value);
      break;
    case PROP_LANGUAGE:
      g_free (text->language);
      text->language = g_value_dup_string (value);
      break;
    case PROP_BASE_DIR:
      text->base_dir = g_value_get_enum (value);
      break;
    case PROP_COLOR:
      color = g_value_get_boxed (value);
      text->color = *color;
      break;
    case PROP_JUSTIFICATION:
      text->justify = g_value_get_enum (value);
      break;
    case PROP_INDENTATION:
      text->indent = g_value_get_double (value);
      break;
    case PROP_LINE_SPACING:
      text->line_spacing = g_value_get_double (value);
      break;
    case PROP_LETTER_SPACING:
      text->letter_spacing = g_value_get_double (value);
      break;
    case PROP_BOX_WIDTH:
      text->box_width = g_value_get_int (value);
      break;
    case PROP_BORDER:
      text->border = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}


/* 
 *  basically copied from gtk_get_default_language()
 */
static gchar *
gimp_text_get_default_language (void)
{
  gchar *lang;
  gchar *p;
  
#ifdef G_OS_WIN32
  p = getenv ("LC_ALL");
  if (p != NULL)
    lang = g_strdup (p);
  else
    {
      p = getenv ("LANG");
      if (p != NULL)
	lang = g_strdup (p);
      else
	{
	  p = getenv ("LC_CTYPE");
	  if (p != NULL)
	    lang = g_strdup (p);
	  else
	    lang = g_win32_getlocale ();
	}
    }
#else
  lang = g_strdup (setlocale (LC_CTYPE, NULL));
#endif
  p = strchr (lang, '.');
  if (p)
    *p = '\0';
  p = strchr (lang, '@');
  if (p)
    *p = '\0';

  return lang;
}
