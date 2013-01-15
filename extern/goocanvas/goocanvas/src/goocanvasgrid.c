/*
 * GooCanvas. Copyright (C) 2005-8 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasgrid.c - a grid item.
 */

/**
 * SECTION:goocanvasgrid
 * @Title: GooCanvasGrid
 * @Short_Description: a grid item.
 *
 * GooCanvasGrid represents a grid item.
 * A grid consists of a number of equally-spaced horizontal and vertical
 * grid lines, plus an optional border.
 *
 * It is a subclass of #GooCanvasItemSimple and so inherits all of the style
 * properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItem interface, so you can use the
 * #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate().
 *
 * To create a #GooCanvasGrid use goo_canvas_grid_new().
 *
 * To get or set the properties of an existing #GooCanvasGrid, use
 * g_object_get() and g_object_set().
 *
 * The grid's position and size is specified with the #GooCanvasGrid:x,
 * #GooCanvasGrid:y, #GooCanvasGrid:width and #GooCanvasGrid:height properties.
 *
 * The #GooCanvasGrid:x-step and #GooCanvasGrid:y-step properties specify the 
 * distance between grid lines. The  #GooCanvasGrid:x-offset and
 * #GooCanvasGrid:y-offset properties specify the distance before the first
 * grid lines.
 *
 * The horizontal or vertical grid lines can be hidden using the
 * #GooCanvasGrid:show-horz-grid-lines and #GooCanvasGrid:show-vert-grid-lines
 * properties.
 *
 * The width of the border can be set using the #GooCanvasGrid:border-width
 * property. The border is drawn outside the area specified with the
 * #GooCanvasGrid:x, #GooCanvasGrid:y, #GooCanvasGrid:width and
 * #GooCanvasGrid:height properties.
 *
 * Other properties allow the colors and widths of the grid lines to be set.
 * The grid line color and width properties override the standard
 * #GooCanvasItemSimple:stroke-color and #GooCanvasItemSimple:line-width
 * properties, enabling different styles for horizontal and vertical grid lines.
 */
#include <config.h>
#include <math.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvas.h"


enum {
  PROP_0,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_X_STEP,
  PROP_Y_STEP,
  PROP_X_OFFSET,
  PROP_Y_OFFSET,
  PROP_HORZ_GRID_LINE_WIDTH,
  PROP_VERT_GRID_LINE_WIDTH,
  PROP_HORZ_GRID_LINE_PATTERN,
  PROP_VERT_GRID_LINE_PATTERN,
  PROP_BORDER_WIDTH,
  PROP_BORDER_PATTERN,
  PROP_SHOW_HORZ_GRID_LINES,
  PROP_SHOW_VERT_GRID_LINES,
  PROP_VERT_GRID_LINES_ON_TOP,

  /* Convenience properties. */
  PROP_HORZ_GRID_LINE_COLOR,
  PROP_HORZ_GRID_LINE_COLOR_RGBA,
  PROP_HORZ_GRID_LINE_PIXBUF,
  PROP_VERT_GRID_LINE_COLOR,
  PROP_VERT_GRID_LINE_COLOR_RGBA,
  PROP_VERT_GRID_LINE_PIXBUF,
  PROP_BORDER_COLOR,
  PROP_BORDER_COLOR_RGBA,
  PROP_BORDER_PIXBUF
};


GooCanvasItemIface *goo_canvas_grid_parent_iface;

static void canvas_item_interface_init      (GooCanvasItemIface  *iface);

G_DEFINE_TYPE_WITH_CODE (GooCanvasGrid, goo_canvas_grid,
                         GOO_TYPE_CANVAS_ITEM_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM,
						canvas_item_interface_init))


static void
goo_canvas_grid_install_common_properties (GObjectClass *gobject_class)
{
  g_object_class_install_property (gobject_class, PROP_X,
				   g_param_spec_double ("x",
							"X",
							_("The x coordinate of the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
				   g_param_spec_double ("y",
							"Y",
							_("The y coordinate of the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
				   g_param_spec_double ("width",
							_("Width"),
							_("The width of the grid"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
				   g_param_spec_double ("height",
							_("Height"),
							_("The height of the grid"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X_STEP,
				   g_param_spec_double ("x-step",
							"X Step",
							_("The distance between the vertical grid lines"),
							0.0, G_MAXDOUBLE, 10.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y_STEP,
				   g_param_spec_double ("y-step",
							"Y Step",
							_("The distance between the horizontal grid lines"),
							0.0, G_MAXDOUBLE, 10.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_X_OFFSET,
				   g_param_spec_double ("x-offset",
							"X Offset",
							_("The distance before the first vertical grid line"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y_OFFSET,
				   g_param_spec_double ("y-offset",
							"Y Offset",
							_("The distance before the first horizontal grid line"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_WIDTH,
                                   g_param_spec_double ("horz-grid-line-width",
                                                        _("Horizontal Grid Line Width"),
                                                        _("The width of the horizontal grid lines"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_WIDTH,
                                   g_param_spec_double ("vert-grid-line-width",
							_("Vertical Grid Line Width"),
							_("The width of the vertical grid lines"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_PATTERN,
                                   g_param_spec_boxed ("horz-grid-line-pattern",
						       _("Horizontal Grid Line Pattern"),
						       _("The cairo pattern to paint the horizontal grid lines with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_PATTERN,
                                   g_param_spec_boxed ("vert-grid-line-pattern",
						       _("Vertical Grid Line Pattern"),
						       _("The cairo pattern to paint the vertical grid lines with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_WIDTH,
				   g_param_spec_double ("border-width",
							_("Border Width"),
							_("The width of the border around the grid"),
							-G_MAXDOUBLE,
							G_MAXDOUBLE, -1.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_PATTERN,
                                   g_param_spec_boxed ("border-pattern",
						       _("Border Pattern"),
						       _("The cairo pattern to paint the border with"),
						       GOO_TYPE_CAIRO_PATTERN,
						       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SHOW_HORZ_GRID_LINES,
                                   g_param_spec_boolean ("show-horz-grid-lines",
							 _("Show Horizontal Grid Lines"),
							 _("If the horizontal grid lines are shown"),
							 TRUE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_SHOW_VERT_GRID_LINES,
                                   g_param_spec_boolean ("show-vert-grid-lines",
							 _("Show Vertical Grid Lines"),
							 _("If the vertical grid lines are shown"),
							 TRUE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINES_ON_TOP,
                                   g_param_spec_boolean ("vert-grid-lines-on-top",
							 _("Vertical Grid Lines On Top"),
							 _("If the vertical grid lines are painted above the horizontal grid lines"),
							 FALSE,
							 G_PARAM_READWRITE));


  /* Convenience properties - some are writable only. */
  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_COLOR,
				   g_param_spec_string ("horz-grid-line-color",
							_("Horizontal Grid Line Color"),
							_("The color to use for the horizontal grid lines"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_COLOR_RGBA,
				   g_param_spec_uint ("horz-grid-line-color-rgba",
						      _("Horizontal Grid Line Color RGBA"),
						      _("The color to use for the horizontal grid lines, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_PIXBUF,
                                   g_param_spec_object ("horz-grid-line-pixbuf",
							_("Horizontal Grid Line Pixbuf"),
							_("The pixbuf to use to draw the horizontal grid lines"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_COLOR,
				   g_param_spec_string ("vert-grid-line-color",
							_("Vertical Grid Line Color"),
							_("The color to use for the vertical grid lines"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_COLOR_RGBA,
				   g_param_spec_uint ("vert-grid-line-color-rgba",
						      _("Vertical Grid Line Color RGBA"),
						      _("The color to use for the vertical grid lines, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_PIXBUF,
                                   g_param_spec_object ("vert-grid-line-pixbuf",
							_("Vertical Grid Line Pixbuf"),
							_("The pixbuf to use to draw the vertical grid lines"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_BORDER_COLOR,
				   g_param_spec_string ("border-color",
							_("Border Color"),
							_("The color to use for the border"),
							NULL,
							G_PARAM_WRITABLE));

  g_object_class_install_property (gobject_class, PROP_BORDER_COLOR_RGBA,
				   g_param_spec_uint ("border-color-rgba",
						      _("Border Color RGBA"),
						      _("The color to use for the border, specified as a 32-bit integer value"),
						      0, G_MAXUINT, 0,
						      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_BORDER_PIXBUF,
                                   g_param_spec_object ("border-pixbuf",
							_("Border Pixbuf"),
							_("The pixbuf to use to draw the border"),
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_WRITABLE));
}


/* This initializes the common grid data. */
static void
goo_canvas_grid_init_data (GooCanvasGridData *grid_data)
{
  grid_data->x = 0.0;
  grid_data->y = 0.0;
  grid_data->width = 0.0;
  grid_data->height = 0.0;
  grid_data->x_step = 10.0;
  grid_data->y_step = 10.0;
  grid_data->x_offset = 0.0;
  grid_data->y_offset = 0.0;
  grid_data->horz_grid_line_width = -1.0;
  grid_data->vert_grid_line_width = -1.0;
  grid_data->horz_grid_line_pattern = NULL;
  grid_data->vert_grid_line_pattern = NULL;
  grid_data->border_width = -1.0;
  grid_data->border_pattern = NULL;
  grid_data->show_horz_grid_lines = TRUE;
  grid_data->show_vert_grid_lines = TRUE;
  grid_data->vert_grid_lines_on_top = FALSE;
}


/* This frees the contents of the grid data, but not the struct itself. */
static void
goo_canvas_grid_free_data (GooCanvasGridData *grid_data)
{

}


static void
goo_canvas_grid_init (GooCanvasGrid *grid)
{
  grid->grid_data = g_slice_new0 (GooCanvasGridData);
  goo_canvas_grid_init_data (grid->grid_data);
}


/**
 * goo_canvas_grid_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @x: the x coordinate of the left of the grid.
 * @y: the y coordinate of the top of the grid.
 * @width: the width of the grid.
 * @height: the height of the grid.
 * @x_step: the distance between the vertical grid lines.
 * @y_step: the distance between the horizontal grid lines.
 * @x_offset: the distance before the first vertical grid line.
 * @y_offset: the distance before the first horizontal grid line.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new grid item.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a grid:
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *grid = goo_canvas_grid_new (mygroup, 100.0, 100.0, 400.0, 200.0,
 *                                             20.0, 20.0, 10.0, 10.0,
 *                                             "horz-grid-line-width", 4.0,
 *                                             "horz-grid-line-color", "yellow",
 *                                             "vert-grid-line-width", 2.0,
 *                                             "vert-grid-line-color", "red",
 *                                             "border-width", 3.0,
 *                                             "border-color", "white",
 *                                             "fill-color", "blue",
 *                                             NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new grid item.
 **/
GooCanvasItem*
goo_canvas_grid_new (GooCanvasItem      *parent,
		     gdouble             x,
		     gdouble             y,
		     gdouble             width,
		     gdouble             height,
		     gdouble             x_step,
		     gdouble             y_step,
		     gdouble             x_offset,
		     gdouble             y_offset,
		     ...)
{
  GooCanvasItem *item;
  GooCanvasGrid *grid;
  GooCanvasGridData *grid_data;
  va_list var_args;
  const char *first_property;

  item = g_object_new (GOO_TYPE_CANVAS_GRID, NULL);
  grid = (GooCanvasGrid*) item;

  grid_data = grid->grid_data;
  grid_data->x = x;
  grid_data->y = y;
  grid_data->width = width;
  grid_data->height = height;
  grid_data->x_step = x_step;
  grid_data->y_step = y_step;
  grid_data->x_offset = x_offset;
  grid_data->y_offset = y_offset;

  va_start (var_args, y_offset);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (item), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


static void
goo_canvas_grid_finalize (GObject *object)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasGrid *grid = (GooCanvasGrid*) object;

  /* Free our data if we didn't have a model. (If we had a model it would
     have been reset in dispose() and simple_data will be NULL.) */
  if (simple->simple_data)
    {
      goo_canvas_grid_free_data (grid->grid_data);
      g_slice_free (GooCanvasGridData, grid->grid_data);
    }
  grid->grid_data = NULL;

  G_OBJECT_CLASS (goo_canvas_grid_parent_class)->finalize (object);
}


static void
goo_canvas_grid_get_common_property (GObject              *object,
				     GooCanvasGridData    *grid_data,
				     guint                 prop_id,
				     GValue               *value,
				     GParamSpec           *pspec)
{
  switch (prop_id)
    {
    case PROP_X:
      g_value_set_double (value, grid_data->x);
      break;
    case PROP_Y:
      g_value_set_double (value, grid_data->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, grid_data->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, grid_data->height);
      break;
    case PROP_X_STEP:
      g_value_set_double (value, grid_data->x_step);
      break;
    case PROP_Y_STEP:
      g_value_set_double (value, grid_data->y_step);
      break;
    case PROP_X_OFFSET:
      g_value_set_double (value, grid_data->x_offset);
      break;
    case PROP_Y_OFFSET:
      g_value_set_double (value, grid_data->y_offset);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      g_value_set_double (value, grid_data->horz_grid_line_width);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      g_value_set_double (value, grid_data->vert_grid_line_width);
      break;
    case PROP_HORZ_GRID_LINE_PATTERN:
      g_value_set_boxed (value, grid_data->horz_grid_line_pattern);
      break;
    case PROP_VERT_GRID_LINE_PATTERN:
      g_value_set_boxed (value, grid_data->vert_grid_line_pattern);
      break;
    case PROP_BORDER_WIDTH:
      g_value_set_double (value, grid_data->border_width);
      break;
    case PROP_BORDER_PATTERN:
      g_value_set_boxed (value, grid_data->border_pattern);
      break;
    case PROP_SHOW_HORZ_GRID_LINES:
      g_value_set_boolean (value, grid_data->show_horz_grid_lines);
      break;
    case PROP_SHOW_VERT_GRID_LINES:
      g_value_set_boolean (value, grid_data->show_vert_grid_lines);
      break;
    case PROP_VERT_GRID_LINES_ON_TOP:
      g_value_set_boolean (value, grid_data->vert_grid_lines_on_top);
      break;

  /* Convenience properties. */
    case PROP_HORZ_GRID_LINE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid_data->horz_grid_line_pattern, value);
      break;
    case PROP_VERT_GRID_LINE_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid_data->vert_grid_line_pattern, value);
      break;
    case PROP_BORDER_COLOR_RGBA:
      goo_canvas_get_rgba_value_from_pattern (grid_data->border_pattern, value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_grid_get_property (GObject              *object,
			      guint                 prop_id,
			      GValue               *value,
			      GParamSpec           *pspec)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) object;

  goo_canvas_grid_get_common_property (object, grid->grid_data,
					  prop_id, value, pspec);
}


static void
goo_canvas_grid_set_common_property (GObject              *object,
				     GooCanvasGridData    *grid_data,
				     guint                 prop_id,
				     const GValue         *value,
				     GParamSpec           *pspec)
{
  switch (prop_id)
    {
    case PROP_X:
      grid_data->x = g_value_get_double (value);
      break;
    case PROP_Y:
      grid_data->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      grid_data->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      grid_data->height = g_value_get_double (value);
      break;
    case PROP_X_STEP:
      grid_data->x_step = g_value_get_double (value);
      break;
    case PROP_Y_STEP:
      grid_data->y_step = g_value_get_double (value);
      break;
    case PROP_X_OFFSET:
      grid_data->x_offset = g_value_get_double (value);
      break;
    case PROP_Y_OFFSET:
      grid_data->y_offset = g_value_get_double (value);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      grid_data->horz_grid_line_width = g_value_get_double (value);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      grid_data->vert_grid_line_width = g_value_get_double (value);
      break;
    case PROP_HORZ_GRID_LINE_PATTERN:
      cairo_pattern_destroy (grid_data->horz_grid_line_pattern);
      grid_data->horz_grid_line_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid_data->horz_grid_line_pattern);
      break;
    case PROP_VERT_GRID_LINE_PATTERN:
      cairo_pattern_destroy (grid_data->vert_grid_line_pattern);
      grid_data->vert_grid_line_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid_data->vert_grid_line_pattern);
      break;
    case PROP_BORDER_WIDTH:
      grid_data->border_width = g_value_get_double (value);
      break;
    case PROP_BORDER_PATTERN:
      cairo_pattern_destroy (grid_data->border_pattern);
      grid_data->border_pattern = g_value_get_boxed (value);
      cairo_pattern_reference (grid_data->border_pattern);
      break;
    case PROP_SHOW_HORZ_GRID_LINES:
      grid_data->show_horz_grid_lines = g_value_get_boolean (value);
      break;
    case PROP_SHOW_VERT_GRID_LINES:
      grid_data->show_vert_grid_lines = g_value_get_boolean (value);
      break;
    case PROP_VERT_GRID_LINES_ON_TOP:
      grid_data->vert_grid_lines_on_top = g_value_get_boolean (value);
      break;

  /* Convenience properties. */
    case PROP_HORZ_GRID_LINE_COLOR:
      cairo_pattern_destroy (grid_data->horz_grid_line_pattern);
      grid_data->horz_grid_line_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_HORZ_GRID_LINE_COLOR_RGBA:
      cairo_pattern_destroy (grid_data->horz_grid_line_pattern);
      grid_data->horz_grid_line_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_HORZ_GRID_LINE_PIXBUF:
      cairo_pattern_destroy (grid_data->horz_grid_line_pattern);
      grid_data->horz_grid_line_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    case PROP_VERT_GRID_LINE_COLOR:
      cairo_pattern_destroy (grid_data->vert_grid_line_pattern);
      grid_data->vert_grid_line_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_VERT_GRID_LINE_COLOR_RGBA:
      cairo_pattern_destroy (grid_data->vert_grid_line_pattern);
      grid_data->vert_grid_line_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_VERT_GRID_LINE_PIXBUF:
      cairo_pattern_destroy (grid_data->vert_grid_line_pattern);
      grid_data->vert_grid_line_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    case PROP_BORDER_COLOR:
      cairo_pattern_destroy (grid_data->border_pattern);
      grid_data->border_pattern = goo_canvas_create_pattern_from_color_value (value);
      break;
    case PROP_BORDER_COLOR_RGBA:
      cairo_pattern_destroy (grid_data->border_pattern);
      grid_data->border_pattern = goo_canvas_create_pattern_from_rgba_value (value);
      break;
    case PROP_BORDER_PIXBUF:
      cairo_pattern_destroy (grid_data->border_pattern);
      grid_data->border_pattern = goo_canvas_create_pattern_from_pixbuf_value (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_grid_set_property (GObject              *object,
			      guint                 prop_id,
			      const GValue         *value,
			      GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasGrid *grid = (GooCanvasGrid*) object;

  if (simple->model)
    {
      g_warning ("Can't set property of a canvas item with a model - set the model property instead");
      return;
    }

  goo_canvas_grid_set_common_property (object, grid->grid_data,
					  prop_id, value, pspec);
  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_grid_update  (GooCanvasItemSimple *simple,
			 cairo_t             *cr)
{
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  GooCanvasGridData *grid_data = grid->grid_data;
  gdouble border_width = 0.0;

  /* We can quickly compute the bounds as being just the grid's size
     plus the border width around each edge. */
  if (grid_data->border_width > 0.0)
    border_width = grid_data->border_width;

  simple->bounds.x1 = grid_data->x - border_width;
  simple->bounds.y1 = grid_data->y - border_width;
  simple->bounds.x2 = grid_data->x + grid_data->width + border_width;
  simple->bounds.y2 = grid_data->y + grid_data->height + border_width;
}


static gdouble
calculate_start_position (gdouble start_pos,
			  gdouble step,
			  gdouble redraw_start_pos,
			  gdouble line_width)
{
  gdouble n = 0.0, result;

  /* We want the first position where pos + line_width/2 >= redraw_start_pos.
     i.e. start_pos + (n * step) + (line_width / 2) >= redraw_start_pos,
     or   (n * step) >= redraw_start_pos - start_pos - (line_width / 2),
     or   n >= (redraw_start_pos - start_pos - (line_width / 2) / step). */
  if (step > 0.0)
    n = ceil (((redraw_start_pos - start_pos - (line_width / 2.0))) / step);

  if (n <= 0.0)
    result = start_pos;
  else
    result = start_pos + (n * step);

  return result;
}


static void
paint_vertical_lines (GooCanvasItemSimple   *simple,
		      cairo_t               *cr,
		      const GooCanvasBounds *bounds)
{
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  GooCanvasGridData *grid_data = grid->grid_data;
  double x, max_x, max_y, max_bounds_x, line_width;
  gboolean has_stroke;

  if (!grid_data->show_vert_grid_lines)
    return;

  max_x = grid_data->x + grid_data->width;
  max_y = grid_data->y + grid_data->height;

  has_stroke = goo_canvas_style_set_stroke_options (simple_data->style, cr);
  line_width = goo_canvas_item_simple_get_line_width (simple);

  /* If the grid's vertical grid line pattern/color has been set, use that.
     If not, and we don't have a stroke color just return. */
  if (grid_data->vert_grid_line_pattern)
    cairo_set_source (cr, grid_data->vert_grid_line_pattern);
  else if (!has_stroke)
    return;

  /* If the grid's vertical grid line width has been set, use that. */
  if (grid_data->vert_grid_line_width > 0.0)
    {
      line_width = grid_data->vert_grid_line_width;
      cairo_set_line_width (cr, line_width);
    }

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  /* Calculate the first grid line that intersects the bounds to redraw. */
  x = calculate_start_position (grid_data->x + grid_data->x_offset,
				grid_data->x_step, bounds->x1, line_width);

  /* Calculate the last possible line position. */
  max_bounds_x = bounds->x2 + (line_width / 2.0);
  max_x = MIN (max_x, max_bounds_x);

  /* Add on a tiny fraction of step to avoid any double comparison issues. */
  max_x += grid_data->x_step * 0.00001;

  while (x <= max_x)
    {
      cairo_move_to (cr, x, grid_data->y);
      cairo_line_to (cr, x, max_y);
      cairo_stroke (cr);

      /* Avoid an infinite loop. */
      if (grid_data->x_step <= 0.0)
        break;

      x += grid_data->x_step;
    }
}


static void
paint_horizontal_lines (GooCanvasItemSimple   *simple,
			cairo_t               *cr,
			const GooCanvasBounds *bounds)
{
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  GooCanvasGridData *grid_data = grid->grid_data;
  double y, max_x, max_y, max_bounds_y, line_width;
  gboolean has_stroke;

  if (!grid_data->show_horz_grid_lines)
    return;

  max_x = grid_data->x + grid_data->width;
  max_y = grid_data->y + grid_data->height;

  has_stroke = goo_canvas_style_set_stroke_options (simple_data->style, cr);
  line_width = goo_canvas_item_simple_get_line_width (simple);

  /* If the grid's horizontal grid line pattern/color has been set, use that.
     If not, and we don't have a stroke color just return. */
  if (grid_data->horz_grid_line_pattern)
    cairo_set_source (cr, grid_data->horz_grid_line_pattern);
  else if (!has_stroke)
    return;

  /* If the grid's horizontal grid line width has been set, use that. */
  if (grid_data->horz_grid_line_width > 0.0)
    {
      line_width = grid_data->horz_grid_line_width;
      cairo_set_line_width (cr, grid_data->horz_grid_line_width);
    }

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  /* Calculate the first grid line that intersects the bounds to redraw. */
  y = calculate_start_position (grid_data->y + grid_data->y_offset,
				grid_data->y_step, bounds->y1, line_width);

  /* Calculate the last possible line position. */
  max_bounds_y = bounds->y2 + (line_width / 2.0);
  max_y = MIN (max_y, max_bounds_y);

  /* Add on a tiny fraction of step to avoid any double comparison issues. */
  max_y += grid_data->y_step * 0.00001;

  while (y <= max_y)
    {
      cairo_move_to (cr, grid_data->x, y);
      cairo_line_to (cr, max_x, y);
      cairo_stroke (cr);

      /* Avoid an infinite loop. */
      if (grid_data->y_step <= 0.0)
        break;

      y += grid_data->y_step;
    }
}


static void
goo_canvas_grid_paint (GooCanvasItemSimple   *simple,
		       cairo_t               *cr,
		       const GooCanvasBounds *bounds)
{
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGrid *grid = (GooCanvasGrid*) simple;
  GooCanvasGridData *grid_data = grid->grid_data;
  GooCanvasBounds redraw_bounds = *bounds;
  gdouble half_border_width;

  /* Paint the background in the fill pattern/color, if one is set. */
  if (goo_canvas_style_set_fill_options (simple_data->style, cr))
    {
      cairo_rectangle (cr, grid_data->x, grid_data->y,
		       grid_data->width, grid_data->height);
      cairo_fill (cr);
    }

  /* Clip to the grid's area while painting the grid lines. */
  cairo_save (cr);
  cairo_rectangle (cr, grid_data->x, grid_data->y,
		   grid_data->width, grid_data->height);
  cairo_clip (cr);

  /* Convert the bounds to be redrawn from device space to item space. */
  goo_canvas_convert_bounds_to_item_space (simple->canvas,
					   (GooCanvasItem*) simple,
					   &redraw_bounds);

  /* Paint the grid lines, in the required order. */
  if (grid_data->vert_grid_lines_on_top)
    {
      paint_horizontal_lines (simple, cr, &redraw_bounds);
      paint_vertical_lines (simple, cr, &redraw_bounds);
    }
  else
    {
      paint_vertical_lines (simple, cr, &redraw_bounds);
      paint_horizontal_lines (simple, cr, &redraw_bounds);
    }

  cairo_restore (cr);

  /* Paint the border. */
  if (grid_data->border_width > 0)
    {
      if (grid_data->border_pattern)
	cairo_set_source (cr, grid_data->border_pattern);
      else
	goo_canvas_style_set_stroke_options (simple_data->style, cr);

      cairo_set_line_width (cr, grid_data->border_width);
      half_border_width = grid_data->border_width / 2.0;
      cairo_rectangle (cr, grid_data->x - half_border_width,
		       grid_data->y - half_border_width,
		       grid_data->width + grid_data->border_width,
		       grid_data->height + grid_data->border_width);
      cairo_stroke (cr);
    }
}


static void
goo_canvas_grid_set_model    (GooCanvasItem      *item,
			      GooCanvasItemModel *model)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGrid *grid = (GooCanvasGrid*) item;
  GooCanvasGridModel *gmodel = (GooCanvasGridModel*) model;

  /* If our grid_data was allocated, free it. */
  if (!simple->model)
    {
      goo_canvas_grid_free_data (grid->grid_data);
      g_slice_free (GooCanvasGridData, grid->grid_data);
    }

  /* Now use the new model's grid_data instead. */
  grid->grid_data = &gmodel->grid_data;

  /* Let the parent class do the rest. */
  goo_canvas_grid_parent_iface->set_model (item, model);
}


static void
canvas_item_interface_init (GooCanvasItemIface *iface)
{
  iface->set_model      = goo_canvas_grid_set_model;
}


static void
goo_canvas_grid_class_init (GooCanvasGridClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemSimpleClass *simple_class = (GooCanvasItemSimpleClass*) klass;

  goo_canvas_grid_parent_iface = g_type_interface_peek (goo_canvas_grid_parent_class, GOO_TYPE_CANVAS_ITEM);

  gobject_class->finalize    = goo_canvas_grid_finalize;

  gobject_class->get_property = goo_canvas_grid_get_property;
  gobject_class->set_property = goo_canvas_grid_set_property;

  simple_class->simple_update      = goo_canvas_grid_update;
  simple_class->simple_paint       = goo_canvas_grid_paint;

  goo_canvas_grid_install_common_properties (gobject_class);
}



/**
 * SECTION:goocanvasgridmodel
 * @Title: GooCanvasGridModel
 * @Short_Description: a model for grid items.
 *
 * GooCanvasGridModel represents a model for grid items.
 * A grid consists of a number of equally-spaced horizontal and vertical
 * grid lines, plus an optional border.
 *
 * It is a subclass of #GooCanvasItemModelSimple and so inherits all of the
 * style properties such as "stroke-color", "fill-color" and "line-width".
 *
 * It also implements the #GooCanvasItemModel interface, so you can use the
 * #GooCanvasItemModel functions such as goo_canvas_item_model_raise() and
 * goo_canvas_item_model_rotate().
 *
 * To create a #GooCanvasGridModel use goo_canvas_grid_model_new().
 *
 * To get or set the properties of an existing #GooCanvasGridModel, use
 * g_object_get() and g_object_set().
 *
 * To respond to events such as mouse clicks on the grid you must connect
 * to the signal handlers of the corresponding #GooCanvasGrid objects.
 * (See goo_canvas_get_item() and #GooCanvas::item-created.)
 *
 * The grid's position and size is specified with the #GooCanvasGridModel:x,
 * #GooCanvasGridModel:y, #GooCanvasGridModel:width and
 * #GooCanvasGridModel:height properties.
 *
 * The #GooCanvasGridModel:x-step and #GooCanvasGridModel:y-step properties
 * specify the distance between grid lines. The  #GooCanvasGridModel:x-offset
 * and #GooCanvasGridModel:y-offset properties specify the distance before the
 * first grid lines.
 *
 * The horizontal or vertical grid lines can be hidden using the
 * #GooCanvasGridModel:show-horz-grid-lines and
 * #GooCanvasGridModel:show-vert-grid-lines properties.
 *
 * The width of the border can be set using the #GooCanvasGridModel:border-width
 * property. The border is drawn outside the area specified with the
 * #GooCanvasGridModel:x, #GooCanvasGridModel:y, #GooCanvasGridModel:width and
 * #GooCanvasGridModel:height properties.
 *
 * Other properties allow the colors and widths of the grid lines to be set.
 * The grid line color and width properties override the standard
 * #GooCanvasItemModelSimple:stroke-color and
 * #GooCanvasItemModelSimple:line-width properties, enabling different styles
 * for horizontal and vertical grid lines.
 */

GooCanvasItemModelIface *goo_canvas_grid_model_parent_iface;

static void item_model_interface_init (GooCanvasItemModelIface *iface);

G_DEFINE_TYPE_WITH_CODE (GooCanvasGridModel, goo_canvas_grid_model,
                         GOO_TYPE_CANVAS_ITEM_MODEL_SIMPLE,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM_MODEL,
						item_model_interface_init))


static void
goo_canvas_grid_model_init (GooCanvasGridModel *gmodel)
{
  goo_canvas_grid_init_data (&gmodel->grid_data);
}


/**
 * goo_canvas_grid_model_new:
 * @parent: the parent model, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @x: the x coordinate of the left of the grid.
 * @y: the y coordinate of the top of the grid.
 * @width: the width of the grid.
 * @height: the height of the grid.
 * @x_step: the distance between the vertical grid lines.
 * @y_step: the distance between the horizontal grid lines.
 * @x_offset: the distance before the first vertical grid line.
 * @y_offset: the distance before the first horizontal grid line.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new grid model.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a grid:
 *
 * <informalexample><programlisting>
 *  GooCanvasItemModel *grid = goo_canvas_grid_model_new (mygroup, 100.0, 100.0, 400.0, 200.0,
 *                                                        20.0, 20.0, 10.0, 10.0,
 *                                                        "horz-grid-line-width", 4.0,
 *                                                        "horz-grid-line-color", "yellow",
 *                                                        "vert-grid-line-width", 2.0,
 *                                                        "vert-grid-line-color", "red",
 *                                                        "border-width", 3.0,
 *                                                        "border-color", "white",
 *                                                        "fill-color", "blue",
 *                                                        NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new grid model.
 **/
GooCanvasItemModel*
goo_canvas_grid_model_new (GooCanvasItemModel *parent,
			   gdouble             x,
			   gdouble             y,
			   gdouble             width,
			   gdouble             height,
			   gdouble             x_step,
			   gdouble             y_step,
			   gdouble             x_offset,
			   gdouble             y_offset,
			   ...)
{
  GooCanvasItemModel *model;
  GooCanvasGridModel *gmodel;
  GooCanvasGridData *grid_data;
  const char *first_property;
  va_list var_args;

  model = g_object_new (GOO_TYPE_CANVAS_GRID_MODEL, NULL);
  gmodel = (GooCanvasGridModel*) model;

  grid_data = &gmodel->grid_data;
  grid_data->x = x;
  grid_data->y = y;
  grid_data->width = width;
  grid_data->height = height;
  grid_data->x_step = x_step;
  grid_data->y_step = y_step;
  grid_data->x_offset = x_offset;
  grid_data->y_offset = y_offset;

  va_start (var_args, y_offset);
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
goo_canvas_grid_model_finalize (GObject *object)
{
  GooCanvasGridModel *gmodel = (GooCanvasGridModel*) object;

  goo_canvas_grid_free_data (&gmodel->grid_data);

  G_OBJECT_CLASS (goo_canvas_grid_model_parent_class)->finalize (object);
}


static void
goo_canvas_grid_model_get_property (GObject              *object,
				    guint                 prop_id,
				    GValue               *value,
				    GParamSpec           *pspec)
{
  GooCanvasGridModel *gmodel = (GooCanvasGridModel*) object;

  goo_canvas_grid_get_common_property (object, &gmodel->grid_data,
					  prop_id, value, pspec);
}


static void
goo_canvas_grid_model_set_property (GObject              *object,
				    guint                 prop_id,
				    const GValue         *value,
				    GParamSpec           *pspec)
{
  GooCanvasGridModel *gmodel = (GooCanvasGridModel*) object;

  goo_canvas_grid_set_common_property (object, &gmodel->grid_data,
					  prop_id, value, pspec);
  g_signal_emit_by_name (gmodel, "changed", TRUE);
}


static GooCanvasItem*
goo_canvas_grid_model_create_item (GooCanvasItemModel *model,
				   GooCanvas          *canvas)
{
  GooCanvasItem *item;

  item = g_object_new (GOO_TYPE_CANVAS_GRID, NULL);
  goo_canvas_item_set_model (item, model);

  return item;
}


static void
item_model_interface_init (GooCanvasItemModelIface *iface)
{
  iface->create_item    = goo_canvas_grid_model_create_item;
}


static void
goo_canvas_grid_model_class_init (GooCanvasGridModelClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  goo_canvas_grid_model_parent_iface = g_type_interface_peek (goo_canvas_grid_model_parent_class, GOO_TYPE_CANVAS_ITEM_MODEL);

  gobject_class->finalize     = goo_canvas_grid_model_finalize;

  gobject_class->get_property = goo_canvas_grid_model_get_property;
  gobject_class->set_property = goo_canvas_grid_model_set_property;

  goo_canvas_grid_install_common_properties (gobject_class);
}


