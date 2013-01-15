#include <stdlib.h>
#include <goocanvas.h>


static gboolean
on_delete_event (GtkWidget *window,
		 GdkEvent  *event,
		 gpointer   unused_data)
{
  exit (0);
}


static GooCanvasItem*
create_item (GooCanvasItem *table,
	     gdouble        width,
	     gdouble        height,
	     gint           row,
	     gint           column,
	     gint           rows,
	     gint           columns,
	     gboolean       x_expand,
	     gboolean       x_shrink,
	     gboolean       x_fill,
	     gboolean       y_expand,
	     gboolean       y_shrink,
	     gboolean       y_fill)
{
  GooCanvasItem *item;

  item = goo_canvas_rect_new (table, 0, 0, width, height,
			      "stroke-color", "black",
			      "fill-color", "red",
			      NULL);

  goo_canvas_item_set_child_properties (table, item,
					"row", row,
					"column", column,
					"rows", rows,
					"columns", columns,
					"x-expand", x_expand,
					"x-fill", x_fill,
					"x-shrink", x_shrink,
					"y-expand", y_expand,
					"y-fill", y_fill,
					"y-shrink", y_shrink,
					NULL);

  return item;
}


void
create_table1 (GooCanvasItem *parent,
	       gdouble        x,
	       gdouble        y,
	       gdouble        width,
	       gdouble        height,
	       gboolean       with_grid_lines)
{
  GooCanvasItem *table, *items[9];
  GooCanvasBounds bounds;
  gint i = 0;

  table = goo_canvas_table_new (parent,
				"width", width,
				"height", height,
				NULL);
  goo_canvas_item_translate (table, x, y);

  if (with_grid_lines)
    {
      g_object_set (G_OBJECT (table),
		    "row-spacing",  2.0,
		    "column-spacing", 2.0,
		    "x-border-spacing", 1.0,
		    "y-border-spacing", 1.0,
		    "horz-grid-line-width", 1.0,
		    "vert-grid-line-width", 2.0,
		    "stroke-color", "blue",
		    "fill-color", "lightblue",
		    NULL);
    }

  items[i++] = create_item (table, 17.3, 12.9, 0, 0, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 33.1, 17.2, 1, 0, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 41.6, 23.9, 2, 0, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  items[i++] = create_item (table, 7.1, 5.7, 0, 1, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 13.5, 18.2, 1, 1, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 25.2, 22.1, 2, 1, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  items[i++] = create_item (table, 11.3, 11.7, 0, 2, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 21.7, 18.8, 1, 2, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 22.2, 13.8, 2, 2, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  for (i = 0; i < 9; i++)
    {
      goo_canvas_item_get_bounds (items[i], &bounds);
      g_print ("Item %i: %g,%g - %g,%g\n", i,
	       bounds.x1 - x, bounds.y1 - y,
	       bounds.x2 - x, bounds.y2 - y);
    }
}

/* Creates a table with items spanning multiple cells */
void
create_table2 (GooCanvasItem *parent,
	       gdouble        x,
	       gdouble        y,
	       gdouble        width,
	       gdouble        height,
	       gboolean       with_grid_lines)
{
  GooCanvasItem *table, *items[9];
  GooCanvasBounds bounds;
  gint i = 0;

  table = goo_canvas_table_new (parent,
				"width", width,
				"height", height,
				NULL);
  goo_canvas_item_translate (table, x, y);

  if (with_grid_lines)
    {
      g_object_set (G_OBJECT (table),
		    "row-spacing",  2.0,
		    "column-spacing", 2.0,
		    "x-border-spacing", 1.0,
		    "y-border-spacing", 1.0,
		    "horz-grid-line-width", 1.0,
		    "vert-grid-line-width", 2.0,
		    NULL);
    }

  items[i++] = create_item (table, 17.3, 12.9, 0, 0, 2, 2,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 33.1, 17.2, 0, 2, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 41.6, 23.9, 1, 2, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  items[i++] = create_item (table, 7.1, 5.7, 0, 3, 2, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 13.5, 18.2, 2, 0, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 25.2, 22.1, 2, 1, 1, 3,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  items[i++] = create_item (table, 11.3, 11.7, 3, 0, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 21.7, 18.8, 3, 1, 1, 1,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
  items[i++] = create_item (table, 22.2, 13.8, 3, 2, 1, 2,
			    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);

  for (i = 0; i < 9; i++)
    {
      goo_canvas_item_get_bounds (items[i], &bounds);
      g_print ("Item %i: %g,%g - %g,%g\n", i,
	       bounds.x1 - x, bounds.y1 - y,
	       bounds.x2 - x, bounds.y2 - y);
    }
}

void
setup_canvas (GtkWidget *canvas)
{
  GooCanvasItem *root;

  root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

  g_print ("\nTable at default size...\n");
  create_table1 (root, 50, 50, -1, -1, FALSE);

  g_print ("\nTable at reduced size...\n");
  create_table1 (root, 250, 50, 30, 30, FALSE);

  g_print ("\nTable at enlarged size...\n");
  create_table1 (root, 450, 50, 100, 100, FALSE);

  g_print ("\nTable with grid lines at default size...\n");
  create_table1 (root, 50, 250, -1, -1, TRUE);

  g_print ("\nTable with grid lines at reduced size...\n");
  create_table1 (root, 250, 250, 30, 30, TRUE);

  g_print ("\nTable with grid lines at enlarged size...\n");
  create_table1 (root, 450, 250, 150, 150, TRUE);

  g_print ("Multispanning table with grid lines at default size...\n");
  create_table2 (root, 50, 450, -1, -1, TRUE);

  g_print ("Multispanning table with grid lines at reduced size...\n");
  create_table2 (root, 250, 450, 30, 30, TRUE);

  g_print ("Multispanning table with grid lines at enlarged size...\n");
  create_table2 (root, 450, 450, 150, 150, TRUE);
}


int
main (int argc, char *argv[])
{
  GtkWidget *window, *vbox, *label, *scrolled_win, *canvas;

  /* Initialize GTK+. */
  gtk_set_locale ();
  gtk_init (&argc, &argv);

  /* Create the window and widgets. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 600);
  gtk_widget_show (window);
  g_signal_connect (window, "delete_event", G_CALLBACK (on_delete_event),
		    NULL);

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  label = gtk_label_new ("Normal Layout");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  /* Create top canvas. */
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
				       GTK_SHADOW_IN);
  gtk_widget_show (scrolled_win);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, FALSE, FALSE, 0);

  canvas = goo_canvas_new ();
  gtk_widget_set_size_request (canvas, 600, 250);
  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 1000, 1000);
  gtk_widget_show (canvas);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  g_print ("\n\nNormal Canvas...\n");
  setup_canvas (canvas);

  label = gtk_label_new ("Integer Layout");
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  /* Create bottom canvas. */
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
				       GTK_SHADOW_IN);
  gtk_widget_show (scrolled_win);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, FALSE, FALSE, 0);

  canvas = goo_canvas_new ();
  g_object_set (canvas,
		"integer-layout", TRUE,
		NULL);
  gtk_widget_set_size_request (canvas, 600, 250);
  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 1000, 1000);
  gtk_widget_show (canvas);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  g_print ("\n\nInteger Layout Canvas...\n");
  setup_canvas (canvas);

  gtk_main ();

  return 0;
}


