/*
 * GooCanvas Demo. Copyright (C) 2007 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * demo-large-rect.c - a demo item that exceeds the cairo 16-bit size limit.
 *                     Note that it doesn't support rotations or shears.
 */
#include <config.h>
#include "goocanvas.h"
#include "demo-large-rect.h"


/* Use the GLib convenience macro to define the type. GooDemoLargeRect is the
   class struct, goo_demo_large_rect is the function prefix, and our class is a
   subclass of GOO_TYPE_CANVAS_ITEM_SIMPLE. */
G_DEFINE_TYPE (GooDemoLargeRect, goo_demo_large_rect,
	       GOO_TYPE_CANVAS_ITEM_SIMPLE)


/* The standard object initialization function. */
static void
goo_demo_large_rect_init (GooDemoLargeRect *demo_large_rect)
{
  demo_large_rect->x = 0.0;
  demo_large_rect->y = 0.0;
  demo_large_rect->width = 0.0;
  demo_large_rect->height = 0.0;
}


/* The convenience function to create new items. This should start with a 
   parent argument and end with a variable list of object properties to fit
   in with the standard canvas items. */
GooCanvasItem*
goo_demo_large_rect_new (GooCanvasItem      *parent,
			 gdouble             x,
			 gdouble             y,
			 gdouble             width,
			 gdouble             height,
			 ...)
{
  GooCanvasItem *item;
  GooDemoLargeRect *demo_large_rect;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_DEMO_LARGE_RECT, NULL);

  demo_large_rect = (GooDemoLargeRect*) item;
  demo_large_rect->x = x;
  demo_large_rect->y = y;
  demo_large_rect->width = width;
  demo_large_rect->height = height;

  va_start (var_args, height);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist ((GObject*) item, first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


/* The update method. This is called when the canvas is initially shown and
   also whenever the object is updated and needs to change its size and/or
   shape. It should calculate its new bounds in its own coordinate space,
   storing them in simple->bounds. */
static void
goo_demo_large_rect_update  (GooCanvasItemSimple *simple,
			     cairo_t             *cr)
{
  GooDemoLargeRect *item = (GooDemoLargeRect*) simple;
  gdouble half_line_width;

  /* We can quickly compute the bounds as being just the rectangle's size
     plus half the line width around each edge. */
  half_line_width = goo_canvas_item_simple_get_line_width (simple) / 2;

  simple->bounds.x1 = item->x - half_line_width;
  simple->bounds.y1 = item->y - half_line_width;
  simple->bounds.x2 = item->x + item->width + half_line_width;
  simple->bounds.y2 = item->y + item->height + half_line_width;
}


static void
create_large_rect_path (GooDemoLargeRect      *rect,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                line_width,
			gdouble                x,
			gdouble                y,
			gdouble                width,
			gdouble                height)
{
  GooCanvasItem *item = (GooCanvasItem*) rect;
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) rect;
  GooCanvas *canvas = simple->canvas;
  GooCanvasBounds tmp_bounds = *bounds;
  gdouble x1 = x, y1 = y, x2 = x + width, y2 = y + height;

  /* Transform the coordinates to the canvas device space, so we can clamp
     the line to the bounds to be painted. */
  goo_canvas_convert_from_item_space (canvas, item, &x1, &y1);
  goo_canvas_convert_from_item_space (canvas, item, &x2, &y2);

  /* Extend the bounds a bit to account for the line width, so when we clamp
     the rect to the bounds the outside edges aren't visible. */
  tmp_bounds.x1 -= line_width;
  tmp_bounds.y1 -= line_width;
  tmp_bounds.x2 += line_width;
  tmp_bounds.y2 += line_width;

  /* If the rect is completely outside the bounds just return. */
  if (x1 > tmp_bounds.x2 || x2 < tmp_bounds.x1
      || y1 > tmp_bounds.y2 || y2 < tmp_bounds.y1)
    return;

  /* Clamp the rect to the bounds. */
  if (x1 < tmp_bounds.x1)
    x1 = tmp_bounds.x1;
  if (x2 > tmp_bounds.x2)
    x2 = tmp_bounds.x2;

  if (y1 < tmp_bounds.y1)
    y1 = tmp_bounds.y1;
  if (y2 > tmp_bounds.y2)
    y2 = tmp_bounds.y2;

  /* Convert back to item space. */
  goo_canvas_convert_to_item_space (canvas, item, &x1, &y1);
  goo_canvas_convert_to_item_space (canvas, item, &x2, &y2);

  /* Create the path. */
  cairo_move_to (cr, x1, y1);
  cairo_line_to (cr, x2, y1);
  cairo_line_to (cr, x2, y2);
  cairo_line_to (cr, x1, y2);
  cairo_close_path (cr);
}


/* The paint method. This should draw the item on the given cairo_t, using
   the item's own coordinate space. */
static void
goo_demo_large_rect_paint (GooCanvasItemSimple   *simple,
			   cairo_t               *cr,
			   const GooCanvasBounds *bounds)
{
  GooDemoLargeRect *item = (GooDemoLargeRect*) simple;
  gdouble line_width;

  line_width = goo_canvas_item_simple_get_line_width (simple);
  create_large_rect_path (item, cr, bounds, line_width,
			  item->x, item->y, item->width, item->height);
  goo_canvas_item_simple_paint_path (simple, cr);
}


/* Hit detection. This should check if the given coordinate (in the item's
   coordinate space) is within the item. If it is it should return TRUE,
   otherwise it should return FALSE. */
static gboolean
goo_demo_large_rect_is_item_at (GooCanvasItemSimple *simple,
				gdouble              x,
				gdouble              y,
				cairo_t             *cr,
				gboolean             is_pointer_event)
{
  GooDemoLargeRect *item = (GooDemoLargeRect*) simple;
  gdouble half_line_width;

  /* We assume the item covers its rectangle + line widths. */
  half_line_width = goo_canvas_item_simple_get_line_width (simple) / 2;

  if ((x < item->x - half_line_width)
      || (x > item->x + item->width + half_line_width)
      || (y < item->y - half_line_width)
      || (y > item->y + item->height + half_line_width))
    return FALSE;

  return TRUE;
}


/* The class initialization function. Here we set the class' update(), paint()
   and is_item_at() methods. */
static void
goo_demo_large_rect_class_init (GooDemoLargeRectClass *klass)
{
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  simple_class->simple_update        = goo_demo_large_rect_update;
  simple_class->simple_paint         = goo_demo_large_rect_paint;
  simple_class->simple_is_item_at    = goo_demo_large_rect_is_item_at;
}


