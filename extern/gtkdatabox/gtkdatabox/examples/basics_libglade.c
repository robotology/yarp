/* $Id: basics_libglade.c 4 2008-06-22 09:19:11Z rbock $ */
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998 - 2006  Dr. Roland Bock
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
#include <glib/gprintf.h>
#include <gtkdatabox.h>
#include <gtkdatabox_points.h>
#include <gtkdatabox_ruler.h>
#include <gtkdatabox_lines.h>
#include <gtkdatabox_bars.h>
#include <gtkdatabox_cross_simple.h>
#include <math.h>

#include <config.h>

#ifdef USE_LIBGLADE
#include <glade/glade.h>
#endif

#define POINTS 2000
#define STEPS 50
#define BARS 25
#define MARKER 10

#ifdef USE_LIBGLADE

/*----------------------------------------------------------------
 *  databox basics
 *----------------------------------------------------------------*/

#define GLADE_FILE "basics_libglade.glade"

static void
create_basics ()
{
   GtkWidget *window = NULL;
   GtkWidget *box;
   GtkWidget *scrollbar;
   GtkWidget *ruler;
   GtkDataboxGraph *graph;
   gfloat min_x, max_x;
   gfloat min_y, max_y;
   gfloat *X;
   gfloat *Y;
   gfloat buffer;
   GdkColor color;
   gint i;
   GladeXML *gxml;

   gxml = glade_xml_new (GLADE_FILE, NULL, NULL);

   /* This is important */
   glade_xml_signal_autoconnect (gxml);
   window = glade_xml_get_widget (gxml, "window");


   /* You could associate a scrollbar with a GtkDatabox widget either 
    * this way ...*/
   scrollbar = glade_xml_get_widget (gxml, "scrollbar_x");
   box = glade_xml_get_widget (gxml, "box");
   gtk_databox_set_adjustment_x (GTK_DATABOX (box),
				gtk_range_get_adjustment (GTK_RANGE
							  (scrollbar)));

   /* or this way ... */
   scrollbar = glade_xml_get_widget (gxml, "scrollbar_y");
   gtk_databox_set_adjustment_y (GTK_DATABOX (box),
				gtk_range_get_adjustment (GTK_RANGE
							  (scrollbar)));
   ruler = glade_xml_get_widget (gxml, "ruler_x");
   gtk_databox_set_ruler_x (GTK_DATABOX (box), GTK_DATABOX_RULER (ruler));
   ruler = glade_xml_get_widget (gxml, "ruler_y");
   gtk_databox_set_ruler_y (GTK_DATABOX (box), GTK_DATABOX_RULER (ruler));
   /* end of gtk_databox_create_box_with_scrollbars_and_rulers */


   color.red = 16383;
   color.green = 16383;
   color.blue = 16383;
   gtk_widget_modify_bg (box, GTK_STATE_NORMAL, &color);


   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i;
      Y[i] = 100. * sin (i * 2 * G_PI / POINTS);
   }
   color.red = 0;
   color.green = 65535;
   color.blue = 0;

   graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   X = g_new0 (gfloat, STEPS);
   Y = g_new0 (gfloat, STEPS);

   for (i = 0; i < STEPS / 2; i++)
   {
      X[i * 2] = X[i * 2 + 1] = 2 * (i + .5) * POINTS / STEPS;

      if (i > 0)
	 Y[i * 2 - 1] = 100. * sin ((i * 2) * 2 * G_PI / STEPS);
      Y[i * 2] = 100. * sin ((i * 2) * 2 * G_PI / STEPS);
      if (i == STEPS / 2 - 1)
	 Y[i * 2 + 1] = 100. * sin (((i + 1) * 2) * 2 * G_PI / STEPS);
   }
   color.red = 65535;
   color.green = 0;
   color.blue = 0;

   graph = gtk_databox_lines_new (STEPS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   X = g_new0 (gfloat, BARS);
   Y = g_new0 (gfloat, BARS);

   for (i = 0; i < BARS; i++)
   {
      X[i] = i * POINTS / BARS;
      Y[i] = 80. * sin (i * 2 * G_PI / BARS);
   }
   color.red = 0;
   color.green = 65535;
   color.blue = 65535;

   graph = gtk_databox_bars_new (BARS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   color.red = 32768;
   color.green = 32768;
   color.blue = 32768;

   graph = gtk_databox_cross_simple_new (&color, 0);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   /* Instead of doing stuff what you see below, you could call 
    * gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);
    * as is done in the other examples 
    * Of course, you are more flexible in adjusting the scaling by doing 
    * it yourself, though. */
   if (0 >
       gtk_databox_calculate_extrema (GTK_DATABOX (box), &min_x, &max_x,
				      &min_y, &max_y))
   {
      g_warning ("Calculating extrema failed. Resorting to default values");
      min_x = -100.;
      min_y = -100.;
      max_x = +100.;
      max_y = +100.;
   }
   else
   {
      gfloat width = max_x - min_x;
      gfloat height = max_y - min_y;
      gfloat border = 0.05;

      min_x -= border * width;
      min_y -= border * height;
      max_x += border * width;
      max_y += border * height;
   }

   /* We want larger y values to be higher on the screen */
   buffer = min_y;
   min_y = max_y;
   max_y = buffer;

   gtk_databox_set_total_limits (GTK_DATABOX (box), min_x, max_x, min_y,
				 max_y);
   /* end of gtk_databox_auto_rescale */

   gtk_widget_show (window);
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

#else /* USE_LIBGLADE */

/* Build some dummy program if libglade is not used. */
gint
main ()
{
   g_printf
      ("This program does nothing useful unless you call the configure script with --enable-libglade\n");
   return 0;
}

#endif /* USE_LIBGLADE */
