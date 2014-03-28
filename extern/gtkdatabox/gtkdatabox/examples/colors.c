/* $Id: colors.c 4 2008-06-22 09:19:11Z rbock $ */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998-2008  Dr. Roland Bock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkcolorsel.h>
#include <gtk/gtkitemfactory.h>
#include <gtkdatabox.h>
#include <gtkdatabox_points.h>
#include <math.h>

#define POINTS 2000
#define NO_OF_DATASETS 5

typedef struct
{
   GtkColorSelectionDialog *selector;
   GtkWidget *box;
   GtkDataboxGraph *graph;
   gint index;
}
col_sel;

static void
get_color_cb (col_sel * sel /*, GtkWidget *widget */ )
{
   GdkColor color;

   g_return_if_fail (GTK_IS_COLOR_SELECTION_DIALOG (sel->selector));
   g_return_if_fail (GTK_IS_DATABOX (sel->box));

   gtk_color_selection_get_current_color (GTK_COLOR_SELECTION
					  (sel->selector->colorsel), &color);

   gtk_databox_graph_set_color (sel->graph, &color);
   gtk_widget_queue_draw (GTK_WIDGET (sel->box));
}

static void
menu_color_change_cb (col_sel * sel)
{
   GtkColorSelectionDialog *selector;
   gchar title[20];
   GdkColor *color;

   sprintf (title, "Choose color #%d", sel->index);
   selector =
      GTK_COLOR_SELECTION_DIALOG (gtk_color_selection_dialog_new (title));
   gtk_widget_destroy (selector->help_button);
   sel->selector = selector;

   color = gtk_databox_graph_get_color (sel->graph);
   gtk_color_selection_set_current_color (GTK_COLOR_SELECTION
					  (selector->colorsel), color);

   g_signal_connect_object (G_OBJECT (selector->cancel_button), "clicked",
			    G_CALLBACK (gtk_widget_destroy),
			    G_OBJECT (selector), G_CONNECT_SWAPPED);
   g_signal_connect_swapped (G_OBJECT
			     (selector->ok_button),
			     "clicked", G_CALLBACK (get_color_cb),
			     (gpointer) sel);
   gtk_widget_show (GTK_WIDGET (selector));

   return;
}

static void
create_menu_entry (GtkWidget * menu, int index,
		   GtkWidget * box, GtkDataboxGraph * graph)
{
   GtkWidget *menu_items;
   char buf[128];
   col_sel *sel = g_new0 (col_sel, 1);

   sel->index = index;
   sel->box = box;
   sel->graph = graph;
   sprintf (buf, "Change Color #%d", index);
   menu_items = gtk_menu_item_new_with_label (buf);
   gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_items);

   g_signal_connect_swapped (G_OBJECT (menu_items), "activate",
			     G_CALLBACK (menu_color_change_cb),
			     (gpointer) sel);

   gtk_widget_show (menu_items);
}

static void
create_colors (void)
{
   static GtkWidget *window = NULL;
   GtkWidget *box1 = NULL;
   GtkWidget *box2 = NULL;
   GtkWidget *close_button = NULL;
   GtkWidget *box = NULL;
   GtkWidget *separator;
   GtkWidget *label;
   GtkWidget *menu;
   GtkWidget *root_menu;
   GtkWidget *menu_bar;
   gfloat *X = NULL;
   gfloat *Y = NULL;
   gint i, j;
   GdkColor color;
   GtkDataboxGraph *graph;

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 500, 300);

   g_signal_connect (G_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window), "GtkDatabox: Colors");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   box1 = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), box1);

   box = gtk_databox_new ();
   gtk_databox_set_enable_selection (GTK_DATABOX (box), FALSE);
   gtk_databox_set_enable_zoom (GTK_DATABOX (box), FALSE);

   color.red = 0;
   color.green = 0;
   color.blue = 0;
   gtk_widget_modify_bg (box, GTK_STATE_NORMAL, &color);

   menu = gtk_menu_new ();
   root_menu = gtk_menu_item_new_with_label ("Color Menu");
   gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
   menu_bar = gtk_menu_bar_new ();
   gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), root_menu);
   gtk_box_pack_start (GTK_BOX (box1), menu_bar, FALSE, TRUE, 0);


   label =
      gtk_label_new
      ("You can change the colors of the shown datasets via the menu.\n\n");
   gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 0);

   g_signal_connect (G_OBJECT (box), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);


   for (i = 0; i < NO_OF_DATASETS; i++)
   {
      if (!i)
	 X = g_new0 (gfloat, POINTS);
      Y = g_new0 (gfloat, POINTS);
      for (j = 0; j < POINTS; j++)
      {
	 X[j] = j;
	 Y[j] = 100. * sin ((i + 1) * 2 * j * G_PI / POINTS);
      }
      color.red = 65535 * ((i + 1) % 2);
      color.green = (65535 / 2) * ((i + 1) % 3);
      color.blue = (65535 / 3) * ((i + 1) % 4);
      graph = gtk_databox_points_new (POINTS, X, Y, &color, 1);
      gtk_databox_graph_add (GTK_DATABOX (box), graph);
      create_menu_entry (menu, i, box, graph);
   }

   gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);

   gtk_box_pack_start (GTK_BOX (box1), box, TRUE, TRUE, 0);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

   box2 = gtk_vbox_new (FALSE, 10);
   gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
   gtk_box_pack_end (GTK_BOX (box1), box2, FALSE, TRUE, 0);
   close_button = gtk_button_new_with_label ("close");
   g_signal_connect (G_OBJECT (close_button), "clicked",
		     G_CALLBACK (gtk_main_quit), (gpointer) NULL);
   gtk_box_pack_start (GTK_BOX (box2), close_button, TRUE, TRUE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   gtk_widget_grab_default (close_button);





   gtk_widget_show_all (window);

}

gint
main (gint argc, char *argv[])
{
   gtk_init (&argc, &argv);

   create_colors ();
   gtk_main ();

   return 0;
}
