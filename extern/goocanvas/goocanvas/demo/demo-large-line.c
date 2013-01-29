/*
 * GooCanvas Demo. Copyright (C) 2007 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * demo-large-line.c - a demo item that exceeds the cairo 16-bit size limit.
 *                     Note that it doesn't support miters.
 */
#include <config.h>
#include "goocanvas.h"
#include "demo-large-line.h"


/* Use the GLib convenience macro to define the type. GooDemoLargeLine is the
   class struct, goo_demo_large_line is the function prefix, and our class is a
   subclass of GOO_TYPE_CANVAS_ITEM_SIMPLE. */
G_DEFINE_TYPE (GooDemoLargeLine, goo_demo_large_line,
	       GOO_TYPE_CANVAS_ITEM_SIMPLE)


/* The standard object initialization function. */
static void
goo_demo_large_line_init (GooDemoLargeLine *demo_large_line)
{
  demo_large_line->x1 = 0.0;
  demo_large_line->y1 = 0.0;
  demo_large_line->x2 = 0.0;
  demo_large_line->y2 = 0.0;
}


/* The convenience function to create new items. This should start with a 
   parent argument and end with a variable list of object properties to fit
   in with the standard canvas items. */
GooCanvasItem*
goo_demo_large_line_new (GooCanvasItem      *parent,
			 gdouble             x1,
			 gdouble             y1,
			 gdouble             x2,
			 gdouble             y2,
			 ...)
{
  GooCanvasItem *item;
  GooDemoLargeLine *demo_large_line;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_DEMO_LARGE_LINE, NULL);

  demo_large_line = (GooDemoLargeLine*) item;
  demo_large_line->x1 = x1;
  demo_large_line->y1 = y1;
  demo_large_line->x2 = x2;
  demo_large_line->y2 = y2;

  va_start (var_args, y2);
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
goo_demo_large_line_update  (GooCanvasItemSimple *simple,
			     cairo_t             *cr)
{
  GooDemoLargeLine *item = (GooDemoLargeLine*) simple;
  gdouble half_line_width;

  half_line_width = goo_canvas_item_simple_get_line_width (simple) / 2;

  /* Compute the new bounds. */
  simple->bounds.x1 = MIN (item->x1, item->x2) - half_line_width;
  simple->bounds.y1 = MIN (item->y1, item->y2) - half_line_width;
  simple->bounds.x2 = MAX (item->x1, item->x2) + half_line_width;
  simple->bounds.y2 = MAX (item->y1, item->y2) + half_line_width;
}


static void
clamp_x (gdouble point1[2],
	 gdouble point2[2],
	 gdouble clamp)

{
  gdouble fraction = (clamp - point1[0]) / (point2[0] - point1[0]);
  point1[0] = clamp;
  point1[1] += fraction * (point2[1] - point1[1]);
}


static void
clamp_y (gdouble point1[2],
	 gdouble point2[2],
	 gdouble clamp)

{
  gdouble fraction = (clamp - point1[1]) / (point2[1] - point1[1]);
  point1[1] = clamp;
  point1[0] += fraction * (point2[0] - point1[0]);
}


static void
paint_large_line (GooDemoLargeLine      *line,
		  cairo_t               *cr,
		  const GooCanvasBounds *bounds,
		  gdouble                line_width,
		  gdouble                x1,
		  gdouble                y1,
		  gdouble                x2,
		  gdouble                y2)
{
  GooCanvasItem *item = (GooCanvasItem*) line;
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) line;
  GooCanvas *canvas = simple->canvas;
  GooCanvasBounds tmp_bounds = *bounds;
  gdouble point1[2], point2[2], *p1, *p2;

  point1[0] = x1;
  point1[1] = y1;
  point2[0] = x2;
  point2[1] = y2;

  /* Transform the coordinates to the canvas device space, so we can clamp
     the line to the bounds to be painted. */
  goo_canvas_convert_from_item_space (canvas, item, &point1[0], &point1[1]);
  goo_canvas_convert_from_item_space (canvas, item, &point2[0], &point2[1]);

  /* Extend the bounds a bit to account for the line width. */
  tmp_bounds.x1 -= line_width;
  tmp_bounds.y1 -= line_width;
  tmp_bounds.x2 += line_width;
  tmp_bounds.y2 += line_width;

  /* Make p1 the left-most point. */
  if (point1[0] < point2[0])
    {
      p1 = point1;
      p2 = point2;
    }
  else
    {
      p1 = point2;
      p2 = point1;
    }

  /* Just return if the line is completely outside the bounds horizontally. */
  if (p2[0] < tmp_bounds.x1 || p1[0] > tmp_bounds.x2)
    return;

  /* Clamp each x coordinate to the bounds. */
  if (p1[0] < tmp_bounds.x1)
    clamp_x (p1, p2, tmp_bounds.x1);
  if (p2[0] > tmp_bounds.x2)
    clamp_x (p2, p1, tmp_bounds.x2);

  /* Now make p1 the top-most point. */
  if (point1[1] < point2[1])
    {
      p1 = point1;
      p2 = point2;
    }
  else
    {
      p1 = point2;
      p2 = point1;
    }

  /* Just return if the line is completely outside the bounds vertically. */
  if (p2[1] < tmp_bounds.y1 || p1[1] > tmp_bounds.y2)
    return;

  /* Clamp each y coordinate to the bounds. */
  if (p1[1] < tmp_bounds.y1)
    clamp_y (p1, p2, tmp_bounds.y1);
  if (p2[1] > tmp_bounds.y2)
    clamp_y (p2, p1, tmp_bounds.y2);

  /* Convert back to item space. */
  goo_canvas_convert_to_item_space (canvas, item, &point1[0], &point1[1]);
  goo_canvas_convert_to_item_space (canvas, item, &point2[0], &point2[1]);

  /* Draw the line. */
  cairo_move_to (cr, point1[0], point1[1]);
  cairo_line_to (cr, point2[0], point2[1]);
  cairo_stroke (cr);
}


/* The paint method. This should draw the item on the given cairo_t, using
   the item's own coordinate space. */
static void
goo_demo_large_line_paint (GooCanvasItemSimple   *simple,
			   cairo_t               *cr,
			   const GooCanvasBounds *bounds)
{
  GooDemoLargeLine *item = (GooDemoLargeLine*) simple;
  gdouble line_width;

  goo_canvas_style_set_stroke_options (simple->simple_data->style, cr);
  line_width = goo_canvas_item_simple_get_line_width (simple);
  paint_large_line (item, cr, bounds, line_width,
		    item->x1, item->y1, item->x2, item->y2);
}


/* Hit detection. This should check if the given coordinate (in the item's
   coordinate space) is within the item. If it is it should return TRUE,
   otherwise it should return FALSE. */
static gboolean
goo_demo_large_line_is_item_at (GooCanvasItemSimple *simple,
				gdouble              x,
				gdouble              y,
				cairo_t             *cr,
				gboolean             is_pointer_event)
{
  /*GooDemoLargeLine *item = (GooDemoLargeLine*) simple;*/

  /* FIXME: Implement this. */

  return FALSE;
}


/* The class initialization function. Here we set the class' update(), paint()
   and is_item_at() methods. */
static void
goo_demo_large_line_class_init (GooDemoLargeLineClass *klass)
{
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  simple_class->simple_update        = goo_demo_large_line_update;
  simple_class->simple_paint         = goo_demo_large_line_paint;
  simple_class->simple_is_item_at    = goo_demo_large_line_is_item_at;
}


