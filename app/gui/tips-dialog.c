#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "gtk/gtk.h"
#include "tips_dialog.h"
#include "gimprc.h"
#include "interface.h"
#include "wilber.h"

#include "libgimp/gimpintl.h"
#include "libgimp/gimpenv.h"

#define TIPS_DIR_NAME	"tips"

static int  tips_dialog_hide (GtkWidget *widget, gpointer data);
static int  tips_show_next (GtkWidget *widget, gpointer data);
static void tips_toggle_update (GtkWidget *widget, gpointer data);
static void read_tips_file(char *filename);

static GtkWidget *tips_dialog = NULL;
static GtkWidget *tips_label;
static char **    tips_text = NULL;
static int        tips_count = 0;
static int        old_show_tips;

void
tips_dialog_create ()
{
  GtkWidget *vbox;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *bbox;
  GtkWidget *vbox_bbox2;
  GtkWidget *bbox2;
  GtkWidget *frame;
  GtkWidget *preview;
  GtkWidget *button_close;
  GtkWidget *button_next;
  GtkWidget *button_prev;
  GtkWidget *vbox_check;
  GtkWidget *button_check;
  gchar  *   temp;
  guchar *   utemp;
  guchar *   src;
  guchar *   dest;
  int        x;
  int        y;

  if (tips_count == 0)
    {
      temp = g_strdup_printf ("%s" G_DIR_SEPARATOR_S TIPS_DIR_NAME
                              G_DIR_SEPARATOR_S "%s",
			      gimp_data_directory (),
                              _("gimp_tips.txt"));
      read_tips_file (temp);
      g_free (temp);
    }

  if (last_tip >= tips_count || last_tip < 0)
    last_tip = 0;

  if (!tips_dialog)
    {
      tips_dialog = gtk_window_new (GTK_WINDOW_DIALOG);
      gtk_window_set_wmclass (GTK_WINDOW (tips_dialog), "tip_of_the_day", "Gimp");
      gtk_window_set_title (GTK_WINDOW (tips_dialog), _("GIMP Tip of the day"));
      gtk_window_set_position (GTK_WINDOW (tips_dialog), GTK_WIN_POS_CENTER);
      gtk_signal_connect (GTK_OBJECT (tips_dialog), "delete_event",
			  GTK_SIGNAL_FUNC (tips_dialog_hide), NULL);
      /* destroy the tips window if the mainlevel gtk_main() function is left */
      gtk_quit_add_destroy (1, GTK_OBJECT (tips_dialog));

      vbox = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (tips_dialog), vbox);
      gtk_widget_show (vbox);

      hbox1 = gtk_hbox_new (FALSE, 5);
      gtk_container_set_border_width (GTK_CONTAINER (hbox1), 10);
      gtk_box_pack_start (GTK_BOX (vbox), hbox1, FALSE, TRUE, 0);
      gtk_widget_show (hbox1);

      hbox2 = gtk_hbox_new (FALSE, 5);
      gtk_container_set_border_width (GTK_CONTAINER (hbox2), 10);
      gtk_box_pack_end (GTK_BOX (vbox), hbox2, FALSE, TRUE, 0);
      gtk_widget_show (hbox2);
      
      bbox = gtk_hbutton_box_new ();
      gtk_box_pack_end (GTK_BOX (hbox2), bbox, FALSE, FALSE, 0);
      gtk_widget_show (bbox);

      vbox_bbox2 = gtk_vbox_new (FALSE, 0);
      gtk_box_pack_end (GTK_BOX (hbox2), vbox_bbox2, FALSE, FALSE, 15);
      gtk_widget_show (vbox_bbox2);

      bbox2 = gtk_hbox_new (TRUE, 5); 
      gtk_box_pack_end (GTK_BOX (vbox_bbox2), bbox2, TRUE, FALSE, 0);
      gtk_widget_show(bbox2);

      preview = gtk_preview_new (GTK_PREVIEW_COLOR);
      gtk_preview_size (GTK_PREVIEW (preview), wilber_width, wilber_height);
      utemp = g_new (guchar, wilber_width * 3);
      src = (guchar *)wilber_data;
      for (y = 0; y < wilber_height; y++)
	{
	  dest = utemp;
	  for (x = 0; x < wilber_width; x++)
	    {
	      HEADER_PIXEL(src, dest);
	      dest += 3;
	    }
	  gtk_preview_draw_row (GTK_PREVIEW (preview), utemp,
				0, y, wilber_width); 
	}
      g_free(utemp);
      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
      gtk_box_pack_end (GTK_BOX (hbox1), frame, FALSE, TRUE, 3);
      gtk_container_add (GTK_CONTAINER (frame), preview);
      gtk_widget_show (preview);
      gtk_widget_show (frame);

      tips_label = gtk_label_new (tips_text[last_tip]);
      gtk_label_set_justify (GTK_LABEL (tips_label), GTK_JUSTIFY_LEFT);
      gtk_box_pack_start (GTK_BOX (hbox1), tips_label, TRUE, TRUE, 3);
      gtk_widget_show (tips_label);

      button_prev = gtk_button_new_with_label (_("Previous Tip"));
      GTK_WIDGET_UNSET_FLAGS (button_prev, GTK_RECEIVES_DEFAULT);
      gtk_signal_connect (GTK_OBJECT (button_prev), "clicked",
			  GTK_SIGNAL_FUNC (tips_show_next),
			  (gpointer) "prev");
      gtk_container_add (GTK_CONTAINER (bbox2), button_prev);
      gtk_widget_show (button_prev);

      button_next = gtk_button_new_with_label (_("Next Tip"));
      GTK_WIDGET_UNSET_FLAGS (button_next, GTK_RECEIVES_DEFAULT);
      gtk_signal_connect (GTK_OBJECT (button_next), "clicked",
			  GTK_SIGNAL_FUNC (tips_show_next),
			  (gpointer) "next");
      gtk_container_add (GTK_CONTAINER (bbox2), button_next);
      gtk_widget_show (button_next);

      button_close = gtk_button_new_with_label (_("Close"));
      GTK_WIDGET_SET_FLAGS (button_close, GTK_CAN_DEFAULT);
      gtk_window_set_default (GTK_WINDOW (tips_dialog), button_close);
      gtk_signal_connect (GTK_OBJECT (button_close), "clicked",
			  GTK_SIGNAL_FUNC (tips_dialog_hide), NULL);
      gtk_container_add (GTK_CONTAINER (bbox), button_close);
      gtk_widget_show (button_close);

      vbox_check = gtk_vbox_new (FALSE, 0); 
      gtk_box_pack_start (GTK_BOX (hbox2), vbox_check, FALSE, TRUE, 0);
      gtk_widget_show (vbox_check);

      button_check = gtk_check_button_new_with_label (_("Show tip next time"));
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_check),
				    show_tips);
      gtk_signal_connect (GTK_OBJECT (button_check), "toggled",
			  GTK_SIGNAL_FUNC (tips_toggle_update),
			  (gpointer) &show_tips);
      gtk_box_pack_start (GTK_BOX (vbox_check), button_check, TRUE, FALSE, 0);
      gtk_widget_show (button_check);

      old_show_tips = show_tips;
    }

  if (!GTK_WIDGET_VISIBLE (tips_dialog))
    {
      gtk_widget_show (tips_dialog);
    }
  else
    {
      gdk_window_raise (tips_dialog->window);
    }
}

static int
tips_dialog_hide (GtkWidget *widget,
		  gpointer data)
{
  GList *update = NULL; /* options that should be updated in .gimprc */
  GList *remove = NULL; /* options that should be commented out */

  gtk_widget_hide (tips_dialog);

  /* the last-shown-tip is now saved in sessionrc */

  if (show_tips != old_show_tips)
    {
      update = g_list_append (update, "show-tips");
      remove = g_list_append (remove, "dont-show-tips");
      old_show_tips = show_tips;
      save_gimprc (&update, &remove);
    }
  g_list_free (update);
  g_list_free (remove);

  return TRUE;
}

static int
tips_show_next (GtkWidget *widget,
		gpointer  data)
{
  if (!strcmp ((char *)data, "prev"))
    {
      last_tip--;
      if (last_tip < 0)
	last_tip = tips_count - 1;
    }
  else
    {
      last_tip++;
      if (last_tip >= tips_count)
	last_tip = 0;
    }
  gtk_label_set (GTK_LABEL (tips_label), tips_text[last_tip]);
  return FALSE;
}

static void
tips_toggle_update (GtkWidget *widget,
		    gpointer   data)
{
  int *toggle_val;

  toggle_val = (int *) data;

  if (GTK_TOGGLE_BUTTON (widget)->active)
    *toggle_val = TRUE;
  else
    *toggle_val = FALSE;
}

static void
store_tip (char *str)
{
  tips_count++;
  tips_text = g_realloc(tips_text, sizeof(char *) * tips_count);
  tips_text[tips_count - 1] = str;
}

static void
read_tips_file (char *filename)
{
  FILE *fp;
  char *tip = NULL;
  char *str = NULL;

  fp = fopen (filename, "r");
  if (!fp)
    {
      store_tip (_("Your GIMP tips file appears to be missing!\n"
		 "There should be a file called gimp_tips.txt in the\n"
		 "GIMP data directory.  Please check your installation."));
      return;
    }

  str = g_new (char, 1024);
  while (!feof (fp))
    {
      if (!fgets (str, 1024, fp))
	continue;
      
      if (str[0] == '#' || str[0] == '\n')
	{
	  if (tip != NULL)
	    {
	      tip[strlen (tip) - 1] = '\000';
	      store_tip (tip);
	      tip = NULL;
	    }
	}
      else
	{
	  if (tip == NULL)
	    {
	      tip = g_malloc (strlen (str) + 1);
	      strcpy (tip, str);
	    }
	  else
	    {
	      tip = g_realloc (tip, strlen (tip) + strlen (str) + 1);
	      strcat (tip, str);
	    }
	}
    }
  if (tip != NULL)
    store_tip (tip);
  g_free (str);
  fclose (fp);
}
