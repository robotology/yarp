/*
 * GooCanvas. Copyright (C) 2005-6 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvaspath.c - a path item, very similar to the SVG path element.
 */

/**
 * SECTION:goocanvaspath
 * @Title: GooCanvasPath
 * @Short_Description: a path item (a series of lines and curves).
 *
 * GooCanvasPath represents a path item, which is a series of one or more
 * lines, bezier curves, or elliptical arcs.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * #GooCanvasPath uses the same path specification strings as the Scalable
 * Vector Graphics (SVG) path element. For details see the
 * <ulink url="http://www.w3.org/Graphics/SVG/">SVG specification</ulink>.
 *
 * To create a #GooCanvasPath use goo_canvas_path_new().
 *
 * To get or set the properties of an existing #GooCanvasPath, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvaspath.h"
#include "goocanvas.h"


enum {
  PROP_0,

  PROP_DATA,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT
};

static void canvas_item_interface_init   (GooCanvasItemIface  *iface);

G_DEFINE_TYPE_WITH_CODE (GooCanvasPath, goo_canvas_path,
			 GOO_TYPE_CANVAS_ITEM_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM,
						canvas_item_interface_init))


static void
goo_canvas_path_install_common_properties (GObjectClass *gobject_class)
{
  /**
   * GooCanvasPath:data
   *
   * The sequence of path commands, specified as a string using the same syntax
   * as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable Vector
   * Graphics (SVG)</ulink> path element.
   */
  g_object_class_install_property (gobject_class, PROP_DATA,
				   g_param_spec_string ("data",
							_("Path Data"),
							_("The sequence of path commands"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the path"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the path"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the path"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the path"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));
}


static void
goo_canvas_path_init (GooCanvasPath *path)
{
  path->path_data = g_slice_new0 (GooCanvasPathData);
}


/**
 * goo_canvas_path_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @path_data: the sequence of path commands, specified as a string using the
 *  same syntax as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable
 *  Vector Graphics (SVG)</ulink> path element.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new path item.
 * 
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a red line from (20,20) to (40,40):
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M 20 20 L 40 40",
 *                                             "stroke-color", "red",
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * This example creates a cubic bezier curve from (20,100) to (100,100) with
 * the control points at (20,50) and (100,50):
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M20,100 C20,50 100,50 100,100",
 *                                             "stroke-color", "blue",
 *                                             NULL);
 * </programlisting></informalexample>
 *
 * This example uses an elliptical arc to create a filled circle with one
 * quarter missing:
 * 
 * <informalexample><programlisting>
 *  GooCanvasItem *path = goo_canvas_path_new (mygroup,
 *                                             "M200,500 h-150 a150,150 0 1,0 150,-150 z",
 *                                             "fill-color", "red",
 *                                             "stroke-color", "blue",
 *                                             "line-width", 5.0,
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new path item.
 **/
GooCanvasItem*
goo_canvas_path_new               (GooCanvasItem *parent,
				   const gchar   *path_data,
				   ...)
{
  GooCanvasItem *item;
  GooCanvasPath *path;
  const char *first_property;
  va_list var_args;

  item = g_object_new (GOO_TYPE_CANVAS_PATH, NULL);
  path = (GooCanvasPath*) item;

  path->path_data->path_commands = goo_canvas_parse_path_data (path_data);

  va_start (var_args, path_data);
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


static void
goo_canvas_path_finalize (GObject *object)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasPath *path = (GooCanvasPath*) object;

  /* Free our data if we didn't have a model. (If we had a model it would
     have been reset in dispose() and simple_data will be NULL.) */
  if (simple->simple_data)
    {
      if (path->path_data->path_commands)
	g_array_free (path->path_data->path_commands, TRUE);
      g_slice_free (GooCanvasPathData, path->path_data);
    }
  path->path_data = NULL;

  G_OBJECT_CLASS (goo_canvas_path_parent_class)->finalize (object);
}

static void
goo_canvas_path_common_get_extent (GooCanvas            *canvas,
                                   GooCanvasPathData    *path_data,
                                   GooCanvasBounds      *bounds)
{
  cairo_t *cr;

  cr = goo_canvas_create_cairo_context (canvas);
  goo_canvas_create_path (path_data->path_commands, cr);
  cairo_fill_extents (cr, &bounds->x1, &bounds->y1, &bounds->x2, &bounds->y2);
  cairo_destroy (cr);
}


/* Moves all the absolute points in the command by the given amounts.
   Relative points don't need to be moved. */
static void
goo_canvas_path_move_command (GooCanvasPathCommand *cmd,
			      gdouble               x_offset,
			      gdouble               y_offset)
{
  switch (cmd->simple.type)
    {
    case GOO_CANVAS_PATH_MOVE_TO:
    case GOO_CANVAS_PATH_CLOSE_PATH:
    case GOO_CANVAS_PATH_LINE_TO:
    case GOO_CANVAS_PATH_HORIZONTAL_LINE_TO:
    case GOO_CANVAS_PATH_VERTICAL_LINE_TO:
      if (!cmd->simple.relative)
        {
          cmd->simple.x += x_offset;
          cmd->simple.y += y_offset;
        }
      break;
    case GOO_CANVAS_PATH_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_CURVE_TO:
    case GOO_CANVAS_PATH_QUADRATIC_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_QUADRATIC_CURVE_TO:
      if (!cmd->curve.relative)
        {
          cmd->curve.x += x_offset;
          cmd->curve.y += y_offset;
          cmd->curve.x1 += x_offset;
          cmd->curve.y1 += y_offset;
          cmd->curve.x2 += x_offset;
          cmd->curve.y2 += y_offset;
        }
      break;
    case GOO_CANVAS_PATH_ELLIPTICAL_ARC:
      if (!cmd->arc.relative)
        {
          cmd->arc.x += x_offset;
          cmd->arc.y += y_offset;
        }
      break;
    default:
      g_assert_not_reached();
      break;
    }
}


/* Scales all the points in the command by the given amounts. Absolute points
   are scaled about the given origin. */
static void
goo_canvas_path_scale_command (GooCanvasPathCommand *cmd,
			       gdouble               x_origin,
			       gdouble               y_origin,
			       gdouble               x_scale,
			       gdouble               y_scale)
{
  switch (cmd->simple.type)
    {
    case GOO_CANVAS_PATH_MOVE_TO:
    case GOO_CANVAS_PATH_CLOSE_PATH:
    case GOO_CANVAS_PATH_LINE_TO:
    case GOO_CANVAS_PATH_HORIZONTAL_LINE_TO:
    case GOO_CANVAS_PATH_VERTICAL_LINE_TO:
      if (cmd->simple.relative)
        {
          cmd->simple.x *= x_scale;
          cmd->simple.y *= y_scale;
        }
      else
        {
          cmd->simple.x = x_origin + (cmd->simple.x - x_origin) * x_scale;
          cmd->simple.y = y_origin + (cmd->simple.y - y_origin) * y_scale;
        }
      break;
    case GOO_CANVAS_PATH_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_CURVE_TO:
    case GOO_CANVAS_PATH_QUADRATIC_CURVE_TO:
    case GOO_CANVAS_PATH_SMOOTH_QUADRATIC_CURVE_TO:
      if (cmd->curve.relative)
        {
          cmd->curve.x *= x_scale;
          cmd->curve.y *= y_scale;
          cmd->curve.x1 *= x_scale;
          cmd->curve.y1 *= y_scale;
          cmd->curve.x2 *= x_scale;
          cmd->curve.y2 *= y_scale;
        }
      else
        {
          cmd->curve.x =  x_origin + (cmd->curve.x -  x_origin) * x_scale;
          cmd->curve.y =  y_origin + (cmd->curve.y -  y_origin) * y_scale;
          cmd->curve.x1 = x_origin + (cmd->curve.x1 - x_origin) * x_scale;
          cmd->curve.y1 = y_origin + (cmd->curve.y1 - y_origin) * y_scale;
          cmd->curve.x2 = x_origin + (cmd->curve.x2 - x_origin) * x_scale;
          cmd->curve.y2 = y_origin + (cmd->curve.y2 - y_origin) * y_scale;
        }
      break;
    case GOO_CANVAS_PATH_ELLIPTICAL_ARC:
      if (cmd->arc.relative)
        {
          cmd->arc.x *= x_scale;
          cmd->arc.y *= y_scale;
        }
      else
        {
          cmd->arc.x = x_origin + (cmd->arc.x - x_origin) * x_scale;
          cmd->arc.y = y_origin + (cmd->arc.y - y_origin) * y_scale;
        }
      break;
    default:
      g_assert_not_reached();
      break;
  }
}

static void
goo_canvas_path_get_common_property (GObject              *object,
                                     GooCanvas            *canvas,
				     GooCanvasPathData    *path_data,
				     guint                 prop_id,
				     GValue               *value,
				     GParamSpec           *pspec)
{
  GooCanvasBounds extent;

  switch (prop_id)
    {
    case PROP_X:
      goo_canvas_path_common_get_extent (canvas, path_data, &extent);
      g_value_set_double (value, extent.x1);
      break;
    case PROP_Y:
      goo_canvas_path_common_get_extent (canvas, path_data, &extent);
      g_value_set_double (value, extent.y1);
      break;
    case PROP_WIDTH:
      goo_canvas_path_common_get_extent (canvas, path_data, &extent);
      g_value_set_double (value, extent.x2 - extent.x1);
      break;
    case PROP_HEIGHT:
      goo_canvas_path_common_get_extent (canvas, path_data, &extent);
      g_value_set_double (value, extent.y2 - extent.y1);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static void
goo_canvas_path_get_property (GObject              *object,
			      guint                 prop_id,
			      GValue               *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasPath *path = (GooCanvasPath*) object;

  goo_canvas_path_get_common_property (object, simple->canvas,
                                       path->path_data, prop_id, value, pspec);
}


static void
goo_canvas_path_set_common_property (GObject              *object,
                                     GooCanvas            *canvas,
				     GooCanvasPathData    *path_data,
				     guint                 prop_id,
				     const GValue         *value,
				     GParamSpec           *pspec)
{
  GooCanvasBounds extent;
  GooCanvasPathCommand *cmd;
  gdouble x_offset, y_offset, x_scale, y_scale;
  guint i;

  switch (prop_id)
    {
    case PROP_DATA:
      if (path_data->path_commands)
	g_array_free (path_data->path_commands, TRUE);
      path_data->path_commands = goo_canvas_parse_path_data (g_value_get_string (value));
      g_object_notify (object, "x");
      g_object_notify (object, "y");
      g_object_notify (object, "width");
      g_object_notify (object, "height");
      break;
    case PROP_X:
      if (path_data->path_commands->len > 0)
        {
	  /* Calculate the x offset from the current position. */
          goo_canvas_path_common_get_extent (canvas, path_data, &extent);
          x_offset = g_value_get_double (value) - extent.x1;

	  /* Add the offset to all the absolute x coordinates. */
          for (i = 0; i < path_data->path_commands->len; i++)
            {
              cmd = &g_array_index (path_data->path_commands,
				    GooCanvasPathCommand, i);
              goo_canvas_path_move_command (cmd, x_offset, 0.0);
            }
          g_object_notify (object, "data");
        }
      break;
    case PROP_Y:
      if (path_data->path_commands->len > 0)
        {
	  /* Calculate the y offset from the current position. */
          goo_canvas_path_common_get_extent (canvas, path_data, &extent);
          y_offset = g_value_get_double (value) - extent.y1;

	  /* Add the offset to all the absolute y coordinates. */
          for (i = 0; i < path_data->path_commands->len; i++)
            {
              cmd = &g_array_index (path_data->path_commands,
				    GooCanvasPathCommand, i);
              goo_canvas_path_move_command (cmd, 0.0, y_offset);
            }
          g_object_notify (object, "data");
        }
      break;
    case PROP_WIDTH:
      if (path_data->path_commands->len >= 2)
        {
          goo_canvas_path_common_get_extent (canvas, path_data, &extent);
          if (extent.x2 - extent.x1 != 0.0)
            {
	      /* Calculate the amount to scale the path. */
              x_scale = g_value_get_double (value) / (extent.x2 - extent.x1);

	      /* Scale the x coordinates, relative to the left-most point. */
              for (i = 0; i < path_data->path_commands->len; i++)
                {
                  cmd = &g_array_index (path_data->path_commands,
					GooCanvasPathCommand, i);
                  goo_canvas_path_scale_command (cmd, extent.x1, 0.0,
						 x_scale, 1.0);
                }
              g_object_notify (object, "data");
            }
        }
      break;
    case PROP_HEIGHT:
      if (path_data->path_commands->len >= 2)
        {
          goo_canvas_path_common_get_extent (canvas, path_data, &extent);
          if (extent.y2 - extent.y1 != 0.0)
            {
	      /* Calculate the amount to scale the polyline. */
              y_scale = g_value_get_double (value) / (extent.y2 - extent.y1);

	      /* Scale the y coordinates, relative to the top-most point. */
              for (i = 0; i < path_data->path_commands->len; i++)
                {
                  cmd = &g_array_index (path_data->path_commands,
					GooCanvasPathCommand, i);
                  goo_canvas_path_scale_command (cmd, 0.0, extent.y1,
						 1.0, y_scale);
                }
              g_object_notify (object, "data");
            }
        }
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_path_set_property (GObject              *object,
			      guint                 prop_id,
			      const GValue         *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasPath *path = (GooCanvasPath*) object;

  if (simple->model)
    {
      g_warning ("Can't set property of a canvas item with a model - set the model property instead");
      return;
    }

  goo_canvas_path_set_common_property (object, simple->canvas, path->path_data,
                                       prop_id, value, pspec);
  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_path_create_path (GooCanvasItemSimple *simple,
			     cairo_t             *cr)
{
  GooCanvasPath *path = (GooCanvasPath*) simple;

  goo_canvas_create_path (path->path_data->path_commands, cr);
}


static gboolean
goo_canvas_path_is_item_at (GooCanvasItemSimple *simple,
			    gdouble              x,
			    gdouble              y,
			    cairo_t             *cr,
			    gboolean             is_pointer_event)
{
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasPointerEvents pointer_events = GOO_CANVAS_EVENTS_ALL;
  gboolean do_fill;

  /* By default only check the fill if a fill color/pattern is specified. */
  do_fill = goo_canvas_style_set_fill_options (simple_data->style, cr);
  if (!do_fill)
    pointer_events &= ~GOO_CANVAS_EVENTS_FILL_MASK;

  /* If is_pointer_event is set use the pointer_events property instead. */
  if (is_pointer_event)
    pointer_events = simple_data->pointer_events;

  goo_canvas_path_create_path (simple, cr);
  if (goo_canvas_item_simple_check_in_path (simple, x, y, cr, pointer_events))
    return TRUE;

  return FALSE;
}


static void
goo_canvas_path_set_model    (GooCanvasItem      *item,
			      GooCanvasItemModel *model)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasPath *path = (GooCanvasPath*) item;
  GooCanvasPathModel *emodel = (GooCanvasPathModel*) model;

  /* If our data was allocated, free it. */
  if (!simple->model)
    {
      if (path->path_data->path_commands)
	g_array_free (path->path_data->path_commands, TRUE);
      g_slice_free (GooCanvasPathData, path->path_data);
    }

  /* Now use the new model's data instead. */
  path->path_data = &emodel->path_data;

  /* Let the parent GooCanvasItemSimple code do the rest. */
  goo_canvas_item_simple_set_model (simple, model);
}


static void
canvas_item_interface_init (GooCanvasItemIface *iface)
{
  iface->set_model      = goo_canvas_path_set_model;
}


static void
goo_canvas_path_class_init (GooCanvasPathClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  gobject_class->finalize     = goo_canvas_path_finalize;

  gobject_class->get_property = goo_canvas_path_get_property;
  gobject_class->set_property = goo_canvas_path_set_property;

  simple_class->simple_create_path = goo_canvas_path_create_path;
  simple_class->simple_is_item_at  = goo_canvas_path_is_item_at;

  goo_canvas_path_install_common_properties (gobject_class);
}


/**
 * SECTION:goocanvaspathmodel
 * @Title: GooCanvasPathModel
 * @Short_Description: a model for path items (a series of lines and curves).
 *
 * GooCanvasPathModel represents a model for path items, which are a series of
 * one or more lines, bezier curves, or elliptical arcs.
 *
 * It is a subclass of #GooCanvasItemModelSimple and so inherits all of the
 * style properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItemModel interface, so you can use the
 * #GooCanvasItemModel functions such as goo_canvas_item_model_raise() and
 * goo_canvas_item_model_rotate().
 *
 * #GooCanvasPathModel uses the same path specification strings as the Scalable
 * Vector Graphics (SVG) path element. For details see the
 * <ulink url="http://www.w3.org/Graphics/SVG/">SVG specification</ulink>.
 *
 * To create a #GooCanvasPathModel use goo_canvas_path_model_new().
 *
 * To get or set the properties of an existing #GooCanvasPathModel, use
 * g_object_get() and g_object_set().
 *
 * To respond to events such as mouse clicks on the path you must connect
 * to the signal handlers of the corresponding #GooCanvasPath objects.
 * (See goo_canvas_get_item() and #GooCanvas::item-created.)
 */

static void item_model_interface_init (GooCanvasItemModelIface *iface);
static void goo_canvas_path_model_finalize     (GObject            *object);
static void goo_canvas_path_model_get_property (GObject            *object,
						   guint               param_id,
						   GValue             *value,
						   GParamSpec         *pspec);
static void goo_canvas_path_model_set_property (GObject            *object,
						   guint               param_id,
						   const GValue       *value,
						   GParamSpec         *pspec);

G_DEFINE_TYPE_WITH_CODE (GooCanvasPathModel, goo_canvas_path_model,
			 GOO_TYPE_CANVAS_ITEM_MODEL_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM_MODEL,
						item_model_interface_init))


static void
goo_canvas_path_model_class_init (GooCanvasPathModelClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  gobject_class->finalize     = goo_canvas_path_model_finalize;

  gobject_class->get_property = goo_canvas_path_model_get_property;
  gobject_class->set_property = goo_canvas_path_model_set_property;

  goo_canvas_path_install_common_properties (gobject_class);
}


static void
goo_canvas_path_model_init (GooCanvasPathModel *pmodel)
{

}


/**
 * goo_canvas_path_model_new:
 * @parent: the parent model, or %NULL. If a parent is specified, it will
 *  assume ownership of the item, and the item will automatically be freed when
 *  it is removed from the parent. Otherwise call g_object_unref() to free it.
 * @path_data: the sequence of path commands, specified as a string using the
 *  same syntax as in the <ulink url="http://www.w3.org/Graphics/SVG/">Scalable
 *  Vector Graphics (SVG)</ulink> path element.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new path model.
 * 
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a red line from (20,20) to (40,40):
 *
 * <informalexample><programlisting>
 *  GooCanvasItemModel *path = goo_canvas_path_model_new (mygroup,
 *                                                        "M 20 20 L 40 40",
 *                                                        "stroke-color", "red",
 *                                                        NULL);
 * </programlisting></informalexample>
 * 
 * This example creates a cubic bezier curve from (20,100) to (100,100) with
 * the control points at (20,50) and (100,50):
 *
 * <informalexample><programlisting>
 *  GooCanvasItemModel *path = goo_canvas_path_model_new (mygroup,
 *                                                        "M20,100 C20,50 100,50 100,100",
 *                                                        "stroke-color", "blue",
 *                                                        NULL);
 * </programlisting></informalexample>
 *
 * This example uses an elliptical arc to create a filled circle with one
 * quarter missing:
 * 
 * <informalexample><programlisting>
 *  GooCanvasItemModel *path = goo_canvas_path_model_new (mygroup,
 *                                                        "M200,500 h-150 a150,150 0 1,0 150,-150 z",
 *                                                        "fill-color", "red",
 *                                                        "stroke-color", "blue",
 *                                                        "line-width", 5.0,
 *                                                        NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new path model.
 **/
GooCanvasItemModel*
goo_canvas_path_model_new (GooCanvasItemModel *parent,
			   const gchar        *path_data,
			   ...)
{
  GooCanvasItemModel *model;
  GooCanvasPathModel *pmodel;
  const char *first_property;
  va_list var_args;

  model = g_object_new (GOO_TYPE_CANVAS_PATH_MODEL, NULL);
  pmodel = (GooCanvasPathModel*) model;

  pmodel->path_data.path_commands = goo_canvas_parse_path_data (path_data);

  va_start (var_args, path_data);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist ((GObject*) model, first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_model_add_child (parent, model, -1);
      g_object_unref (model);
    }

  return model;
}


static void
goo_canvas_path_model_finalize (GObject *object)
{
  GooCanvasPathModel *pmodel = (GooCanvasPathModel*) object;

  if (pmodel->path_data.path_commands)
    g_array_free (pmodel->path_data.path_commands, TRUE);

  G_OBJECT_CLASS (goo_canvas_path_model_parent_class)->finalize (object);
}


static void
goo_canvas_path_model_get_property (GObject              *object,
				    guint                 prop_id,
				    GValue               *value,
				    GParamSpec           *pspec)
{
  GooCanvasPathModel *pmodel = (GooCanvasPathModel*) object;

  goo_canvas_path_get_common_property (object, NULL, &pmodel->path_data,
                                       prop_id, value, pspec);
}


static void
goo_canvas_path_model_set_property (GObject              *object,
				    guint                 prop_id,
				    const GValue         *value,
				    GParamSpec           *pspec)
{
  GooCanvasPathModel *pmodel = (GooCanvasPathModel*) object;

  goo_canvas_path_set_common_property (object, NULL, &pmodel->path_data,
                                       prop_id, value, pspec);
  g_signal_emit_by_name (pmodel, "changed", TRUE);
}


static GooCanvasItem*
goo_canvas_path_model_create_item (GooCanvasItemModel *model,
				   GooCanvas          *canvas)
{
  GooCanvasItem *item;

  item = g_object_new (GOO_TYPE_CANVAS_PATH, NULL);
  goo_canvas_item_set_model (item, model);

  return item;
}


static void
item_model_interface_init (GooCanvasItemModelIface *iface)
{
  iface->create_item    = goo_canvas_path_model_create_item;
}
