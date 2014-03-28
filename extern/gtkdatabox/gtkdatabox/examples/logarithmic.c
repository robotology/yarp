/* $Id: logarithmic.c 4 2008-06-22 09:19:11Z rbock $ */
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
#include <gtkdatabox_ruler.h>
#include <math.h>

#define POINTS 100000
/*
#define STEPS 50
#define BARS 25
#define MARKER 10
*/
#define NO_BOXES 4

/*----------------------------------------------------------------
 *  databox logarithmic
 *----------------------------------------------------------------*/

static void
create_logarithmic (void)
{
   GtkWidget *window = NULL;
   GtkWidget *hbox;
   GtkWidget *vbox;
   GtkWidget *close_button;
   GtkWidget *box[NO_BOXES];
   GtkWidget *label;
   GtkWidget *separator;
   GtkWidget *table;
   GtkDataboxGraph *graph;
   gfloat min_x, max_x;
   gfloat min_y, max_y;
   gfloat *X;
   gfloat *Y;
   GdkColor color;
   gint i;

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 1000, 500);

   g_signal_connect (GTK_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window),
			 "GtkDatabox: Logarithmic Plots");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), vbox);

   label = gtk_label_new
      ("The following types of curves are shown (with some adjustments to make them fit nicely into the window):\n Green: sin^2, Red: exp, Magenta: 1/x, Yellow: x^2\n(100K Points per each curve in each graph)\n");
   gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

   hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

   label = gtk_label_new ("Linear Plot:");
   gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

   label = gtk_label_new ("Logarithmic Plot:");
   gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

   label = gtk_label_new ("Double Logarithmic Plot:");
   gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

   label = gtk_label_new ("Log base 2 Plot:");
   gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

   /* Create the databoxes */
   hbox = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

   for (i = 0; i < NO_BOXES; ++i)
   {
      gtk_databox_create_box_with_scrollbars_and_rulers (&box[i], &table,
							 TRUE, TRUE, TRUE,
							 TRUE);
      gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0);
   }

   /* set the background */
   color.red = 16383;
   color.green = 16383;
   color.blue = 16383;
   for (i = 0; i < NO_BOXES; ++i)
      gtk_widget_modify_bg (box[i], GTK_STATE_NORMAL, &color);

   /* add a sinus^2 */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i / 100.;
      Y[i] = 0.01 + 1000 * pow (sin (i * G_PI / POINTS), 2);
   }

   color.red = 0;
   color.green = 65535;
   color.blue = 0;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   for (i = 0; i < NO_BOXES; ++i)
      gtk_databox_graph_add (GTK_DATABOX (box[i]), graph);

   /* add an exp function */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i / 100.;
      Y[i] = exp (log (1000) * i / ((gfloat) POINTS));
   }

   color.red = 65535;
   color.green = 0;
   color.blue = 0;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   for (i = 0; i < NO_BOXES; ++i)
      gtk_databox_graph_add (GTK_DATABOX (box[i]), graph);

   /* add an 1/x function */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i / 100.;
      Y[i] = 1000. / (100. * (gfloat) i / POINTS);
   }

   color.red = 65535;
   color.green = 0;
   color.blue = 65535;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   for (i = 0; i < NO_BOXES; ++i)
      gtk_databox_graph_add (GTK_DATABOX (box[i]), graph);

   /* add an x^2 function */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i / 100.;
      Y[i] = 1000. * (gfloat) i / POINTS * (gfloat) i / POINTS;
   }

   color.red = 65535;
   color.green = 65535;
   color.blue = 0;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   for (i = 0; i < NO_BOXES; ++i)
      gtk_databox_graph_add (GTK_DATABOX (box[i]), graph);

   /* set the extrema */
   min_x = 0.01;
   max_x = 1000.;
   min_y = 1000.;
   max_y = 0.01;

   for (i = 0; i < NO_BOXES-1; ++i)
      gtk_databox_set_total_limits (GTK_DATABOX (box[i]), min_x, max_x, min_y,
				    max_y);
      gtk_databox_set_total_limits (GTK_DATABOX (box[i]), 8, max_x, min_y,
				    max_y);

   gtk_databox_set_scale_type_y (GTK_DATABOX (box[1]), GTK_DATABOX_SCALE_LOG);
   gtk_databox_set_scale_type_x (GTK_DATABOX (box[2]), GTK_DATABOX_SCALE_LOG);
   gtk_databox_set_scale_type_y (GTK_DATABOX (box[2]), GTK_DATABOX_SCALE_LOG);
   gtk_databox_set_scale_type_x (GTK_DATABOX (box[3]), GTK_DATABOX_SCALE_LOG2);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);

   close_button = gtk_button_new_with_label ("close");
   g_signal_connect (GTK_OBJECT (close_button), "clicked",
		     G_CALLBACK (gtk_main_quit), NULL);
   gtk_box_pack_start (GTK_BOX (vbox), close_button, FALSE, FALSE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);
   gtk_widget_grab_focus (close_button);

   gtk_widget_show_all (window);
   gdk_window_set_cursor (box[0]->window, gdk_cursor_new (GDK_CROSS));
}

gint
main (gint argc, char *argv[])
{
   gtk_init (&argc, &argv);

   create_logarithmic ();
   gtk_main ();

   return 0;
}
