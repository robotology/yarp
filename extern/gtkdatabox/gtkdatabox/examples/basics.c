/* $Id: basics.c 4 2008-06-22 09:19:11Z rbock $ */
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

#define POINTS 2000
#define STEPS 50
#define BARS 25
#define MARKER 10

/*----------------------------------------------------------------
 *  databox basics
 *----------------------------------------------------------------*/

static void
create_basics (void)
{
   GtkWidget *window = NULL;
   GtkWidget *vbox;
   GtkWidget *close_button;
   GtkWidget *box;
   GtkWidget *label;
   GtkWidget *separator;
   GtkWidget *table;
   GtkDataboxGraph *graph;
   gfloat *X;
   gfloat *Y;
   GdkColor color;
   gint i;

   /* We define some data */
   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i;
      Y[i] = sin (i * 4 * G_PI / POINTS);
   }

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 500, 500);

   g_signal_connect (GTK_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window), "GtkDatabox: Basics");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   vbox = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), vbox);

   label =
      gtk_label_new
      ("\nThe code for this example demonstrates\n the simplest way to use a GtkDatabox widget.\n\nUsage:\nDraw a selection with the left button pressed,\nThan click into the selection.\nUse the right mouse button to zoom out.\nShift+ right mouse button zooms to default.");
   gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);

   /* ----------------------------------------------------------------- 
    * This is all you need:
    * -----------------------------------------------------------------
    */

   /* Create the GtkDatabox widget */
   gtk_databox_create_box_with_scrollbars_and_rulers (&box, &table,
						      TRUE, TRUE, TRUE, TRUE);

   /* Put it somewhere */
   gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);

   /* Add your data data in some color */
   color.red = 0;
   color.green = 0;
   color.blue = 0;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   gtk_databox_set_total_limits (GTK_DATABOX (box), -1000., 5000., -10000., 23000.);
   gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);

   /* ----------------------------------------------------------------- 
    * Done :-)
    * -----------------------------------------------------------------
    */

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 0);

   close_button = gtk_button_new_with_label ("close");
   g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
			     G_CALLBACK (gtk_main_quit), GTK_OBJECT (box));
   gtk_box_pack_start (GTK_BOX (vbox), close_button, FALSE, FALSE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);
   gtk_widget_grab_focus (close_button);

   gtk_widget_show_all (window);
   gdk_window_set_cursor (box->window, gdk_cursor_new (GDK_CROSS));
}

gint
main (gint argc, char *argv[])
{
   gtk_init (&argc, &argv);

   create_basics ();
   gtk_main ();

   return 0;
}
