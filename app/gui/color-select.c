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

#include <stdlib.h>
#include <string.h>

#include "appenv.h"
#include "color_select.h"
#include "colormaps.h"
#include "gimpdnd.h"
#include "gimprc.h"
#include "gimpui.h"
#include "session.h"
#include "color_area.h" /* for color_area_draw_rect */

#include "libgimp/color_selector.h"

#include "libgimp/gimpintl.h"


#define XY_DEF_WIDTH       240
#define XY_DEF_HEIGHT      240
#define Z_DEF_WIDTH        15
#define Z_DEF_HEIGHT       240
#define COLOR_AREA_WIDTH   74
#define COLOR_AREA_HEIGHT  20

#define COLOR_AREA_MASK GDK_EXPOSURE_MASK | \
                        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | \
			GDK_BUTTON1_MOTION_MASK | GDK_ENTER_NOTIFY_MASK

typedef enum
{
  COLOR_SELECT_OK,
  COLOR_SELECT_CANCEL,
  COLOR_SELECT_UPDATE
} ColorSelectState;

typedef enum
{
  COLOR_SELECT_HUE = 0,
  COLOR_SELECT_SATURATION,
  COLOR_SELECT_VALUE,
  COLOR_SELECT_RED,
  COLOR_SELECT_GREEN,
  COLOR_SELECT_BLUE,
  COLOR_SELECT_HUE_SATURATION,
  COLOR_SELECT_HUE_VALUE,
  COLOR_SELECT_SATURATION_VALUE,
  COLOR_SELECT_RED_GREEN,
  COLOR_SELECT_RED_BLUE,
  COLOR_SELECT_GREEN_BLUE
} ColorSelectFillType;

typedef enum
{
  UPDATE_VALUES     = 1 << 0,
  UPDATE_POS        = 1 << 1,
  UPDATE_XY_COLOR   = 1 << 2,
  UPDATE_Z_COLOR    = 1 << 3,
  UPDATE_NEW_COLOR  = 1 << 4,
  UPDATE_ORIG_COLOR = 1 << 5,
  UPDATE_CALLER     = 1 << 6
} ColorSelectUpdateType;

typedef void (*ColorSelectCallback) (gint, gint, gint, ColorSelectState, void *);

typedef struct _ColorSelect ColorSelect, *ColorSelectP;

struct _ColorSelect
{
  GtkWidget     *xy_color;
  GtkWidget     *z_color;
  GtkWidget     *new_color;
  GtkWidget     *orig_color;
  GtkWidget     *toggles[6];
  GtkWidget     *entries[6];
  GtkWidget     *hex_entry;
  GtkAdjustment *slider_data[6];

  gint      pos[3];
  gint      values[6];
  gint      z_color_fill;
  gint      xy_color_fill;
  gint      orig_values[3];
  gboolean  wants_updates;
  GdkGC    *gc;

  ColorSelectCallback  callback;
  void                *client_data;
};

typedef struct _ColorSelectFill ColorSelectFill;

typedef void (*ColorSelectFillUpdateProc) (ColorSelectFill *);

struct _ColorSelectFill
{
  guchar *buffer;
  gint    y;
  gint    width;
  gint    height;
  gint   *values;

  ColorSelectFillUpdateProc update;
};

static GtkWidget * color_select_widget_new (ColorSelect *, gint, gint, gint);

static void color_select_drag_new_color    (GtkWidget *,
					    guchar *, guchar *, guchar *,
					    gpointer);
static void color_select_drop_new_color    (GtkWidget *,
					    guchar, guchar, guchar,
					    gpointer);
static void color_select_drag_old_color    (GtkWidget *,
					    guchar *, guchar *, guchar *,
					    gpointer);

static void color_select_update            (ColorSelect *,
					    ColorSelectUpdateType);
static void color_select_update_caller     (ColorSelect *);
static void color_select_update_values     (ColorSelect *);
static void color_select_update_rgb_values (ColorSelect *);
static void color_select_update_hsv_values (ColorSelect *);
static void color_select_update_pos        (ColorSelect *);
static void color_select_update_sliders    (ColorSelect *, gint);
static void color_select_update_entries    (ColorSelect *, gint);
static void color_select_update_colors     (ColorSelect *, gint);

static gint color_select_xy_expose       (GtkWidget *, GdkEventExpose *,
					  ColorSelect *);
static gint color_select_xy_events       (GtkWidget *, GdkEvent *,
					  ColorSelect *);
static gint color_select_z_expose        (GtkWidget *, GdkEventExpose *,
					  ColorSelect *);
static gint color_select_z_events        (GtkWidget *, GdkEvent *,
					  ColorSelect *);
static gint color_select_color_events    (GtkWidget *, GdkEvent *);
static void color_select_slider_update   (GtkAdjustment *, gpointer);
static void color_select_entry_update    (GtkWidget *, gpointer);
static void color_select_toggle_update   (GtkWidget *, gpointer);
static gint color_select_hex_entry_leave (GtkWidget *, GdkEvent *, gpointer);

static void color_select_image_fill      (GtkWidget *, ColorSelectFillType,
					  gint *);

static void color_select_draw_z_marker   (ColorSelect *, GdkRectangle *);
static void color_select_draw_xy_marker  (ColorSelect *, GdkRectangle *);

static void color_select_update_red              (ColorSelectFill *);
static void color_select_update_green            (ColorSelectFill *);
static void color_select_update_blue             (ColorSelectFill *);
static void color_select_update_hue              (ColorSelectFill *);
static void color_select_update_saturation       (ColorSelectFill *);
static void color_select_update_value            (ColorSelectFill *);
static void color_select_update_red_green        (ColorSelectFill *);
static void color_select_update_red_blue         (ColorSelectFill *);
static void color_select_update_green_blue       (ColorSelectFill *);
static void color_select_update_hue_saturation   (ColorSelectFill *);
static void color_select_update_hue_value        (ColorSelectFill *);
static void color_select_update_saturation_value (ColorSelectFill *);

static GtkWidget * color_select_notebook_new      (gint, gint, gint,
						   GimpColorSelector_Callback,
						   void *, void **);
static void        color_select_notebook_free     (void *);
static void        color_select_notebook_setcolor (void *, gint, gint, gint,
						   gboolean);
static void        color_select_notebook_update_callback (gint, gint, gint,
						          ColorSelectState,
							  void *);

/*  Static variables  */
static ColorSelectFillUpdateProc update_procs[] =
{
  color_select_update_hue,
  color_select_update_saturation,
  color_select_update_value,
  color_select_update_red,
  color_select_update_green,
  color_select_update_blue,
  color_select_update_hue_saturation,
  color_select_update_hue_value,
  color_select_update_saturation_value,
  color_select_update_red_green,
  color_select_update_red_blue,
  color_select_update_green_blue,
};

/*  dnd stuff  */
static GtkTargetEntry color_select_target_table[] =
{
  GIMP_TARGET_COLOR
};
static guint n_color_select_targets = (sizeof (color_select_target_table) /
				       sizeof (color_select_target_table[0]));


/*  Register the GIMP colour selector with the color notebook  */
void
color_select_init (void)
{
  GimpColorSelectorMethods methods =
  {
    color_select_notebook_new,
    color_select_notebook_free,
    color_select_notebook_setcolor
  };

  gimp_color_selector_register ("GIMP", "built_in.html", &methods);
}


static GtkWidget *
color_select_widget_new (ColorSelect *csp,
			 gint         r,
			 gint         g,
			 gint         b)
{
  static gchar  *toggle_titles[6] = { "H", "S", "V", "R", "G", "B" };
  static gfloat  slider_max_vals[6] = { 360, 100, 100, 255, 255, 255 };
  static gfloat  slider_incs[6] = { 0.1, 0.1, 0.1, 1.0, 1.0, 1.0 };

  GtkWidget *main_vbox;
  GtkWidget *main_hbox;
  GtkWidget *xy_frame;
  GtkWidget *z_frame;
  GtkWidget *colors_frame;
  GtkWidget *colors_hbox;
  GtkWidget *right_vbox;
  GtkWidget *table;
  GtkWidget *slider;
  GtkWidget *hex_hbox;
  GtkWidget *label;
  GSList *group;
  gchar buffer[16];
  gint i;

  main_vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 2);

  main_hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 0);
  gtk_box_pack_start (GTK_BOX (main_vbox), main_hbox, TRUE, TRUE, 2);
  gtk_widget_show (main_hbox);

  /*  The x/y component preview  */
  xy_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (xy_frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (main_hbox), xy_frame, FALSE, FALSE, 2);
  gtk_widget_show (xy_frame);

  csp->xy_color = gtk_preview_new (GTK_PREVIEW_COLOR);
  gtk_preview_set_dither (GTK_PREVIEW (csp->xy_color), GDK_RGB_DITHER_MAX);
  gtk_preview_size (GTK_PREVIEW (csp->xy_color), XY_DEF_WIDTH, XY_DEF_HEIGHT);
  gtk_widget_set_events (csp->xy_color, COLOR_AREA_MASK);
  gtk_signal_connect_after (GTK_OBJECT (csp->xy_color), "expose_event",
			    (GtkSignalFunc) color_select_xy_expose,
			    csp);
  gtk_signal_connect (GTK_OBJECT (csp->xy_color), "event",
		      (GtkSignalFunc) color_select_xy_events,
		      csp);
  gtk_container_add (GTK_CONTAINER (xy_frame), csp->xy_color);
  gtk_widget_show (csp->xy_color);

  /*  dnd stuff  */
  gtk_drag_dest_set (csp->xy_color,
                     GTK_DEST_DEFAULT_HIGHLIGHT |
                     GTK_DEST_DEFAULT_MOTION |
                     GTK_DEST_DEFAULT_DROP,
                     color_select_target_table, n_color_select_targets,
                     GDK_ACTION_COPY);
  gimp_dnd_color_dest_set (csp->xy_color, color_select_drop_new_color, csp);

  /*  The z component preview  */
  z_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (z_frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (main_hbox), z_frame, FALSE, FALSE, 2);
  gtk_widget_show (z_frame);

  csp->z_color = gtk_preview_new (GTK_PREVIEW_COLOR);
  gtk_preview_set_dither (GTK_PREVIEW (csp->z_color), GDK_RGB_DITHER_MAX);
  gtk_preview_size (GTK_PREVIEW (csp->z_color), Z_DEF_WIDTH, Z_DEF_HEIGHT);
  gtk_widget_set_events (csp->z_color, COLOR_AREA_MASK);
  gtk_signal_connect_after (GTK_OBJECT (csp->z_color), "expose_event",
			    (GtkSignalFunc) color_select_z_expose,
			    csp);
  gtk_signal_connect (GTK_OBJECT (csp->z_color), "event",
		      (GtkSignalFunc) color_select_z_events,
		      csp);
  gtk_container_add (GTK_CONTAINER (z_frame), csp->z_color);
  gtk_widget_show (csp->z_color);

  /*  The right vertical box with old/new color area and color space sliders  */
  right_vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_set_border_width (GTK_CONTAINER (right_vbox), 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), right_vbox, TRUE, TRUE, 0);
  gtk_widget_show (right_vbox);

  /*  The old/new color area frame and hbox  */
  colors_frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (colors_frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (right_vbox), colors_frame, FALSE, FALSE, 0);
  gtk_widget_show (colors_frame);

  colors_hbox = gtk_hbox_new (TRUE, 2);
  gtk_container_add (GTK_CONTAINER (colors_frame), colors_hbox);
  gtk_widget_show (colors_hbox);

  /*  The new color area  */
  csp->new_color = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (csp->new_color),
			 COLOR_AREA_WIDTH, COLOR_AREA_HEIGHT);
  gtk_widget_set_events (csp->new_color, GDK_EXPOSURE_MASK);
  gtk_signal_connect (GTK_OBJECT (csp->new_color), "event",
		      (GtkSignalFunc) color_select_color_events,
		      csp);
  gtk_object_set_user_data (GTK_OBJECT (csp->new_color), csp);
  gtk_box_pack_start (GTK_BOX (colors_hbox), csp->new_color, TRUE, TRUE, 0);
  gtk_widget_show (csp->new_color);

  /*  dnd stuff  */
  gtk_drag_source_set (csp->new_color,
                       GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
                       color_select_target_table, n_color_select_targets,
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);
  gimp_dnd_color_source_set (csp->new_color, color_select_drag_new_color, csp);

  gtk_drag_dest_set (csp->new_color,
                     GTK_DEST_DEFAULT_HIGHLIGHT |
                     GTK_DEST_DEFAULT_MOTION |
                     GTK_DEST_DEFAULT_DROP,
                     color_select_target_table, n_color_select_targets,
                     GDK_ACTION_COPY);
  gimp_dnd_color_dest_set (csp->new_color, color_select_drop_new_color, csp);

  /*  The old color area  */
  csp->orig_color = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (csp->orig_color),
			 COLOR_AREA_WIDTH, COLOR_AREA_HEIGHT);
  gtk_widget_set_events (csp->orig_color, GDK_EXPOSURE_MASK);
  gtk_signal_connect (GTK_OBJECT (csp->orig_color), "event",
		      (GtkSignalFunc) color_select_color_events,
		      csp);
  gtk_object_set_user_data (GTK_OBJECT (csp->orig_color), csp);
  gtk_box_pack_start (GTK_BOX (colors_hbox), csp->orig_color, TRUE, TRUE, 0);
  gtk_widget_show (csp->orig_color);

  /*  dnd stuff  */
  gtk_drag_source_set (csp->orig_color,
                       GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
                       color_select_target_table, n_color_select_targets,
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);
  gimp_dnd_color_source_set (csp->orig_color, color_select_drag_old_color, csp);

  /*  The color space sliders, toggle buttons and entries  */
  table = gtk_table_new (6, 3, FALSE);
  gtk_table_set_row_spacings (GTK_TABLE (table), 3);
  gtk_table_set_col_spacings (GTK_TABLE (table), 3);
  gtk_container_set_border_width (GTK_CONTAINER (table), 2);
  gtk_box_pack_start (GTK_BOX (right_vbox), table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  group = NULL;
  for (i = 0; i < 6; i++)
    {
      csp->toggles[i] =
	gtk_radio_button_new_with_label (group, toggle_titles[i]);
      group = gtk_radio_button_group (GTK_RADIO_BUTTON (csp->toggles[i]));
      gtk_table_attach (GTK_TABLE (table), csp->toggles[i],
			0, 1, i, i+1, GTK_FILL, GTK_EXPAND, 0, 0);
      gtk_signal_connect (GTK_OBJECT (csp->toggles[i]), "toggled",
			  (GtkSignalFunc) color_select_toggle_update,
			  csp);
      gtk_widget_show (csp->toggles[i]);

      csp->slider_data[i] =
	GTK_ADJUSTMENT (gtk_adjustment_new (csp->values[i], 0.0,
					    slider_max_vals[i],
					    slider_incs[i],
					    1.0, 0.0));

      slider = gtk_hscale_new (csp->slider_data[i]);
      gtk_table_attach (GTK_TABLE (table), slider, 1, 2, i, i+1,
			GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
      gtk_scale_set_value_pos (GTK_SCALE (slider), GTK_POS_TOP);
      gtk_scale_set_draw_value (GTK_SCALE (slider), FALSE);
      gtk_signal_connect (GTK_OBJECT (csp->slider_data[i]), "value_changed",
			  (GtkSignalFunc) color_select_slider_update,
			  csp);
      gtk_widget_show (slider);

      csp->entries[i] = gtk_entry_new ();
      g_snprintf (buffer, sizeof (buffer), "%d", csp->values[i]);
      gtk_entry_set_text (GTK_ENTRY (csp->entries[i]), buffer);
      gtk_widget_set_usize (GTK_WIDGET (csp->entries[i]), 40, 0);
      gtk_table_attach (GTK_TABLE (table), csp->entries[i],
			2, 3, i, i+1, GTK_FILL, GTK_EXPAND, 0, 0);
      gtk_signal_connect (GTK_OBJECT (csp->entries[i]), "changed",
			  (GtkSignalFunc) color_select_entry_update,
			  csp);
      gtk_widget_show (csp->entries[i]);
    }

  /* The hex triplet entry */
  hex_hbox = gtk_hbox_new (FALSE, 3);
  gtk_box_pack_start (GTK_BOX (right_vbox), hex_hbox, FALSE, FALSE, 0);
  gtk_widget_show (hex_hbox);

  csp->hex_entry = gtk_entry_new ();
  g_snprintf (buffer, sizeof (buffer), "#%.2x%.2x%.2x", r, g, b);
  gtk_entry_set_text (GTK_ENTRY (csp->hex_entry), buffer);
  gtk_widget_set_usize (GTK_WIDGET (csp->hex_entry), 75, 0);
  gtk_box_pack_end (GTK_BOX (hex_hbox), csp->hex_entry, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (csp->hex_entry), "focus_out_event",
		      (GtkSignalFunc) color_select_hex_entry_leave,
		      csp);
  gtk_widget_show (csp->hex_entry);

  label = gtk_label_new (_("Hex Triplet:"));
  gtk_box_pack_end (GTK_BOX (hex_hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  return main_vbox;
}

static void
color_select_drag_new_color (GtkWidget *widget,
			     guchar    *r,
			     guchar    *g,
			     guchar    *b,
			     gpointer   data)
{
  ColorSelect *csp;

  csp = (ColorSelect *) data;

  *r = (guchar) csp->values[COLOR_SELECT_RED];
  *g = (guchar) csp->values[COLOR_SELECT_GREEN];
  *b = (guchar) csp->values[COLOR_SELECT_BLUE];
}

static void
color_select_drop_new_color (GtkWidget *widget,
			     guchar     r,
			     guchar     g,
			     guchar     b,
			     gpointer   data)
{
  ColorSelect *csp;

  csp = (ColorSelect *) data;

  csp->values[COLOR_SELECT_RED]   = (gint) r;
  csp->values[COLOR_SELECT_GREEN] = (gint) g;
  csp->values[COLOR_SELECT_BLUE]  = (gint) b;

  color_select_update_hsv_values (csp);
  color_select_update_pos (csp);
  color_select_update_sliders (csp, -1);
  color_select_update_entries (csp, -1);
  color_select_update_colors (csp, 0);

  color_select_update (csp, UPDATE_Z_COLOR);
  color_select_update (csp, UPDATE_XY_COLOR);
}

static void
color_select_drag_old_color (GtkWidget *widget,
			     guchar    *r,
			     guchar    *g,
			     guchar    *b,
			     gpointer   data)
{
  ColorSelect *csp;

  csp = (ColorSelect *) data;

  *r = (guchar) csp->orig_values[0];
  *g = (guchar) csp->orig_values[1];
  *b = (guchar) csp->orig_values[2];
}

static void
color_select_set_color (ColorSelect *csp,
			gint         r,
			gint         g,
			gint         b,
			gboolean     set_current)
{
  if (csp)
    {
      csp->orig_values[0] = r;
      csp->orig_values[1] = g;
      csp->orig_values[2] = b;

      color_select_update_colors (csp, 1);

      if (set_current)
	{
	  csp->values[COLOR_SELECT_RED] = r;
	  csp->values[COLOR_SELECT_GREEN] = g;
	  csp->values[COLOR_SELECT_BLUE] = b;

	  color_select_update_hsv_values (csp);
	  color_select_update_pos (csp);
	  color_select_update_sliders (csp, -1);
	  color_select_update_entries (csp, -1);
	  color_select_update_colors (csp, 0);

	  color_select_update (csp, UPDATE_Z_COLOR);
	  color_select_update (csp, UPDATE_XY_COLOR);
	}
    }
}

static void
color_select_update (ColorSelect           *csp,
		     ColorSelectUpdateType  update)
{
  if (csp)
    {
      if (update & UPDATE_POS)
	color_select_update_pos (csp);

      if (update & UPDATE_VALUES)
	{
	  color_select_update_values (csp);
	  color_select_update_sliders (csp, -1);
	  color_select_update_entries (csp, -1);

	  if (!(update & UPDATE_NEW_COLOR))
	    color_select_update_colors (csp, 0);
	}

      if (update & UPDATE_XY_COLOR)
	{
	  color_select_image_fill (csp->xy_color, csp->xy_color_fill, csp->values);
	  gtk_widget_draw (csp->xy_color, NULL);
	}

      if (update & UPDATE_Z_COLOR)
	{
	  color_select_image_fill (csp->z_color, csp->z_color_fill, csp->values);
	  gtk_widget_draw (csp->z_color, NULL);
	}

      if (update & UPDATE_NEW_COLOR)
	color_select_update_colors (csp, 0);

      if (update & UPDATE_ORIG_COLOR)
	color_select_update_colors (csp, 1);

      /*if (update & UPDATE_CALLER)*/
      color_select_update_caller (csp);
    }
}

static void
color_select_update_caller (ColorSelect *csp)
{
  if (csp && csp->wants_updates && csp->callback)
    {
      (* csp->callback) (csp->values[COLOR_SELECT_RED],
			 csp->values[COLOR_SELECT_GREEN],
			 csp->values[COLOR_SELECT_BLUE],
			 COLOR_SELECT_UPDATE,
			 csp->client_data);
    }
}

static void
color_select_update_values (ColorSelect *csp)
{
  if (csp)
    {
      switch (csp->z_color_fill)
	{
	case COLOR_SELECT_RED:
	  csp->values[COLOR_SELECT_BLUE] = csp->pos[0];
	  csp->values[COLOR_SELECT_GREEN] = csp->pos[1];
	  csp->values[COLOR_SELECT_RED] = csp->pos[2];
	  break;
	case COLOR_SELECT_GREEN:
	  csp->values[COLOR_SELECT_BLUE] = csp->pos[0];
	  csp->values[COLOR_SELECT_RED] = csp->pos[1];
	  csp->values[COLOR_SELECT_GREEN] = csp->pos[2];
	  break;
	case COLOR_SELECT_BLUE:
	  csp->values[COLOR_SELECT_GREEN] = csp->pos[0];
	  csp->values[COLOR_SELECT_RED] = csp->pos[1];
	  csp->values[COLOR_SELECT_BLUE] = csp->pos[2];
	  break;
	case COLOR_SELECT_HUE:
	  csp->values[COLOR_SELECT_VALUE] = csp->pos[0] * 100 / 255;
	  csp->values[COLOR_SELECT_SATURATION] = csp->pos[1] * 100 / 255;
	  csp->values[COLOR_SELECT_HUE] = csp->pos[2] * 360 / 255;
	  break;
	case COLOR_SELECT_SATURATION:
	  csp->values[COLOR_SELECT_VALUE] = csp->pos[0] * 100 / 255;
	  csp->values[COLOR_SELECT_HUE] = csp->pos[1] * 360 / 255;
	  csp->values[COLOR_SELECT_SATURATION] = csp->pos[2] * 100 / 255;
	  break;
	case COLOR_SELECT_VALUE:
	  csp->values[COLOR_SELECT_SATURATION] = csp->pos[0] * 100 / 255;
	  csp->values[COLOR_SELECT_HUE] = csp->pos[1] * 360 / 255;
	  csp->values[COLOR_SELECT_VALUE] = csp->pos[2] * 100 / 255;
	  break;
	}

      switch (csp->z_color_fill)
	{
	case COLOR_SELECT_RED:
	case COLOR_SELECT_GREEN:
	case COLOR_SELECT_BLUE:
	  color_select_update_hsv_values (csp);
	  break;
	case COLOR_SELECT_HUE:
	case COLOR_SELECT_SATURATION:
	case COLOR_SELECT_VALUE:
	  color_select_update_rgb_values (csp);
	  break;
	}
    }
}

static void
color_select_update_rgb_values (ColorSelect *csp)
{
  gfloat h, s, v;
  gfloat f, p, q, t;

  if (csp)
    {
      h = csp->values[COLOR_SELECT_HUE];
      s = csp->values[COLOR_SELECT_SATURATION] / 100.0;
      v = csp->values[COLOR_SELECT_VALUE] / 100.0;

      if (s == 0)
	{
	  csp->values[COLOR_SELECT_RED] = v * 255;
	  csp->values[COLOR_SELECT_GREEN] = v * 255;
	  csp->values[COLOR_SELECT_BLUE] = v * 255;
	}
      else
	{
	  if (h == 360)
	    h = 0;

	  h /= 60;
	  f = h - (int) h;
	  p = v * (1 - s);
	  q = v * (1 - (s * f));
	  t = v * (1 - (s * (1 - f)));

	  switch ((int) h)
	    {
	    case 0:
	      csp->values[COLOR_SELECT_RED] = v * 255;
	      csp->values[COLOR_SELECT_GREEN] = t * 255;
	      csp->values[COLOR_SELECT_BLUE] = p * 255;
	      break;
	    case 1:
	      csp->values[COLOR_SELECT_RED] = q * 255;
	      csp->values[COLOR_SELECT_GREEN] = v * 255;
	      csp->values[COLOR_SELECT_BLUE] = p * 255;
	      break;
	    case 2:
	      csp->values[COLOR_SELECT_RED] = p * 255;
	      csp->values[COLOR_SELECT_GREEN] = v * 255;
	      csp->values[COLOR_SELECT_BLUE] = t * 255;
	      break;
	    case 3:
	      csp->values[COLOR_SELECT_RED] = p * 255;
	      csp->values[COLOR_SELECT_GREEN] = q * 255;
	      csp->values[COLOR_SELECT_BLUE] = v * 255;
	      break;
	    case 4:
	      csp->values[COLOR_SELECT_RED] = t * 255;
	      csp->values[COLOR_SELECT_GREEN] = p * 255;
	      csp->values[COLOR_SELECT_BLUE] = v * 255;
	      break;
	    case 5:
	      csp->values[COLOR_SELECT_RED] = v * 255;
	      csp->values[COLOR_SELECT_GREEN] = p * 255;
	      csp->values[COLOR_SELECT_BLUE] = q * 255;
	      break;
	    }
	}
    }
}

static void
color_select_update_hsv_values (ColorSelect *csp)
{
  gint r, g, b;
  gfloat h, s, v;
  gint min, max;
  gint delta;

  if (csp)
    {
      r = csp->values[COLOR_SELECT_RED];
      g = csp->values[COLOR_SELECT_GREEN];
      b = csp->values[COLOR_SELECT_BLUE];

      if (r > g)
	{
	  if (r > b)
	    max = r;
	  else
	    max = b;

	  if (g < b)
	    min = g;
	  else
	    min = b;
	}
      else
	{
	  if (g > b)
	    max = g;
	  else
	    max = b;

	  if (r < b)
	    min = r;
	  else
	    min = b;
	}

      v = max;

      if (max != 0)
	s = (max - min) / (float) max;
      else
	s = 0;

      if (s == 0)
	h = 0;
      else
	{
	  h = 0;
	  delta = max - min;
	  if (r == max)
	    h = (g - b) / (float) delta;
	  else if (g == max)
	    h = 2 + (b - r) / (float) delta;
	  else if (b == max)
	    h = 4 + (r - g) / (float) delta;
	  h *= 60;

	  if (h < 0)
	    h += 360;
	}

      csp->values[COLOR_SELECT_HUE] = h;
      csp->values[COLOR_SELECT_SATURATION] = s * 100;
      csp->values[COLOR_SELECT_VALUE] = v * 100 / 255;
    }
}

static void
color_select_update_pos (ColorSelect *csp)
{
  if (csp)
    {
      switch (csp->z_color_fill)
	{
	case COLOR_SELECT_RED:
	  csp->pos[0] = csp->values[COLOR_SELECT_BLUE];
	  csp->pos[1] = csp->values[COLOR_SELECT_GREEN];
	  csp->pos[2] = csp->values[COLOR_SELECT_RED];
	  break;
	case COLOR_SELECT_GREEN:
	  csp->pos[0] = csp->values[COLOR_SELECT_BLUE];
	  csp->pos[1] = csp->values[COLOR_SELECT_RED];
	  csp->pos[2] = csp->values[COLOR_SELECT_GREEN];
	  break;
	case COLOR_SELECT_BLUE:
	  csp->pos[0] = csp->values[COLOR_SELECT_GREEN];
	  csp->pos[1] = csp->values[COLOR_SELECT_RED];
	  csp->pos[2] = csp->values[COLOR_SELECT_BLUE];
	  break;
	case COLOR_SELECT_HUE:
	  csp->pos[0] = csp->values[COLOR_SELECT_VALUE] * 255 / 100;
	  csp->pos[1] = csp->values[COLOR_SELECT_SATURATION] * 255 / 100;
	  csp->pos[2] = csp->values[COLOR_SELECT_HUE] * 255 / 360;
	  break;
	case COLOR_SELECT_SATURATION:
	  csp->pos[0] = csp->values[COLOR_SELECT_VALUE] * 255 / 100;
	  csp->pos[1] = csp->values[COLOR_SELECT_HUE] * 255 / 360;
	  csp->pos[2] = csp->values[COLOR_SELECT_SATURATION] * 255 / 100;
	  break;
	case COLOR_SELECT_VALUE:
	  csp->pos[0] = csp->values[COLOR_SELECT_SATURATION] * 255 / 100;
	  csp->pos[1] = csp->values[COLOR_SELECT_HUE] * 255 / 360;
	  csp->pos[2] = csp->values[COLOR_SELECT_VALUE] * 255 / 100;
	  break;
	}
    }
}

static void
color_select_update_sliders (ColorSelect *csp,
			     gint         skip)
{
  gint i;

  if (csp)
    {
      for (i = 0; i < 6; i++)
	if (i != skip)
	  {
	    csp->slider_data[i]->value = (gfloat) csp->values[i];

	    gtk_signal_handler_block_by_data (GTK_OBJECT (csp->slider_data[i]), csp);
	    gtk_signal_emit_by_name (GTK_OBJECT (csp->slider_data[i]), "value_changed");
	    gtk_signal_handler_unblock_by_data (GTK_OBJECT (csp->slider_data[i]), csp);
	  }
    }
}

static void
color_select_update_entries (ColorSelect *csp,
			     gint         skip)
{
  gchar buffer[16];
  gint i;

  if (csp)
    {
      for (i = 0; i < 6; i++)
	if (i != skip)
	  {
	    g_snprintf (buffer, sizeof (buffer), "%d", csp->values[i]);

	    gtk_signal_handler_block_by_data (GTK_OBJECT (csp->entries[i]), csp);
	    gtk_entry_set_text (GTK_ENTRY (csp->entries[i]), buffer);
	    gtk_signal_handler_unblock_by_data (GTK_OBJECT (csp->entries[i]), csp);
	  }

      g_snprintf (buffer, sizeof (buffer), "#%.2x%.2x%.2x",
		  csp->values[COLOR_SELECT_RED],
		  csp->values[COLOR_SELECT_GREEN],
		  csp->values[COLOR_SELECT_BLUE]);
      gtk_entry_set_text (GTK_ENTRY (csp->hex_entry), buffer);
    }
}

static void
color_select_update_colors (ColorSelect *csp,
			    gint         which)
{
  GdkWindow *window;
  GdkColor color;
  gint red, green, blue;
  gint width, height;

  if (csp)
    {
      if (which)
	{
	  window = csp->orig_color->window;
	  red   = csp->orig_values[0];
	  green = csp->orig_values[1];
	  blue  = csp->orig_values[2];
	}
      else
	{
	  window = csp->new_color->window;
	  red   = csp->values[COLOR_SELECT_RED];
	  green = csp->values[COLOR_SELECT_GREEN];
	  blue  = csp->values[COLOR_SELECT_BLUE];
	}

      /* if we haven't yet been realised, there's no need to redraw
       * anything. */
      if (!window)
	return;

      color.pixel = get_color (red, green, blue);

      gdk_window_get_size (window, &width, &height);

      if (csp->gc)
	{
#ifdef OLD_COLOR_AREA
	  gdk_gc_set_foreground (csp->gc, &color);
	  gdk_draw_rectangle (window, csp->gc, 1,
			      0, 0, width, height);
#else
	  color_area_draw_rect (window, csp->gc,
				0, 0, width, height,
				red, green, blue);
#endif
	}
    }
}

static gint
color_select_xy_expose (GtkWidget      *widget,
			GdkEventExpose *event,
			ColorSelect    *csp)
{
  if (!csp->gc)
    csp->gc = gdk_gc_new (widget->window);

  color_select_draw_xy_marker (csp, &event->area);

  return FALSE;
}

static gint
color_select_xy_events (GtkWidget   *widget,
			GdkEvent    *event,
			ColorSelect *csp)
{
  GdkEventButton *bevent;
  GdkEventMotion *mevent;
  gint tx, ty;

  switch (event->type)
    {
    case GDK_BUTTON_PRESS:
      bevent = (GdkEventButton *) event;

      color_select_draw_xy_marker (csp, NULL);

      csp->pos[0] = (bevent->x * 255) / (XY_DEF_WIDTH - 1);
      csp->pos[1] = 255 - (bevent->y * 255) / (XY_DEF_HEIGHT - 1);

      if (csp->pos[0] < 0)
	csp->pos[0] = 0;
      if (csp->pos[0] > 255)
	csp->pos[0] = 255;
      if (csp->pos[1] < 0)
	csp->pos[1] = 0;
      if (csp->pos[1] > 255)
	csp->pos[1] = 255;

      gdk_pointer_grab (csp->xy_color->window, FALSE,
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_BUTTON1_MOTION_MASK |
			GDK_BUTTON_RELEASE_MASK,
			NULL, NULL, bevent->time);
      color_select_draw_xy_marker (csp, NULL);

      color_select_update (csp, UPDATE_VALUES);
      break;

    case GDK_BUTTON_RELEASE:
      bevent = (GdkEventButton *) event;

      color_select_draw_xy_marker (csp, NULL);

      csp->pos[0] = (bevent->x * 255) / (XY_DEF_WIDTH - 1);
      csp->pos[1] = 255 - (bevent->y * 255) / (XY_DEF_HEIGHT - 1);

      if (csp->pos[0] < 0)
	csp->pos[0] = 0;
      if (csp->pos[0] > 255)
	csp->pos[0] = 255;
      if (csp->pos[1] < 0)
	csp->pos[1] = 0;
      if (csp->pos[1] > 255)
	csp->pos[1] = 255;

      gdk_pointer_ungrab (bevent->time);
      color_select_draw_xy_marker (csp, NULL);
      color_select_update (csp, UPDATE_VALUES);
      break;

    case GDK_MOTION_NOTIFY:
      mevent = (GdkEventMotion *) event;
      if (mevent->is_hint)
	{
	  gdk_window_get_pointer (widget->window, &tx, &ty, NULL);
	  mevent->x = tx;
	  mevent->y = ty;
	}

      color_select_draw_xy_marker (csp, NULL);

      csp->pos[0] = (mevent->x * 255) / (XY_DEF_WIDTH - 1);
      csp->pos[1] = 255 - (mevent->y * 255) / (XY_DEF_HEIGHT - 1);

      if (csp->pos[0] < 0)
	csp->pos[0] = 0;
      if (csp->pos[0] > 255)
	csp->pos[0] = 255;
      if (csp->pos[1] < 0)
	csp->pos[1] = 0;
      if (csp->pos[1] > 255)
	csp->pos[1] = 255;

      color_select_draw_xy_marker (csp, NULL);
      color_select_update (csp, UPDATE_VALUES);
      break;

    default:
      break;
    }

  return FALSE;
}

static gint
color_select_z_expose (GtkWidget      *widget,
		       GdkEventExpose *event,
		       ColorSelect    *csp)
{
  if (!csp->gc)
    csp->gc = gdk_gc_new (widget->window);

  color_select_draw_z_marker (csp, &event->area);

  return FALSE;
}

static gint
color_select_z_events (GtkWidget   *widget,
		       GdkEvent    *event,
		       ColorSelect *csp)
{
  GdkEventButton *bevent;
  GdkEventMotion *mevent;
  gint tx, ty;

  switch (event->type)
    {
    case GDK_BUTTON_PRESS:
      bevent = (GdkEventButton *) event;

      color_select_draw_z_marker (csp, NULL);

      csp->pos[2] = 255 - (bevent->y * 255) / (Z_DEF_HEIGHT - 1);
      if (csp->pos[2] < 0)
	csp->pos[2] = 0;
      if (csp->pos[2] > 255)
	csp->pos[2] = 255;

      gdk_pointer_grab (csp->z_color->window, FALSE,
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_BUTTON1_MOTION_MASK |
			GDK_BUTTON_RELEASE_MASK,
			NULL, NULL, bevent->time);
      color_select_draw_z_marker (csp, NULL);
      color_select_update (csp, UPDATE_VALUES);
      break;

    case GDK_BUTTON_RELEASE:
      bevent = (GdkEventButton *) event;

      color_select_draw_z_marker (csp, NULL);

      csp->pos[2] = 255 - (bevent->y * 255) / (Z_DEF_HEIGHT - 1);
      if (csp->pos[2] < 0)
	csp->pos[2] = 0;
      if (csp->pos[2] > 255)
	csp->pos[2] = 255;

      gdk_pointer_ungrab (bevent->time);
      color_select_draw_z_marker (csp, NULL);
      color_select_update (csp, UPDATE_VALUES | UPDATE_XY_COLOR);
      break;

    case GDK_MOTION_NOTIFY:
      mevent = (GdkEventMotion *) event;
      if (mevent->is_hint)
	{
	  gdk_window_get_pointer (widget->window, &tx, &ty, NULL);
	  mevent->x = tx;
	  mevent->y = ty;
	}

      color_select_draw_z_marker (csp, NULL);

      csp->pos[2] = 255 - (mevent->y * 255) / (Z_DEF_HEIGHT - 1);
      if (csp->pos[2] < 0)
	csp->pos[2] = 0;
      if (csp->pos[2] > 255)
	csp->pos[2] = 255;

      color_select_draw_z_marker (csp, NULL);
      color_select_update (csp, UPDATE_VALUES);
      break;

    default:
      break;
    }

  return FALSE;
}

static gint
color_select_color_events (GtkWidget *widget,
			   GdkEvent  *event)
{
  ColorSelect *csp;

  csp = (ColorSelect *) gtk_object_get_user_data (GTK_OBJECT (widget));

  if (!csp)
    return FALSE;

  switch (event->type)
    {
    case GDK_EXPOSE:
      if (!csp->gc)
	csp->gc = gdk_gc_new (widget->window);

      if (widget == csp->new_color)
	color_select_update (csp, UPDATE_NEW_COLOR);
      else if (widget == csp->orig_color)
	color_select_update (csp, UPDATE_ORIG_COLOR);
      break;

    default:
      break;
    }

  return FALSE;
}

static void
color_select_slider_update (GtkAdjustment *adjustment,
			    gpointer       data)
{
  ColorSelect *csp;
  gint old_values[6];
  gint update_z_marker;
  gint update_xy_marker;
  gint i, j;

  csp = (ColorSelect *) data;

  if (csp)
    {
      for (i = 0; i < 6; i++)
	if (csp->slider_data[i] == adjustment)
	  break;

      for (j = 0; j < 6; j++)
	old_values[j] = csp->values[j];

      csp->values[i] = (int) adjustment->value;

      if ((i >= COLOR_SELECT_HUE) && (i <= COLOR_SELECT_VALUE))
	color_select_update_rgb_values (csp);
      else if ((i >= COLOR_SELECT_RED) && (i <= COLOR_SELECT_BLUE))
	color_select_update_hsv_values (csp);
      color_select_update_sliders (csp, i);
      color_select_update_entries (csp, -1);

      update_z_marker = 0;
      update_xy_marker = 0;
      for (j = 0; j < 6; j++)
	{
	  if (j == csp->z_color_fill)
	    {
	      if (old_values[j] != csp->values[j])
		update_z_marker = 1;
	    }
	  else
	    {
	      if (old_values[j] != csp->values[j])
		update_xy_marker = 1;
	    }
	}

      if (update_z_marker)
	{
	  color_select_draw_z_marker (csp, NULL);
	  color_select_update (csp, UPDATE_POS | UPDATE_XY_COLOR);
	  color_select_draw_z_marker (csp, NULL);
	}
      else
	{
	  if (update_z_marker)
	    color_select_draw_z_marker (csp, NULL);
	  if (update_xy_marker)
	    color_select_draw_xy_marker (csp, NULL);

	  color_select_update (csp, UPDATE_POS);

	  if (update_z_marker)
	    color_select_draw_z_marker (csp, NULL);
	  if (update_xy_marker)
	    color_select_draw_xy_marker (csp, NULL);
	}

      color_select_update (csp, UPDATE_NEW_COLOR);
    }
}

static void
color_select_entry_update (GtkWidget *widget,
			   gpointer   data)
{
  ColorSelect *csp;
  int old_values[6];
  int update_z_marker;
  int update_xy_marker;
  gint i, j;

  csp = (ColorSelect *) data;

  if (csp)
    {
      for (i = 0; i < 6; i++)
	if (csp->entries[i] == widget)
	  break;

      for (j = 0; j < 6; j++)
	old_values[j] = csp->values[j];

      csp->values[i] = atoi (gtk_entry_get_text (GTK_ENTRY (csp->entries[i])));
      if (csp->values[i] == old_values[i])
	return;

      if ((i >= COLOR_SELECT_HUE) && (i <= COLOR_SELECT_VALUE))
	color_select_update_rgb_values (csp);
      else if ((i >= COLOR_SELECT_RED) && (i <= COLOR_SELECT_BLUE))
	color_select_update_hsv_values (csp);
      color_select_update_entries (csp, i);
      color_select_update_sliders (csp, -1);

      update_z_marker = 0;
      update_xy_marker = 0;
      for (j = 0; j < 6; j++)
	{
	  if (j == csp->z_color_fill)
	    {
	      if (old_values[j] != csp->values[j])
		update_z_marker = 1;
	    }
	  else
	    {
	      if (old_values[j] != csp->values[j])
		update_xy_marker = 1;
	    }
	}

      if (update_z_marker)
	{
	  color_select_draw_z_marker (csp, NULL);
	  color_select_update (csp, UPDATE_POS | UPDATE_XY_COLOR);
	  color_select_draw_z_marker (csp, NULL);
	}
      else
	{
	  if (update_z_marker)
	    color_select_draw_z_marker (csp, NULL);
	  if (update_xy_marker)
	    color_select_draw_xy_marker (csp, NULL);

	  color_select_update (csp, UPDATE_POS);

	  if (update_z_marker)
	    color_select_draw_z_marker (csp, NULL);
	  if (update_xy_marker)
	    color_select_draw_xy_marker (csp, NULL);
	}

      color_select_update (csp, UPDATE_NEW_COLOR);
    }
}

static void
color_select_toggle_update (GtkWidget *widget,
			    gpointer   data)
{
  ColorSelect *csp;
  ColorSelectFillType type = COLOR_SELECT_HUE;
  gint i;

  if (!GTK_TOGGLE_BUTTON (widget)->active)
    return;

  csp = (ColorSelect *) data;

  if (csp)
    {
      for (i = 0; i < 6; i++)
	if (widget == csp->toggles[i])
	  type = (ColorSelectFillType) i;

      switch (type)
	{
	case COLOR_SELECT_HUE:
	  csp->z_color_fill = COLOR_SELECT_HUE;
	  csp->xy_color_fill = COLOR_SELECT_SATURATION_VALUE;
	  break;
	case COLOR_SELECT_SATURATION:
	  csp->z_color_fill = COLOR_SELECT_SATURATION;
	  csp->xy_color_fill = COLOR_SELECT_HUE_VALUE;
	  break;
	case COLOR_SELECT_VALUE:
	  csp->z_color_fill = COLOR_SELECT_VALUE;
	  csp->xy_color_fill = COLOR_SELECT_HUE_SATURATION;
	  break;
	case COLOR_SELECT_RED:
	  csp->z_color_fill = COLOR_SELECT_RED;
	  csp->xy_color_fill = COLOR_SELECT_GREEN_BLUE;
	  break;
	case COLOR_SELECT_GREEN:
	  csp->z_color_fill = COLOR_SELECT_GREEN;
	  csp->xy_color_fill = COLOR_SELECT_RED_BLUE;
	  break;
	case COLOR_SELECT_BLUE:
	  csp->z_color_fill = COLOR_SELECT_BLUE;
	  csp->xy_color_fill = COLOR_SELECT_RED_GREEN;
	  break;
	default:
	  break;
	}

      color_select_update (csp, UPDATE_POS);
      color_select_update (csp, UPDATE_Z_COLOR | UPDATE_XY_COLOR);
    }
}

static gint
color_select_hex_entry_leave (GtkWidget *widget,
			      GdkEvent  *event,
			      gpointer   data)
{
  ColorSelect *csp;
  gchar  buffer[8];
  gchar *hex_color;
  guint  hex_rgb;

  csp = (ColorSelect *) data;

  if (csp)
    {
      hex_color = g_strdup (gtk_entry_get_text (GTK_ENTRY (csp->hex_entry)));

      g_snprintf(buffer, sizeof (buffer), "#%.2x%.2x%.2x",
		 csp->values[COLOR_SELECT_RED],
		 csp->values[COLOR_SELECT_GREEN],
		 csp->values[COLOR_SELECT_BLUE]);

      if ((strlen (hex_color) == 7) &&
	  (g_strcasecmp (buffer, hex_color) != 0))
	{
	  if ((sscanf (hex_color, "#%x", &hex_rgb) == 1) &&
	      (hex_rgb < (1 << 24)))
	    color_select_set_color (csp,
				    (hex_rgb & 0xff0000) >> 16,
				    (hex_rgb & 0x00ff00) >> 8,
				    hex_rgb & 0x0000ff,
				    TRUE);
	}

      g_free (hex_color);
    }

  return FALSE;
}

static void
color_select_image_fill (GtkWidget           *preview,
			 ColorSelectFillType  type,
			 gint                *values)
{
  ColorSelectFill csf;
  gint height;

  csf.buffer = g_malloc (preview->requisition.width * 3);

  csf.update = update_procs[type];

  csf.y = -1;
  csf.width = preview->requisition.width;
  csf.height = preview->requisition.height;
  csf.values = values;

  height = csf.height;
  if (height > 0)
    while (height--)
      {
	(* csf.update) (&csf);
	gtk_preview_draw_row (GTK_PREVIEW (preview),
			      csf.buffer, 0, csf.y, csf.width);
      }

  g_free (csf.buffer);
}

static void
color_select_draw_z_marker (ColorSelect  *csp,
			    GdkRectangle *clip)
{
  gint width;
  gint height;
  gint y;
  gint minx;
  gint miny;

  if (csp->gc)
    {
      y = (Z_DEF_HEIGHT - 1) - ((Z_DEF_HEIGHT - 1) * csp->pos[2]) / 255;
      width = csp->z_color->requisition.width;
      height = csp->z_color->requisition.height;
      minx = 0;
      miny = 0;
      if (width <= 0)
	return;

      if (clip)
        {
	  width  = MIN(width,  clip->x + clip->width);
	  height = MIN(height, clip->y + clip->height);
	  minx   = MAX(0, clip->x);
	  miny   = MAX(0, clip->y);
	}

      if (y >= miny && y < height)
        {
	  gdk_gc_set_function (csp->gc, GDK_INVERT);
	  gdk_draw_line (csp->z_color->window, csp->gc, minx, y, width - 1, y);
	  gdk_gc_set_function (csp->gc, GDK_COPY);
	}
    }
}

static void
color_select_draw_xy_marker (ColorSelect  *csp,
			     GdkRectangle *clip)
{
  gint width;
  gint height;
  gint x, y;
  gint minx, miny;

  if (csp->gc)
    {
      x = ((XY_DEF_WIDTH - 1) * csp->pos[0]) / 255;
      y = (XY_DEF_HEIGHT - 1) - ((XY_DEF_HEIGHT - 1) * csp->pos[1]) / 255;
      width = csp->xy_color->requisition.width;
      height = csp->xy_color->requisition.height;
      minx = 0;
      miny = 0;
      if ((width <= 0) || (height <= 0))
	return;

      gdk_gc_set_function (csp->gc, GDK_INVERT);

      if (clip)
        {
	  width  = MIN(width,  clip->x + clip->width);
	  height = MIN(height, clip->y + clip->height);
	  minx   = MAX(0, clip->x);
	  miny   = MAX(0, clip->y);
	}

      if (y >= miny && y < height)
	gdk_draw_line (csp->xy_color->window, csp->gc, minx, y, width - 1, y);

      if (x >= minx && x < width)
	gdk_draw_line (csp->xy_color->window, csp->gc, x, miny, x, height - 1);

      gdk_gc_set_function (csp->gc, GDK_COPY);
    }
}

static void
color_select_update_red (ColorSelectFill *csf)
{
  guchar *p;
  gint i, r;

  p = csf->buffer;

  csf->y += 1;
  r = (csf->height - csf->y + 1) * 255 / csf->height;

  if (r < 0)
    r = 0;
  if (r > 255)
    r = 255;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = r;
      *p++ = 0;
      *p++ = 0;
    }
}

static void
color_select_update_green (ColorSelectFill *csf)
{
  guchar *p;
  gint i, g;

  p = csf->buffer;

  csf->y += 1;
  g = (csf->height - csf->y + 1) * 255 / csf->height;

  if (g < 0)
    g = 0;
  if (g > 255)
    g = 255;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = 0;
      *p++ = g;
      *p++ = 0;
    }
}

static void
color_select_update_blue (ColorSelectFill *csf)
{
  guchar *p;
  gint i, b;

  p = csf->buffer;

  csf->y += 1;
  b = (csf->height - csf->y + 1) * 255 / csf->height;

  if (b < 0)
    b = 0;
  if (b > 255)
    b = 255;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = 0;
      *p++ = 0;
      *p++ = b;
    }
}

static void
color_select_update_hue (ColorSelectFill *csf)
{
  guchar *p;
  gfloat h, f;
  gint r, g, b;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  h = csf->y * 360 / csf->height;

  h = 360 - h;

  if (h < 0)
    h = 0;
  if (h >= 360)
    h = 0;

  h /= 60;
  f = (h - (int) h) * 255;

  r = g = b = 0;

  switch ((int) h)
    {
    case 0:
      r = 255;
      g = f;
      b = 0;
      break;
    case 1:
      r = 255 - f;
      g = 255;
      b = 0;
      break;
    case 2:
      r = 0;
      g = 255;
      b = f;
      break;
    case 3:
      r = 0;
      g = 255 - f;
      b = 255;
      break;
    case 4:
      r = f;
      g = 0;
      b = 255;
      break;
    case 5:
      r = 255;
      g = 0;
      b = 255 - f;
      break;
    }

  for (i = 0; i < csf->width; i++)
    {
      *p++ = r;
      *p++ = g;
      *p++ = b;
    }
}

static void
color_select_update_saturation (ColorSelectFill *csf)
{
  guchar *p;
  gint s;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  s = csf->y * 255 / csf->height;

  if (s < 0)
    s = 0;
  if (s > 255)
    s = 255;

  s = 255 - s;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = s;
      *p++ = s;
      *p++ = s;
    }
}

static void
color_select_update_value (ColorSelectFill *csf)
{
  guchar *p;
  gint v;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  v = csf->y * 255 / csf->height;

  if (v < 0)
    v = 0;
  if (v > 255)
    v = 255;

  v = 255 - v;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = v;
      *p++ = v;
      *p++ = v;
    }
}

static void
color_select_update_red_green (ColorSelectFill *csf)
{
  guchar *p;
  gint i, r, b;
  gfloat g, dg;

  p = csf->buffer;

  csf->y += 1;
  b = csf->values[COLOR_SELECT_BLUE];
  r = (csf->height - csf->y + 1) * 255 / csf->height;

  if (r < 0)
    r = 0;
  if (r > 255)
    r = 255;

  g = 0;
  dg = 255.0 / csf->width;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = r;
      *p++ = g;
      *p++ = b;

      g += dg;
    }
}

static void
color_select_update_red_blue (ColorSelectFill *csf)
{
  guchar *p;
  gint i, r, g;
  gfloat b, db;

  p = csf->buffer;

  csf->y += 1;
  g = csf->values[COLOR_SELECT_GREEN];
  r = (csf->height - csf->y + 1) * 255 / csf->height;

  if (r < 0)
    r = 0;
  if (r > 255)
    r = 255;

  b = 0;
  db = 255.0 / csf->width;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = r;
      *p++ = g;
      *p++ = b;

      b += db;
    }
}

static void
color_select_update_green_blue (ColorSelectFill *csf)
{
  guchar *p;
  gint i, g, r;
  gfloat b, db;

  p = csf->buffer;

  csf->y += 1;
  r = csf->values[COLOR_SELECT_RED];
  g = (csf->height - csf->y + 1) * 255 / csf->height;

  if (g < 0)
    g = 0;
  if (g > 255)
    g = 255;

  b = 0;
  db = 255.0 / csf->width;

  for (i = 0; i < csf->width; i++)
    {
      *p++ = r;
      *p++ = g;
      *p++ = b;

      b += db;
    }
}

static void
color_select_update_hue_saturation (ColorSelectFill *csf)
{
  guchar *p;
  gfloat h, v, s, ds;
  gint f;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  h = 360 - (csf->y * 360 / csf->height);

  if (h < 0)
    h = 0;
  if (h > 359)
    h = 359;

  h /= 60;
  f = (h - (int) h) * 255;

  s = 0;
  ds = 1.0 / csf->width;

  v = csf->values[COLOR_SELECT_VALUE] / 100.0;

  switch ((int) h)
    {
    case 0:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * 255 * (1 - s);

	  s += ds;
	}
      break;
    case 1:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);

	  s += ds;
	}
      break;
    case 2:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v *255;
	  *p++ = v * (255 - (s * (255 - f)));

	  s += ds;
	}
      break;
    case 3:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;

	  s += ds;
	}
      break;
    case 4:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * (255 * (1 - s));
	  *p++ = v * 255;

	  s += ds;
	}
      break;
    case 5:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);

	  s += ds;
	}
      break;
    }
}

static void
color_select_update_hue_value (ColorSelectFill *csf)
{
  guchar *p;
  gfloat h, v, dv, s;
  gint f;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  h = 360 - (csf->y * 360 / csf->height);

  if (h < 0)
    h = 0;
  if (h > 359)
    h = 359;

  h /= 60;
  f = (h - (int) h) * 255;

  v = 0;
  dv = 1.0 / csf->width;

  s = csf->values[COLOR_SELECT_SATURATION] / 100.0;

  switch ((int) h)
    {
    case 0:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * 255 * (1 - s);

	  v += dv;
	}
      break;
    case 1:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);

	  v += dv;
	}
      break;
    case 2:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v *255;
	  *p++ = v * (255 - (s * (255 - f)));

	  v += dv;
	}
      break;
    case 3:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;

	  v += dv;
	}
      break;
    case 4:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * (255 * (1 - s));
	  *p++ = v * 255;

	  v += dv;
	}
      break;
    case 5:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);

	  v += dv;
	}
      break;
    }
}

static void
color_select_update_saturation_value (ColorSelectFill *csf)
{
  guchar *p;
  gfloat h, v, dv, s;
  gint f;
  gint i;

  p = csf->buffer;

  csf->y += 1;
  s = (float) csf->y / csf->height;

  if (s < 0)
    s = 0;
  if (s > 1)
    s = 1;

  s = 1 - s;

  h = (float) csf->values[COLOR_SELECT_HUE];
  if (h >= 360)
    h -= 360;
  h /= 60;
  f = (h - (int) h) * 255;

  v = 0;
  dv = 1.0 / csf->width;

  switch ((int) h)
    {
    case 0:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * 255 * (1 - s);

	  v += dv;
	}
      break;
    case 1:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);

	  v += dv;
	}
      break;
    case 2:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v *255;
	  *p++ = v * (255 - (s * (255 - f)));

	  v += dv;
	}
      break;
    case 3:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);
	  *p++ = v * 255;

	  v += dv;
	}
      break;
    case 4:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * (255 - (s * (255 - f)));
	  *p++ = v * (255 * (1 - s));
	  *p++ = v * 255;

	  v += dv;
	}
      break;
    case 5:
      for (i = 0; i < csf->width; i++)
	{
	  *p++ = v * 255;
	  *p++ = v * 255 * (1 - s);
	  *p++ = v * (255 - s * f);

	  v += dv;
	}
      break;
    }
}


/*****************************/
/* Colour notebook glue      */

typedef struct
{
  GimpColorSelector_Callback  callback;
  gpointer                   *client_data;
  ColorSelect                *csp;
  GtkWidget                  *main_vbox;
} notebook_glue;

static GtkWidget *
color_select_notebook_new (gint                         r,
			   gint                         g,
			   gint                         b,
			   GimpColorSelector_Callback   callback,
			   gpointer                     data,
			   /* RETURNS: */
			   void                       **selector_data)
{
  ColorSelect   *csp;
  notebook_glue *glue;

  glue = g_new (notebook_glue, 1);

  csp = g_new (ColorSelect, 1);

  glue->csp         = csp;
  glue->callback    = callback;
  glue->client_data = data;

  csp->callback      = color_select_notebook_update_callback;
  csp->client_data   = glue;
  csp->z_color_fill  = COLOR_SELECT_HUE;
  csp->xy_color_fill = COLOR_SELECT_SATURATION_VALUE;
  csp->gc            = NULL;
  csp->wants_updates = TRUE;

  csp->values[COLOR_SELECT_RED]   = csp->orig_values[0] = r;
  csp->values[COLOR_SELECT_GREEN] = csp->orig_values[1] = g;
  csp->values[COLOR_SELECT_BLUE]  = csp->orig_values[2] = b;
  color_select_update_hsv_values (csp);
  color_select_update_pos (csp);

  glue->main_vbox = color_select_widget_new (csp, r, g, b);

  color_select_image_fill (csp->z_color, csp->z_color_fill, csp->values);
  color_select_image_fill (csp->xy_color, csp->xy_color_fill, csp->values);

  (*selector_data) = glue;
  return glue->main_vbox;
}

static void
color_select_notebook_free (void *data)
{
  notebook_glue *glue = data;

  gdk_gc_destroy (glue->csp->gc);
  g_free (glue->csp);
  /* don't need to destroy the widget, since it's done by the caller
   * of this function */
  g_free (glue);
}


static void
color_select_notebook_setcolor (void *data,
				int r, int g, int b, int set_current)
{
  notebook_glue *glue = data;

  color_select_set_color (glue->csp, r, g, b, set_current);
}

static void
color_select_notebook_update_callback (gint              r,
				       gint              g,
				       gint              b,
				       ColorSelectState  state,
				       void             *data)
{
  notebook_glue *glue = data;

  switch (state)
    {
    case COLOR_SELECT_UPDATE:
      glue->callback (glue->client_data, r, g, b);
      break;

    default:
      g_warning ("state %d can't happen!", state);
      break;
    }
}
