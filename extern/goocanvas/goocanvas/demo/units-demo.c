#include <stdlib.h>
#include <goocanvas.h>

cairo_pattern_t *flower_pattern;
gdouble flower_width, flower_height;


static gboolean
on_motion_notify (GooCanvasItem *item,
		  GooCanvasItem *target,
		  GdkEventMotion *event,
		  gpointer data)
{
  gchar *id = g_object_get_data (G_OBJECT (item), "id");

  g_print ("%s item received 'motion-notify' signal\n", id ? id : "Unknown");

  return FALSE;
}


static void
setup_canvas (GtkWidget *canvas,
	      GtkUnit    units,
	      gchar     *units_name)
{
  GooCanvasItem *root, *item;
  gchar buffer[256], font_desc[64];
  double *d;
  double data[4][12] = {
    /* Pixels */
    { 100, 100, 200, 20, 10,
      200, 310, 24,
      310, 100, 20, 20
    },

    /* Points */
    { 100, 100, 200, 20, 10,
      200, 310, 24,
      310, 100, 20, 20
    },

    /* Inches */
    { 1, 1, 3, 0.5, 0.16,
      3, 4, 0.3,
      4.2, 1, 0.5, 0.5
    },

    /* MM */
    { 30, 30, 100, 10, 5,
      80, 60, 10,
      135, 30, 10, 10
    }
  };

  d = data[units];

  root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

  item = goo_canvas_rect_new (root, d[0], d[1], d[2], d[3],
			      NULL);
  g_signal_connect (item, "motion_notify_event",
		    G_CALLBACK (on_motion_notify), NULL);

  sprintf (buffer, "This box is %gx%g %s", d[2], d[3], units_name);
  sprintf (font_desc, "Sans %gpx", d[4]);
  item = goo_canvas_text_new (root, buffer, d[0] + d[2] / 2, d[1] + d[3] / 2,
			      -1, GTK_ANCHOR_CENTER,
			      "font", font_desc,
			      NULL);


  sprintf (buffer, "This font is %g %s high", d[7], units_name);
  sprintf (font_desc, "Sans %gpx", d[7]);
  item = goo_canvas_text_new (root, buffer, d[5], d[6], -1,
			      GTK_ANCHOR_CENTER,
			      "font", font_desc,
			      NULL);

  item = goo_canvas_image_new (root, NULL, d[8], d[9],
			       "pattern", flower_pattern,
			       "width", d[10],
			       "height", d[11],
			       "scale-to-fit", TRUE,
			       NULL);
}


static void
zoom_changed (GtkAdjustment *adj, GooCanvas *canvas)
{
  goo_canvas_set_scale (canvas, adj->value);
}


GtkWidget *
create_canvas (GtkUnit         units,
	       gchar          *units_name)
{
  GtkWidget *vbox, *hbox, *w, *scrolled_win, *canvas;
  GtkAdjustment *adj;

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  canvas = goo_canvas_new ();

  w = gtk_label_new ("Zoom:");
  gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
  gtk_widget_show (w);

  adj = GTK_ADJUSTMENT (gtk_adjustment_new (1.00, 0.05, 100.00, 0.05, 0.50, 0.50));
  w = gtk_spin_button_new (adj, 0.0, 2);
  g_signal_connect (adj, "value_changed",
		    G_CALLBACK (zoom_changed),
		    canvas);
  gtk_widget_set_size_request (w, 50, -1);
  gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
  gtk_widget_show (w);

  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
  gtk_widget_show (scrolled_win);

  /* Create the canvas. */
  gtk_widget_set_size_request (canvas, 600, 450);
  setup_canvas (canvas, units, units_name);

  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 1000, 1000);
  g_object_set (canvas,
		"units", units,
		"anchor", GTK_ANCHOR_CENTER,
		NULL);

  gtk_widget_show (canvas);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  return vbox;
}


static gboolean
on_delete_event (GtkWidget *window,
		 GdkEvent  *event,
		 gpointer   unused_data)
{
  gtk_main_quit ();
  return FALSE;
}


int
main (int argc, char *argv[])
{
  GtkWidget *window, *notebook;
  cairo_surface_t *surface;

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 600);
  gtk_widget_show (window);
  g_signal_connect (window, "delete_event", G_CALLBACK (on_delete_event),
		    NULL);

  notebook = gtk_notebook_new ();
  gtk_widget_show (notebook);
  gtk_container_add (GTK_CONTAINER (window), notebook);

  surface = cairo_image_surface_create_from_png ("flower.png");
  flower_width = cairo_image_surface_get_width (surface);
  flower_height = cairo_image_surface_get_height (surface);
  flower_pattern = cairo_pattern_create_for_surface (surface);
  cairo_surface_destroy (surface);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas (GTK_UNIT_PIXEL, "pixels"),
			    gtk_label_new ("Pixels"));

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas (GTK_UNIT_POINTS, "points"),
			    gtk_label_new ("Points"));

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas (GTK_UNIT_INCH, "inches"),
			    gtk_label_new ("Inches"));

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
			    create_canvas (GTK_UNIT_MM, "millimeters"),
			    gtk_label_new ("Millimeters"));

  gtk_main ();

  return 0;
}


