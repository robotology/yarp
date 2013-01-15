#include <config.h>
#include <stdlib.h>
#include <goocanvas.h>
#include "demo-large-line.h"
#include "demo-large-rect.h"

/*
 * We need to make sure that that (MAX_ZOOM * canvas width) < 2^31 so we stay
 * within the GDK window size limit (a signed 32-bit int).
 *
 * The canvas width is 107374182 * 2 = 214748364.
 * Multiply by MAX_ZOOM (10) = 2147483640.
 * 2^31 = 2147483648 so we are just inside the GDK limit.
 */
#if 1
#define CANVAS_TOP	-107374182
#define CANVAS_BOTTOM	107374182
#define CANVAS_LEFT	-107374182
#define CANVAS_RIGHT    107374182
#else
#define CANVAS_TOP	0
#define CANVAS_BOTTOM	10000
#define CANVAS_LEFT	0
#define CANVAS_RIGHT	10000
#endif

#define MAX_ZOOM	10

static gboolean
on_motion_notify (GooCanvasItem *item,
		  GooCanvasItem *target,
		  GdkEventMotion *event,
		  gpointer data)
{
  GooCanvasItem *ancestor = target;
  gchar *id;

  while (ancestor)
    {
      id = g_object_get_data (G_OBJECT (ancestor), "id");
      if (id)
	{
	  g_print ("%s item received 'motion-notify' signal\n", id);
	  return TRUE;
	}

      ancestor = goo_canvas_item_get_parent (ancestor);
    }

  return TRUE;
}


static void
zoom_changed (GtkAdjustment *adj, GooCanvas *canvas)
{
  goo_canvas_set_scale (canvas, adj->value);
}


GtkWidget *
create_large_items_page (void)
{
  GtkWidget *vbox, *hbox, *w, *scrolled_win, *canvas;
  GtkAdjustment *adj;
  GooCanvasItem *root, *item;
  gchar *text;

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);

  text = g_strdup_printf ("This is a very large canvas, from (%i, %i) to (%i, %i).\nThe maximum zoom is %i. At this scale the canvas comes very close to the GDK window size limit (32-bit gint).\nThe items have been specially written to work around cairo's limits (transformed coords must be < +/- 32768).",
			  CANVAS_LEFT, CANVAS_TOP,
			  CANVAS_RIGHT, CANVAS_BOTTOM, MAX_ZOOM);
  w = gtk_label_new (text);
  g_free (text);
  gtk_box_pack_start (GTK_BOX (vbox), w, FALSE, FALSE, 0);
  gtk_widget_show (w);

  hbox = gtk_hbox_new (FALSE, 4);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
				       GTK_SHADOW_IN);
  gtk_widget_show (scrolled_win);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

  /* Create the canvas. */
  canvas = goo_canvas_new ();
  gtk_widget_set_size_request (canvas, 600, 450);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

  g_signal_connect (root, "motion_notify_event",
		    G_CALLBACK (on_motion_notify), NULL);

#if 1
  item = goo_demo_large_rect_new (root, CANVAS_LEFT + 10, CANVAS_TOP + 10,
				  CANVAS_RIGHT - CANVAS_LEFT - 20,
				  CANVAS_BOTTOM - CANVAS_TOP - 20,
				  "fill-color", "yellow",
				  NULL);
  g_object_set_data (G_OBJECT (item), "id", "Large Yellow Rect");
#endif

#if 1
  item = goo_demo_large_rect_new (root, CANVAS_LEFT + 100, CANVAS_TOP + 100,
				  CANVAS_RIGHT - CANVAS_LEFT - 200, 50,
				  "fill-color", "orange",
				  NULL);
  g_object_set_data (G_OBJECT (item), "id", "Large Orange Rect");
#endif

#if 1
  item = goo_demo_large_line_new (root, CANVAS_LEFT + 100, CANVAS_TOP + 200,
				  CANVAS_RIGHT - 100, CANVAS_TOP + 200,
				  "stroke-color", "purple",
				  "line-width", 10.0,
				  NULL);
  g_object_set_data (G_OBJECT (item), "id", "Large Purple Line");
#endif

#if 1
  item = goo_demo_large_line_new (root, CANVAS_LEFT + 100, CANVAS_TOP + 300,
				  CANVAS_RIGHT - 100, CANVAS_BOTTOM - 100,
				  "stroke-color", "blue",
				  "line-width", 10.0,
				  NULL);
  g_object_set_data (G_OBJECT (item), "id", "Large Blue Line");
  /*goo_canvas_item_rotate (item, 10, CANVAS_LEFT + 100, CANVAS_TOP + 200);*/
#endif

  goo_canvas_set_bounds (GOO_CANVAS (canvas), CANVAS_LEFT, CANVAS_TOP,
			 CANVAS_RIGHT, CANVAS_BOTTOM);
  gtk_widget_show (canvas);

  /* Zoom */
  w = gtk_label_new ("Zoom:");
  gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
  gtk_widget_show (w);

  adj = GTK_ADJUSTMENT (gtk_adjustment_new (1.00, 0.05, 10.00, 0.05, 0.50, 0.50));
  w = gtk_spin_button_new (adj, 0.0, 2);
  g_signal_connect (adj, "value_changed",
		    G_CALLBACK (zoom_changed),
		    canvas);
  gtk_widget_set_size_request (w, 50, -1);
  gtk_box_pack_start (GTK_BOX (hbox), w, FALSE, FALSE, 0);
  gtk_widget_show (w);

  return vbox;
}
