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

#include <glib-object.h>
#include <pango/pangoft2.h>

#include "libgimpbase/gimpbase.h"

#include "text-types.h"

#include "base/pixel-region.h"
#include "base/tile-manager.h"

#include "core/gimpimage.h"

#include "gimptext.h"
#include "gimptextlayout.h"


struct _GimpTextLayout
{
  GObject         object;

  GimpText       *text;
  PangoLayout    *layout;
  PangoRectangle  extents;
};

struct _GimpTextLayoutClass
{
  GObjectClass   parent_class;
};


static void   gimp_text_layout_class_init  (GimpTextLayoutClass *klass);
static void   gimp_text_layout_init        (GimpTextLayout      *layout);
static void   gimp_text_layout_finalize    (GObject             *object);

static void   gimp_text_layout_position    (GimpTextLayout      *layout);

static PangoContext * gimp_text_get_pango_context (GimpText     *text,
                                                   gdouble       xres,
                                                   gdouble       yres);


static GObjectClass * parent_class = NULL;


GType
gimp_text_layout_get_type (void)
{
  static GType layout_type = 0;

  if (! layout_type)
    {
      static const GTypeInfo layout_info =
      {
        sizeof (GimpTextLayoutClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_text_layout_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpTextLayout),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_text_layout_init,
      };

      layout_type = g_type_register_static (G_TYPE_OBJECT,
                                            "GimpTextLayout",
                                            &layout_info, 0);
    }

  return layout_type;
}

static void
gimp_text_layout_class_init (GimpTextLayoutClass *klass)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = gimp_text_layout_finalize;
}

static void
gimp_text_layout_init (GimpTextLayout *layout)
{
  layout->text   = NULL;
  layout->layout = NULL;
}

static void
gimp_text_layout_finalize (GObject *object)
{
  GimpTextLayout *layout;

  layout = GIMP_TEXT_LAYOUT (object);

  if (layout->text)
    {
      g_object_unref (layout->text);
      layout->text = NULL;
    }
  if (layout->layout)
    {
      g_object_unref (layout->layout);
      layout->layout = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}


GimpTextLayout *
gimp_text_layout_new (GimpText  *text,
                      GimpImage *image)
{
  GimpTextLayout       *layout;
  PangoContext         *context;
  PangoFontDescription *font_desc;
  PangoAlignment        alignment = PANGO_ALIGN_LEFT;
  gdouble               xres, yres;
  gint                  size;

  g_return_val_if_fail (GIMP_IS_TEXT (text), NULL);
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  font_desc = pango_font_description_from_string (text->font);
  g_return_val_if_fail (font_desc != NULL, NULL);
  if (!font_desc)
    return NULL;

  gimp_image_get_resolution (image, &xres, &yres);

  switch (text->font_size_unit)
    {
    case GIMP_UNIT_PIXEL:
      size = PANGO_SCALE * text->font_size;
      break;

    default:
      {
	gdouble factor;

	factor = gimp_unit_get_factor (text->font_size_unit);
	g_return_val_if_fail (factor > 0.0, NULL);

	size = (gdouble) PANGO_SCALE * text->font_size * yres / factor;
      }
      break;
    }
  
  pango_font_description_set_size (font_desc, MAX (1, size));

  context = gimp_text_get_pango_context (text, xres, yres);

  layout = g_object_new (GIMP_TYPE_TEXT_LAYOUT, NULL);
  layout->text   = g_object_ref (text);
  layout->layout = pango_layout_new (context);

  g_object_unref (context);

  pango_layout_set_font_description (layout->layout, font_desc);
  pango_font_description_free (font_desc);

  pango_layout_set_text (layout->layout, text->text, -1);

  switch (text->justify)
    {
    case GIMP_TEXT_JUSTIFY_LEFT:
      alignment = PANGO_ALIGN_LEFT;
      break;
    case GIMP_TEXT_JUSTIFY_RIGHT:
      alignment = PANGO_ALIGN_RIGHT;
      break;
    case GIMP_TEXT_JUSTIFY_CENTER:
      alignment = PANGO_ALIGN_CENTER;
      break;
    case GIMP_TEXT_JUSTIFY_FILL:
      /* FIXME: This doesn't work since the implementation is missing
         at the Pango level.
       */
      alignment = PANGO_ALIGN_LEFT;
      pango_layout_set_justify (layout->layout, TRUE);
      break;
    }

  pango_layout_set_alignment (layout->layout, alignment);

  pango_layout_set_width (layout->layout,
			  text->box_width > 0 ? 
			  text->box_width * PANGO_SCALE : -1);

  pango_layout_set_indent (layout->layout, text->indent * PANGO_SCALE);
  pango_layout_set_spacing (layout->layout, text->line_spacing * PANGO_SCALE);

  gimp_text_layout_position (layout);

  return layout;
}

gboolean
gimp_text_layout_get_size (GimpTextLayout *layout,
                           gint           *width,
                           gint           *height)
{
  g_return_val_if_fail (GIMP_IS_TEXT_LAYOUT (layout), FALSE);

  if (width)
    *width = layout->extents.width;
  if (height)
    *height = layout->extents.height;

  return (layout->extents.width > 0 && layout->extents.height > 0);
}

void
gimp_text_layout_get_offsets (GimpTextLayout *layout,
                              gint           *x,
                              gint           *y)
{
  g_return_if_fail (GIMP_IS_TEXT_LAYOUT (layout));

  if (x)
    *x = layout->extents.x;
  if (y)
    *y = layout->extents.y;
}

TileManager *
gimp_text_layout_render (GimpTextLayout *layout,
                         gint            width,
                         gint            height)
{
  TileManager  *mask;
  FT_Bitmap     bitmap;
  PixelRegion   maskPR;
  gint          i;
  gint          x, y;

  g_return_val_if_fail (GIMP_IS_TEXT_LAYOUT (layout), NULL);

  gimp_text_layout_get_offsets (layout, &x, &y);

  bitmap.width = width;
  bitmap.rows  = height;
  bitmap.pitch = width;
  if (bitmap.pitch & 3)
    bitmap.pitch += 4 - (bitmap.pitch & 3);

  bitmap.buffer = g_malloc0 (bitmap.rows * bitmap.pitch);
  
  pango_ft2_render_layout (&bitmap, layout->layout, x, y);

  mask = tile_manager_new (width, height, 1);
  pixel_region_init (&maskPR, mask, 0, 0, width, height, TRUE);

  for (i = 0; i < height; i++)
    pixel_region_set_row (&maskPR,
			  0, i, width, bitmap.buffer + i * bitmap.pitch);

  g_free (bitmap.buffer);

  return mask;
}

static void
gimp_text_layout_position (GimpTextLayout *layout)
{
  PangoRectangle  ink;
  PangoRectangle  logical;
  gint            x1, y1;
  gint            x2, y2;

  layout->extents.x      = 0;
  layout->extents.x      = 0;
  layout->extents.width  = 0;
  layout->extents.height = 0;
  
  pango_layout_get_pixel_extents (layout->layout, &ink, &logical);

#ifdef VERBOSE
  g_print ("ink rect: %d x %d @ %d, %d\n", 
           ink.width, ink.height, ink.x, ink.y);
  g_print ("logical rect: %d x %d @ %d, %d\n", 
           logical.width, logical.height, logical.x, logical.y);
#endif

  if (ink.width < 1 || ink.height < 1)
    return;

  x1 = MIN (0, logical.x);
  y1 = MIN (0, logical.y);
  x2 = MAX (ink.x + ink.width,  logical.x + logical.width);
  y2 = MAX (ink.y + ink.height, logical.y + logical.height);

  layout->extents.width  = x2 - x1;
  layout->extents.height = y2 - y1;

  /* border should only be used by the compatibility API */
  if (layout->text->border > 0)
    {
      gint border = layout->text->border;

      layout->extents.x      += border;     
      layout->extents.y      += border;     
      layout->extents.width  += 2 * border;
      layout->extents.height += 2 * border;
    }

#ifdef VERBOSE
  g_print ("layout extents: %d x %d @ %d, %d\n", 
           layout->extents.width, layout->extents.height,
           layout->extents.x, layout->extents.y);
#endif
}


static void
gimp_text_ft2_subst_func (FcPattern *pattern,
                          gpointer   data)
{
  GimpText *text = GIMP_TEXT (data);

  FcPatternAddBool (pattern, FC_HINTING,   text->hinting);
  FcPatternAddBool (pattern, FC_AUTOHINT,  text->autohint);
  FcPatternAddBool (pattern, FC_ANTIALIAS, text->antialias);
}

static PangoContext *
gimp_text_get_pango_context (GimpText *text,
                             gdouble   xres,
                             gdouble   yres)
{
  PangoContext    *context;
  PangoFT2FontMap *fontmap;

  fontmap = PANGO_FT2_FONT_MAP (pango_ft2_font_map_new ());
  
  pango_ft2_font_map_set_resolution (fontmap, xres, yres);

  pango_ft2_font_map_set_default_substitute (fontmap,
                                             gimp_text_ft2_subst_func,
                                             g_object_ref (text),
                                             (GDestroyNotify) g_object_unref);

  context = pango_ft2_font_map_create_context (fontmap);
  g_object_unref (fontmap);

  if (text->language)
    pango_context_set_language (context,
                                pango_language_from_string (text->language));

  switch (text->base_dir)
    {
    case GIMP_TEXT_DIRECTION_LTR:
      pango_context_set_base_dir (context, PANGO_DIRECTION_LTR);
      break;
    case GIMP_TEXT_DIRECTION_RTL:
      pango_context_set_base_dir (context, PANGO_DIRECTION_RTL);
      break;
    }

  return context;
}
