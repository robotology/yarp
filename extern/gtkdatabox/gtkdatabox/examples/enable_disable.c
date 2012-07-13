/* $Id: enable_disable.c 4 2008-06-22 09:19:11Z rbock $ */
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
#include <gtkdatabox_bars.h>
#include <math.h>

#define POINTS 50

/*----------------------------------------------------------------
 *  databox show_hide
 *----------------------------------------------------------------*/

typedef void (*set_function) (GtkDatabox * box, gboolean enable);
typedef gboolean (*get_function) (GtkDatabox * box);

typedef struct
{
   char *title;
   set_function set;
   get_function get;
   GtkDatabox *box;
   GtkEntry *entry;
}
EnableSet;

EnableSet enableSets[] = {
   {"Selection Enabled",
    gtk_databox_set_enable_selection,
    gtk_databox_get_enable_selection,
    NULL,
    NULL},
   {"Zoom Enabled",
    gtk_databox_set_enable_zoom,
    gtk_databox_get_enable_zoom,
    NULL,
    NULL},
};

gint noEnableSets = sizeof (enableSets) / sizeof (EnableSet);

static gboolean
update_enable (EnableSet * enableSet)
{
   gboolean status = !(enableSet->get (enableSet->box));

   enableSet->set (enableSet->box, status);
   gtk_entry_set_text (GTK_ENTRY (enableSet->entry),
		       (status) ? "TRUE" : "FALSE");

   return FALSE;
}

static void
create_show_hide (void)
{
   GtkWidget *window = NULL;
   GtkWidget *box1;
   GtkWidget *box2;
   GtkWidget *close_button;
   GtkWidget *enable_box;
   GtkWidget *box;
   GtkWidget *table;
   GtkWidget *label;
   GtkWidget *separator;
   GtkDataboxGraph *graph;
   gfloat *X;
   gfloat *Y;
   GdkColor color;
   gint i;

   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_widget_set_size_request (window, 600, 600);

   g_signal_connect (GTK_OBJECT (window), "destroy",
		     G_CALLBACK (gtk_main_quit), NULL);

   gtk_window_set_title (GTK_WINDOW (window),
			 "GtkDatabox: Enable/Disable Features");
   gtk_container_set_border_width (GTK_CONTAINER (window), 0);

   box1 = gtk_vbox_new (FALSE, 0);
   gtk_container_add (GTK_CONTAINER (window), box1);

   label =
      gtk_label_new
      ("Click on the buttons to enable/disable features.\n\nFor basic understanding: See basics :-)\n\n");
   gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 0);

   enable_box = gtk_hbox_new (FALSE, 0);
   gtk_box_pack_start (GTK_BOX (box1), enable_box, FALSE, FALSE, 0);
   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, FALSE, 0);

   /* Create a GtkDatabox widget along with scrollbars and rulers */
   gtk_databox_create_box_with_scrollbars_and_rulers (&box, &table,
						      TRUE, TRUE, TRUE, TRUE);

   gtk_box_pack_start (GTK_BOX (box1), table, TRUE, TRUE, 0);

   X = g_new0 (gfloat, POINTS);
   Y = g_new0 (gfloat, POINTS);

   for (i = 0; i < POINTS; i++)
   {
      X[i] = i;
      Y[i] = 100. * sin (i * 2 * G_PI / POINTS);
   }
   color.red = 0;
   color.green = 0;
   color.blue = 65535;

   graph = gtk_databox_bars_new (POINTS, X, Y, &color, 1);
   gtk_databox_graph_add (GTK_DATABOX (box), graph);

   gtk_databox_auto_rescale (GTK_DATABOX (box), 0.05);

   separator = gtk_hseparator_new ();
   gtk_box_pack_start (GTK_BOX (box1), separator, FALSE, TRUE, 0);

   box2 = gtk_vbox_new (FALSE, 10);
   gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
   gtk_box_pack_end (GTK_BOX (box1), box2, FALSE, TRUE, 0);
   close_button = gtk_button_new_with_label ("close");
   g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
			     G_CALLBACK (gtk_main_quit), GTK_OBJECT (box));
   gtk_box_pack_start (GTK_BOX (box2), close_button, TRUE, TRUE, 0);
   GTK_WIDGET_SET_FLAGS (close_button, GTK_CAN_DEFAULT);
   for (i = 0; i < noEnableSets; ++i)
   {
      GtkWidget *vbox = gtk_vbox_new (FALSE, 10);
      GtkWidget *label = gtk_label_new (enableSets[i].title);
      GtkWidget *entry = gtk_entry_new ();
      GtkWidget *button = gtk_button_new_with_label ("Change");

      gtk_box_pack_start (GTK_BOX (enable_box), vbox, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (vbox), entry, TRUE, TRUE, 0);
      gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

      gtk_editable_set_editable (GTK_EDITABLE (entry), FALSE);
      g_signal_connect_swapped (button, "clicked",
				G_CALLBACK (update_enable),
				(gpointer) & (enableSets[i]));
      enableSets[i].box = GTK_DATABOX (box);
      enableSets[i].entry = GTK_ENTRY (entry);

      update_enable (&enableSets[i]);
      update_enable (&enableSets[i]);

   }

   gtk_widget_grab_default (close_button);
   gtk_widget_show_all (window);

}

gint
main (gint argc, char *argv[])
{
   gtk_init (&argc, &argv);

   create_show_hide ();
   gtk_main ();

   return 0;
}
