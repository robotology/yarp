#include <stdlib.h>
#include <goocanvas.h>

typedef enum {
  MODE_MOVE,
  MODE_RESIZE
} Mode;

Mode             drag_mode;
GooCanvasItem   *drag_item = NULL;
gdouble          drag_x = 0.0;
gdouble          drag_y = 0.0;

gdouble          item_x = 0.0;
gdouble          item_y = 0.0;
gdouble          item_width = 0.0;
gdouble          item_height = 0.0;

static gboolean
on_button_press_event_cb (GooCanvasItem *item,
                          GooCanvasItem *target_item,
                          GdkEventButton *event,
                          gpointer user_data)
{
  GooCanvasItemModel *model = goo_canvas_item_get_model (item);

  if (event->state & GDK_CONTROL_MASK)
  {
    if (event->button == 1 || event->button == 3)
    {
      if (event->button == 1)
        drag_mode = MODE_MOVE;
      else
        drag_mode = MODE_RESIZE;

      drag_item = item;
      drag_x = event->x;
      drag_y = event->y;

      g_object_get (G_OBJECT (model),
                    "x", &item_x,
                    "y", &item_y,
                    "width", &item_width,
                    "height", &item_height,
                    NULL);

      goo_canvas_pointer_grab (GOO_CANVAS (user_data), item, GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK, NULL, event->time);
      return TRUE;
    }
  }

  return FALSE;
}

static gboolean
on_button_release_event_cb (GooCanvasItem  *item,
                            GooCanvasItem  *target_item,
                            GdkEventButton *event,
                            gpointer user_data)
{
  if (drag_item == item && drag_item != NULL)
  {
    goo_canvas_pointer_ungrab (GOO_CANVAS (user_data), drag_item, event->time);
    drag_item = NULL;
    return TRUE;
  }

  return FALSE;
}

static gboolean
on_motion_notify_event_cb (GooCanvasItem  *item,
                           GooCanvasItem  *target_item,
                           GdkEventMotion *event,
                           gpointer user_data)
{
  GooCanvasItemModel *model = goo_canvas_item_get_model (item);

  if (drag_item == item && drag_item != NULL)
  {
    gdouble rel_x = event->x - drag_x;
    gdouble rel_y = event->y - drag_y;

    if (drag_mode == MODE_MOVE)
    {
      g_object_set (G_OBJECT (model), "x", item_x + rel_x, "y", item_y + rel_y, NULL);
    }
    else
    {
      gdouble new_width = MAX (item_width + rel_x, 5.0);
      gdouble new_height = MAX (item_height + rel_y, 5.0);

      g_object_set (G_OBJECT (model), "width", new_width, "height", new_height, NULL);
    }

    return TRUE;
  }

  return FALSE;
}


static void
on_item_created (GooCanvas          *canvas,
		 GooCanvasItem      *item,
		 GooCanvasItemModel *model,
		 gpointer            data)
{
  if (g_object_get_data (G_OBJECT (model), "setup-dnd-signals"))
    {
      g_signal_connect (G_OBJECT (item), "button-press-event", G_CALLBACK (on_button_press_event_cb), canvas);
      g_signal_connect (G_OBJECT (item), "button-release-event", G_CALLBACK (on_button_release_event_cb), canvas);
      /*  g_signal_connect (G_OBJECT (item), "grab-broken-event", G_CALLBACK (on_button_release_event_cb), canvas);*/
      g_signal_connect (G_OBJECT (item), "motion-notify-event", G_CALLBACK (on_motion_notify_event_cb), canvas);
    }
}


static GooCanvasItemModel*
create_model (GdkPixbuf *pixbuf)
{
  GooCanvasItemModel *root;
  GooCanvasItemModel *item;
  GooCanvasItemModel* child;

  root = goo_canvas_group_model_new (NULL, NULL);

  /* Test clipping of GooCanvasGroup: We put the rectangle and the ellipse into
   * a group with width=200 and height=200. */
  item = goo_canvas_group_model_new (root, "x", 50.0, "y", 350.0, "width", 200.0, "height", 200.0, NULL);
  /*goo_canvas_item_model_rotate(item, 45.0, 150.0, 450.0);*/

  child = goo_canvas_rect_model_new (item, 0.0, 0.0, 100, 100, "fill-color", "blue", NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");
  goo_canvas_item_model_rotate(child, 45.0, 50.0, 50.0);

  child = goo_canvas_ellipse_model_new (item, 150, 00, 50, 50, "fill-color", "red", NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");

  item = goo_canvas_polyline_model_new (root, FALSE, 5.0, 250.0, 350.0, 275.0, 400.0, 300.0, 350.0, 325.0, 400.0, 350.0, 350.0, "stroke-color", "cyan", "line-width", 5.0, NULL);
  g_object_set_data (G_OBJECT (item), "setup-dnd-signals", "TRUE");

  item = goo_canvas_path_model_new (root, "M20,500 C20,450 100,450 100,500", "stroke-color", "green", "line-width", 5.0, NULL);
  g_object_set_data (G_OBJECT (item), "setup-dnd-signals", "TRUE");

  item = goo_canvas_image_model_new (root, pixbuf, 150, 450, /*"fill-color", "yellow", */NULL);
  g_object_unref (pixbuf);
  g_object_set_data (G_OBJECT (item), "setup-dnd-signals", "TRUE");

  item = goo_canvas_text_model_new (root, "Hello, World!", 250, 450, -1, GTK_ANCHOR_NW, "fill-color", "magenta", "wrap", PANGO_WRAP_WORD_CHAR, NULL);
  g_object_set_data (G_OBJECT (item), "setup-dnd-signals", "TRUE");

  item = goo_canvas_table_model_new (root, "horz-grid-line-width", 2.0, "vert-grid-line-width", 2.0, "row-spacing", 2.0, "column-spacing", 2.0, NULL);
  goo_canvas_item_model_translate (item, 10.0, 10.0);
  g_object_set_data (G_OBJECT (item), "setup-dnd-signals", "TRUE");

  child = goo_canvas_rect_model_new (item, 10.0, 10.0, 50.0, 50.0, "fill-color", "blue", "x", 10.0, "y", 25.0, NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");
  goo_canvas_item_model_set_child_properties (item, child, "column", 0, "row", 0, "columns", 1, "rows", 1, NULL);
  /*goo_canvas_item_model_translate (child, 10.0, 10.0);*/

  child = goo_canvas_rect_model_new (item, 0.0, 0.0, 50.0, 50.0, "fill-color", "red", NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");
  goo_canvas_item_model_set_child_properties (item, child, "column", 1, "row", 0, "columns", 1, "rows", 1, NULL);

  child = goo_canvas_rect_model_new (item, 0.0, 0.0, 50.0, 50.0, "fill-color", "green", NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");
  goo_canvas_item_model_set_child_properties(item, child, "column", 0, "row", 1, "columns", 1, "rows", 1, NULL);

  child = goo_canvas_rect_model_new (item, 0.0, 0.0, 50.0, 50.0, "fill-color", "yellow", NULL);
  g_object_set_data (G_OBJECT (child), "setup-dnd-signals", "TRUE");
  goo_canvas_item_model_set_child_properties (item, child, "column", 1, "row", 1, "columns", 1, "rows", 1, NULL);

  return root;
}


static GtkWidget*
create_window (GooCanvasItemModel *model)
{
  GtkWidget *window, *vbox, *label, *scrolled_win, *canvas;

  /* Create the window and widgets. */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 640, 600);
  gtk_widget_show (window);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  label = gtk_label_new ("Use Ctrl+Left Click to move items or Ctrl+Right Click to resize items");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.0);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  /* Create top canvas. */
  scrolled_win = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
                                       GTK_SHADOW_IN);
  gtk_widget_show (scrolled_win);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

  canvas = goo_canvas_new ();
  g_object_set (G_OBJECT (canvas), "integer-layout", TRUE, NULL);
/*  gtk_widget_set_size_request (canvas, 600, 250);*/
  goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, 1000, 1000);
  gtk_widget_show (canvas);
  gtk_container_add (GTK_CONTAINER (scrolled_win), canvas);

  g_signal_connect (canvas, "item_created",
		    G_CALLBACK (on_item_created), NULL);

  goo_canvas_set_root_item_model (GOO_CANVAS (canvas), model);

  return window;
}


int
main (int argc, char *argv[])
{
  GtkWidget *window;
  GooCanvasItemModel *model;
  GdkPixbuf *pixbuf;

  /* Initialize GTK+. */
  gtk_set_locale ();
  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  pixbuf = gtk_widget_render_icon (window, GTK_STOCK_DIALOG_WARNING,
				   GTK_ICON_SIZE_DIALOG, NULL);

  model = create_model (pixbuf);

  /* Create 2 windows to show off multiple views. */
  window = create_window (model);
#if 1
  window = create_window (model);
#endif

  g_object_unref (model);

  gtk_main ();

  return 0;
}
