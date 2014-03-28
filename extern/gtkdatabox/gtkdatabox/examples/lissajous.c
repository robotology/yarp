/* $Id: lissajous.c 4 2008-06-22 09:19:11Z rbock $ */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998 - 2008  Dr. Roland Bock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <stdio.h>

#include <gtk/gtk.h>
#include <gtkdatabox.h>
#include <gtkdatabox_points.h>
#include <gtkdatabox_lines.h>
#include <math.h>

#define POINTS 2000

/*----------------------------------------------------------------
 *  databox lissajous
 *----------------------------------------------------------------*/

static gfloat *lissajousX = NULL;
static gfloat *lissajousY = NULL;

static gint lissajous_idle = 0;
static gfloat lissajous_frequency = 3. * G_PI / 2.;
static GtkWidget *lissajous_label = NULL;
static guint lissajous_counter = 0;

static gboolean
lissajous_idle_func (GtkDatabox * box)
{
   gfloat freq;
   gfloat off;
   gchar label[10];
   gint i;

   if (!GTK_IS_DATABOX (box))
      return FALSE;

   lissajous_frequency += 0.001;
   off = lissajous_counter * 4 * G_PI / POINTS;

   freq = 14 + 10 * sin (lissajous_frequency);
   for (i = 0; i < POINTS; i++)
   {
      lissajousX[i] = 100. * sin (i * 4 * G_PI / POINTS + off);
      lissajousY[i] = 100. * cos (i * freq * G_PI / POINTS + off);
   }


   gtk_widget_queue_draw (GTK_WIDGET (box));

   sprintf (label, "%d", lissajous_counter++);
   gtk_entry_set_text (GTK_ENTRY (lissajous_label), label);

   return TRUE;
}

static void
create_lissajous (void)
{
   GtkWidget *window = NULL;
   GtkWidget *box1;
   GtkWidget *box2;
   GtkWidget *close_button;
   GtkWidget *box;
   GtkWidget *label;
   GtkWidget *table;
   GtkWidget *separator;
   GtkDataboxGraph *graph;
   GdkColor color;
   gint i;

   lissajous_frequency = 0;
   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 500, 500);

   g_signal_connect (G_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window),
			 "GtkDatabox: Lissajous Example");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   box1 = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), box1);

   label =
      gtk_label_new
      ("This example resembles an oszilloscope\nreceiving two signals, one is a sine (horizontal),\nthe other is a cosine with ever changing frequency (vertical).\nThe counter is synchron with the updates.");
   gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 0);
   lissajous_label = gtk_entry_new ();
   gtk_entry_set_text (GTK_ENTRY (lissajous_label), "0");
   gtk_box_pack_start (GTK_BOX (box1), lissajous_label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 0);

   lissajous_idle = 0;
   lissajous_frequency = 3. * G_PI / 2.;
   lissajous_counter = 0;

   /* Create a GtkDatabox widget along with scrollbars and rulers */
   gtk_databox_create_box_with_scrollbars_and_rulers (&box, &table,
						      TRUE, TRUE, TRUE, TRUE);

   gtk_box_pack_start (GTK_BOX (box1), table, TRUE, TRUE, 0);

   color.red = 0;
   color.green = 0;
   color.blue = 32768;

   gtk_widget_modify_bg (box, GTK_STATE_NORMAL, &color);

   lissajousX = g_new0 (gfloat, POINTS);
   lissajousY = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      lissajousX[i] = 100. * sin (i * 4 * G_PI / POINTS);
      lissajousY[i] = 100. * cos (i * 4 * G_PI / POINTS);
   }
   color.red = 65535;
   color.green = 65535;
   color.blue = 0;

   graph = gtk_databox_lines_new (POINTS, lissajousX, lissajousY, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

   box2 = gtk_vbox_new (FALSE, 10);
   gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
   gtk_box_pack_end (GTK_BOX (box1), box2, FALSE, TRUE, 0);
   close_button = gtk_button_new_with_label ("close");

   g_signal_connect_swapped (G_OBJECT (close_button), "clicked",
			     G_CALLBACK (gtk_main_quit), G_OBJECT (box));

   gtk_box_pack_start (GTK_BOX (box2), close_button, TRUE, TRUE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);
   lissajous_idle = g_idle_add ((GSourceFunc) lissajous_idle_func, box);

   gtk_widget_show_all (window);
}

gint
main (gint argc, char *argv[])
{
   gtk_init (&argc, &argv);

   create_lissajous ();
   gtk_main ();

   return 0;
}
