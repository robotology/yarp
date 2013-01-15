/*
 * GooCanvas. Copyright (C) 2005-7 Damon Chaplin.
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 * Copyright (C) 1997-2000 the GTK+ team.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvastable.c - table item. A lot of this code has been ported from
 * the GtkTable widget.
 */

/**
 * SECTION:goocanvastable
 * @Title: GooCanvasTable
 * @Short_Description: a table container to layout items.
 *
 * #GooCanvasTable is a table container used to lay out other canvas items.
 * It is used in a similar way to how the GtkTable widget is used to lay out
 * GTK+ widgets.
 *
 * Items are added to the table using the normal methods, then
 * goo_canvas_item_set_child_properties() is used to specify how each child
 * item is to be positioned within the table (i.e. which row and column it is
 * in, how much padding it should have and whether it should expand or
 * shrink).
 *
 * #GooCanvasTable is a subclass of #GooCanvasItemSimple and so
 * inherits all of the style properties such as "stroke-color", "fill-color"
 * and "line-width". Setting a style property on a #GooCanvasTable will affect
 * all children of the #GooCanvasTable (unless the children override the
 * property setting).
 *
 * #GooCanvasTable implements the #GooCanvasItem interface, so you can use
 * the #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate(), and the properties such as "visibility" and
 * "pointer-events".
 *
 * To create a #GooCanvasTable use goo_canvas_table_new().
 *
 * To get or set the properties of an existing #GooCanvasTable, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <math.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvastable.h"
#include "goocanvas.h"


enum
{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_ROW_SPACING,
  PROP_COLUMN_SPACING,
  PROP_HOMOGENEOUS_ROWS,
  PROP_HOMOGENEOUS_COLUMNS,
  PROP_X_BORDER_SPACING,
  PROP_Y_BORDER_SPACING,
  PROP_VERT_GRID_LINE_WIDTH,
  PROP_HORZ_GRID_LINE_WIDTH
};

enum
{
  CHILD_PROP_0,
  CHILD_PROP_LEFT_PADDING,
  CHILD_PROP_RIGHT_PADDING,
  CHILD_PROP_TOP_PADDING,
  CHILD_PROP_BOTTOM_PADDING,
  CHILD_PROP_X_ALIGN,
  CHILD_PROP_Y_ALIGN,
  CHILD_PROP_ROW,
  CHILD_PROP_COLUMN,
  CHILD_PROP_ROWS,
  CHILD_PROP_COLUMNS,
  CHILD_PROP_X_EXPAND,
  CHILD_PROP_X_FILL,
  CHILD_PROP_X_SHRINK,
  CHILD_PROP_Y_EXPAND,
  CHILD_PROP_Y_FILL,
  CHILD_PROP_Y_SHRINK
};

#define HORZ 0
#define VERT 1

typedef enum
{
  GOO_CANVAS_TABLE_CHILD_EXPAND	= 1 << 0,
  GOO_CANVAS_TABLE_CHILD_FILL 	= 1 << 1,
  GOO_CANVAS_TABLE_CHILD_SHRINK	= 1 << 2
} GooCanvasTableChildFlags;


/* This is used in the GooCanvasTableData children GArray to keep the child
   properties for each of the children. */
typedef struct _GooCanvasTableChild  GooCanvasTableChild;
struct _GooCanvasTableChild
{
  gdouble position[2];			/* Translation offset in table. */
  gdouble start_pad[2], end_pad[2];
  gdouble align[2];
  guint16 start[2], size[2];		/* Start row/col & num rows/cols. */
  guint8 flags[2];			/* GooCanvasTableChildFlags. */
};

typedef struct _GooCanvasTableDimensionLayoutData GooCanvasTableDimensionLayoutData;
struct _GooCanvasTableDimensionLayoutData
{
  /* This is the actual spacing for after the row or column, including
     the grid line width. It is set in goo_canvas_table_init_layout_data(). */
  gdouble spacing;

  /* Stores the grid line visibilty for the grid lines to the right or bottom
     of this row/column, respectivel, for each cell in the other dimension. */
  guint32 *grid_line_visibility;

  /* The requisition is calculated in goo_canvas_table_size_request_pass[123]*/
  gdouble requisition;

  /* The allocation is initialized in goo_canvas_table_size_allocate_init()
     and calculated in goo_canvas_table_size_allocate_pass1(). */
  gdouble allocation;

  /* The row or column start and end positions are calculated in
     goo_canvas_table_size_allocate_pass2(). */
  gdouble start;
  gdouble end;

  /* These flags are initialized in goo_canvas_table_init_layout_data() and
     set in goo_canvas_table_size_request_init(). */
  guint need_expand : 1;
  guint need_shrink : 1;
  guint expand : 1;
  guint shrink : 1;
  guint empty : 1;
};

typedef struct _GooCanvasTableChildLayoutData GooCanvasTableChildLayoutData;
struct _GooCanvasTableChildLayoutData
{
  gdouble requested_position[2];
  gdouble requested_size[2];
  gdouble start_pad[2], end_pad[2];
};

/* Convenience macros to set/unset/check bit-flags. */
#define GOO_CANVAS_TABLE_SET_GRID_LINE_VISIBILITY(dim, x, y) \
  ((dim)[(x)].grid_line_visibility[(y)/32] |= (1 << ((y) % 32)))

#define GOO_CANVAS_TABLE_UNSET_GRID_LINE_VISIBILITY(dim, x, y) \
  ((dim)[(x)].grid_line_visibility[(y)/32] &= ~(1 << ((y) % 32)))

#define GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(dim, x, y) \
  ((dim)[(x)].grid_line_visibility[(y)/32] & (1 << ((y) % 32)))

/* The children array is only kept around while doing the layout.
   It gets freed in goo_canvas_table_allocate_area(). */
struct _GooCanvasTableLayoutData
{
  GooCanvasTableDimensionLayoutData *dldata[2];
  GooCanvasTableChildLayoutData *children;

  /* Position of the table */
  gdouble x;
  gdouble y;

  /* This is TRUE if we are rounding everything to the nearest integer. */
  gboolean integer_layout;

  /* This is the border width used, possibly rounded to an integer. */
  gdouble border_width;

  /* The actual property value */
  gdouble prop_grid_line_width[2];

  /* The grid line width in both directions (rounded for integer layout). */
  gdouble grid_line_width[2];

  /* This is the actual spacing between the uppermost, bottommost, leftmost
     and rightmost cells from the widget border. This is the border spacing
     for that dimension. */
  gdouble border_spacing[2];

  /* These are in the table's coordinate space. */
  gdouble natural_size[2];
  gdouble requested_size[2];
  gdouble allocated_size[2];

  /* This is the last width for which we have calculated the requested heights.
     It is initialized to -1 in goo_canvas_table_init_layout_data() and
     checked/set in goo_canvas_table_update_requested_heights(). */
  gdouble last_width;
};

static GooCanvasItemIface *goo_canvas_table_parent_iface;

static void item_interface_init           (GooCanvasItemIface *iface);
static void goo_canvas_table_finalize     (GObject            *object);
static void goo_canvas_table_get_property (GObject            *object,
					   guint               param_id,
					   GValue             *value,
					   GParamSpec         *pspec);
static void goo_canvas_table_set_property (GObject            *object,
					   guint               param_id,
					   const GValue       *value,
					   GParamSpec         *pspec);

static void goo_canvas_table_init_layout_data (GooCanvasTable *table);
static void goo_canvas_table_free_layout_data (GooCanvasTableData *table_data);

G_DEFINE_TYPE_WITH_CODE (GooCanvasTable, goo_canvas_table,
			 GOO_TYPE_CANVAS_GROUP,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM,
						item_interface_init))

typedef void (*InstallChildPropertyFunc) (GObjectClass*, guint, GParamSpec*);

static void
goo_canvas_table_install_common_properties (GObjectClass *gobject_class,
					    InstallChildPropertyFunc install_child_property)
{
  /* Override from GooCanvasGroup */
  g_object_class_override_property (gobject_class, PROP_X, "x");
  g_object_class_override_property (gobject_class, PROP_Y, "y");
  g_object_class_override_property (gobject_class, PROP_WIDTH, "width");
  g_object_class_override_property (gobject_class, PROP_HEIGHT, "height");

  /* FIXME: Support setting individual row/col spacing. */
  g_object_class_install_property (gobject_class, PROP_ROW_SPACING,
                                   g_param_spec_double ("row-spacing",
							_("Row Spacing"),
							_("The default space between rows"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_COLUMN_SPACING,
                                   g_param_spec_double ("column-spacing",
							_("Column Spacing"),
							_("The default space between columns"),
							0.0, G_MAXDOUBLE, 0.0,
							G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HOMOGENEOUS_ROWS,
                                   g_param_spec_boolean ("homogeneous-rows",
							 _("Homogenous Rows"),
							 _("If all rows are the same height"),
							 FALSE,
							 G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HOMOGENEOUS_COLUMNS,
                                   g_param_spec_boolean ("homogeneous-columns",
							 _("Homogenous Columns"),
							 _("If all columns are the same width"),
							 FALSE,
							 G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_X_BORDER_SPACING,
                                   g_param_spec_double("x-border-spacing",
                                                       _("X Border Spacing"),
                                                       _("The amount of spacing between the lefmost and rightmost cells and the border grid line"),
                                                       0.0, G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_Y_BORDER_SPACING,
                                   g_param_spec_double("y-border-spacing",
                                                       _("Y Border Spacing"),
                                                       _("The amount of spacing between the topmost and bottommost cells and the border grid line"),
                                                       0.0, G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, PROP_HORZ_GRID_LINE_WIDTH,
                                   g_param_spec_double("horz-grid-line-width",
                                                        _("Horizontal Grid Line Width"),
                                                        _("The width of the grid line to draw between rows"),
                                                        0.0, G_MAXDOUBLE, 0.0,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_VERT_GRID_LINE_WIDTH,
                                   g_param_spec_double("vert-grid-line-width",
                                                       _("Vertical Grid Line Width"),
                                                       _("The width of the grid line to draw between columns"),
                                                       0.0, G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));

  /*
   * Child properties.
   */
  install_child_property (gobject_class, CHILD_PROP_LEFT_PADDING,
			  g_param_spec_double ("left-padding", 
					       _("Left Padding"), 
					       _("Extra space to add to the left of the item"),
					       0.0, G_MAXDOUBLE, 0.0,
					       G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_RIGHT_PADDING,
			  g_param_spec_double ("right-padding", 
					       _("Right Padding"), 
					       _("Extra space to add to the right of the item"),
					       0.0, G_MAXDOUBLE, 0.0,
					       G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_TOP_PADDING,
			  g_param_spec_double ("top-padding", 
					       _("Top Padding"), 
					       _("Extra space to add above the item"),
					       0.0, G_MAXDOUBLE, 0.0,
					       G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_BOTTOM_PADDING,
			  g_param_spec_double ("bottom-padding", 
					       _("Bottom Padding"), 
					       _("Extra space to add below the item"),
					       0.0, G_MAXDOUBLE, 0.0,
					       G_PARAM_READWRITE));

  install_child_property (gobject_class, CHILD_PROP_X_ALIGN,
			  g_param_spec_double ("x-align", 
					       _("X Align"), 
					       _("The horizontal position of the item within its allocated space. 0.0 is left-aligned, 1.0 is right-aligned"),
					       0.0, 1.0, 0.5,
					       G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_Y_ALIGN,
			  g_param_spec_double ("y-align", 
					       _("Y Align"), 
					       _("The vertical position of the item within its allocated space. 0.0 is top-aligned, 1.0 is bottom-aligned"),
					       0.0, 1.0, 0.5,
					       G_PARAM_READWRITE));

  install_child_property (gobject_class, CHILD_PROP_ROW,
			  g_param_spec_uint ("row", 
					     _("Row"), 
					     _("The row to place the item in"),
					     0, 65535, 0,
					     G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_COLUMN,
			  g_param_spec_uint ("column", 
					     _("Column"), 
					     _("The column to place the item in"),
					     0, 65535, 0,
					     G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_ROWS,
			  g_param_spec_uint ("rows", 
					     _("Rows"), 
					     _("The number of rows that the item spans"),
					     0, 65535, 1,
					     G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_COLUMNS,
			  g_param_spec_uint ("columns", 
					     _("Columns"), 
					     _("The number of columns that the item spans"),
					     0, 65535, 1,
					     G_PARAM_READWRITE));

  install_child_property (gobject_class, CHILD_PROP_X_EXPAND,
			  g_param_spec_boolean ("x-expand", 
						_("X Expand"), 
						_("If the item expands horizontally as the table expands"),
						FALSE,
						G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_X_FILL,
			  g_param_spec_boolean ("x-fill", 
						_("X Fill"), 
						_("If the item fills all horizontal allocated space"),
						FALSE,
						G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_X_SHRINK,
			  g_param_spec_boolean ("x-shrink", 
						_("X Shrink"), 
						_("If the item can shrink smaller than its requested size horizontally"),
						FALSE,
						G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_Y_EXPAND,
			  g_param_spec_boolean ("y-expand", 
						_("Y Expand"), 
						_("If the item expands vertically as the table expands"),
						FALSE,
						G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_Y_FILL,
			  g_param_spec_boolean ("y-fill", 
						_("Y Fill"), 
						_("If the item fills all vertical allocated space"),
						FALSE,
						G_PARAM_READWRITE));
  install_child_property (gobject_class, CHILD_PROP_Y_SHRINK,
			  g_param_spec_boolean ("y-shrink", 
						_("Y Shrink"), 
						_("If the item can shrink smaller than its requested size vertically"),
						FALSE,
						G_PARAM_READWRITE));
}


static void
goo_canvas_table_class_init (GooCanvasTableClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  goo_canvas_table_parent_iface = g_type_interface_peek (goo_canvas_table_parent_class, GOO_TYPE_CANVAS_ITEM);

  gobject_class->finalize = goo_canvas_table_finalize;
  gobject_class->get_property = goo_canvas_table_get_property;
  gobject_class->set_property = goo_canvas_table_set_property;

  goo_canvas_table_install_common_properties (gobject_class, goo_canvas_item_class_install_child_property);
}


/* This initializes the common table data. */
static void
goo_canvas_table_init_data (GooCanvasTableData *table_data)
{
  gint d;

  table_data->width = -1.0;
  table_data->height = -1.0;

  for (d = 0; d < 2; d++)
    {
      table_data->dimensions[d].size = 0;
      table_data->dimensions[d].default_spacing = 0.0;
      table_data->dimensions[d].spacings = NULL;
      table_data->dimensions[d].homogeneous = FALSE;
    }

  table_data->border_width = 0.0;

  table_data->children = g_array_new (0, 0, sizeof (GooCanvasTableChild));

  table_data->layout_data = g_slice_new (GooCanvasTableLayoutData);
  table_data->layout_data->x = 0.0;
  table_data->layout_data->y = 0.0;

  table_data->layout_data->children = NULL;
  for (d = 0; d < 2; d++)
    {
      table_data->layout_data->dldata[d] = NULL;
      table_data->layout_data->prop_grid_line_width[d] = 0.0;
      table_data->layout_data->grid_line_width[d] = 0.0;
      table_data->layout_data->border_spacing[d] = 0.0;
    }
}


/* This frees the contents of the table data, but not the struct itself. */
static void
goo_canvas_table_free_data (GooCanvasTableData *table_data)
{
  gint d;

  g_array_free (table_data->children, TRUE);

  for (d = 0; d < 2; d++)
    {
      g_free (table_data->dimensions[d].spacings);
      table_data->dimensions[d].spacings = NULL;
    }

  goo_canvas_table_free_layout_data (table_data);
}


static void
goo_canvas_table_init (GooCanvasTable *table)
{
  table->table_data = g_slice_new0 (GooCanvasTableData);
  goo_canvas_table_init_data (table->table_data);
}


/**
 * goo_canvas_table_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new table item.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a table with a square, a circle and
 * a triangle in it:
 *
 * <informalexample><programlisting>
 *  GooCanvasItem *table, *square, *circle, *triangle;
 *  
 *  table = goo_canvas_table_new (root,
 *                                "row-spacing", 4.0,
 *                                "column-spacing", 4.0,
 *                                NULL);
 *  goo_canvas_item_translate (table, 400, 200);
 *  
 *  square = goo_canvas_rect_new (table, 0.0, 0.0, 50.0, 50.0,
 *                                "fill-color", "red",
 *                                NULL);
 *  goo_canvas_item_set_child_properties (table, square,
 *                                        "row", 0,
 *                                        "column", 0,
 *                                        NULL);
 *  
 *  circle = goo_canvas_ellipse_new (table, 0.0, 0.0, 25.0, 25.0,
 *                                   "fill-color", "blue",
 *                                   NULL);
 *  goo_canvas_item_set_child_properties (table, circle,
 *                                        "row", 0,
 *                                        "column", 1,
 *                                        NULL);
 *  
 *  triangle = goo_canvas_polyline_new (table, TRUE, 3,
 *                                      25.0, 0.0, 0.0, 50.0, 50.0, 50.0,
 *                                      "fill-color", "yellow",
 *                                      NULL);
 *  goo_canvas_item_set_child_properties (table, triangle,
 *                                        "row", 0,
 *                                        "column", 2,
 *                                        NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new table item.
 **/
GooCanvasItem*
goo_canvas_table_new (GooCanvasItem  *parent,
		      ...)
{
  GooCanvasItem *item;
  va_list var_args;
  const char *first_property;

  item = g_object_new (GOO_TYPE_CANVAS_TABLE, NULL);

  va_start (var_args, parent);
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
goo_canvas_table_finalize (GObject *object)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasTable *table = (GooCanvasTable*) object;

  /* Free our data if we didn't have a model. (If we had a model it would
     have been reset in dispose() and simple_data will be NULL.) */
  if (simple->simple_data)
    {
      goo_canvas_table_free_data (table->table_data);
      g_slice_free (GooCanvasTableData, table->table_data);
    }
  table->table_data = NULL;

  G_OBJECT_CLASS (goo_canvas_table_parent_class)->finalize (object);
}


static void
goo_canvas_table_get_common_property (GObject              *object,
				      GooCanvasTableData   *table_data,
				      guint                 prop_id,
				      GValue               *value,
				      GParamSpec           *pspec)
{
  switch (prop_id)
    {
    case PROP_X:
      g_value_set_double (value, table_data->layout_data->x);
      break;
    case PROP_Y:
      g_value_set_double (value, table_data->layout_data->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, table_data->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, table_data->height);
      break;
    case PROP_ROW_SPACING:
      g_value_set_double (value, table_data->dimensions[VERT].default_spacing);
      break;
    case PROP_COLUMN_SPACING:
      g_value_set_double (value, table_data->dimensions[HORZ].default_spacing);
      break;
    case PROP_HOMOGENEOUS_ROWS:
      g_value_set_boolean (value, table_data->dimensions[VERT].homogeneous);
      break;
    case PROP_HOMOGENEOUS_COLUMNS:
      g_value_set_boolean (value, table_data->dimensions[HORZ].homogeneous);
      break;
    case PROP_X_BORDER_SPACING:
      g_value_set_double (value, table_data->layout_data->border_spacing[HORZ]);
      break;
    case PROP_Y_BORDER_SPACING:
      g_value_set_double (value, table_data->layout_data->border_spacing[VERT]);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      g_value_set_double (value, table_data->layout_data->prop_grid_line_width[HORZ]);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      g_value_set_double (value, table_data->layout_data->prop_grid_line_width[VERT]);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_table_get_property (GObject              *object,
			       guint                 prop_id,
			       GValue               *value,
			       GParamSpec           *pspec)
{
  GooCanvasTable *table = (GooCanvasTable*) object;

  goo_canvas_table_get_common_property (object, table->table_data,
                                        prop_id, value, pspec);
}


static gboolean
goo_canvas_table_set_common_property (GObject              *object,
				      GooCanvasTableData   *table_data,
				      guint                 prop_id,
				      const GValue         *value,
				      GParamSpec           *pspec)
{
  gboolean recompute_bounds = TRUE;

  switch (prop_id)
    {
    case PROP_X:
      table_data->layout_data->x = g_value_get_double (value);
      break;
    case PROP_Y:
      table_data->layout_data->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      table_data->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      table_data->height = g_value_get_double (value);
      break;
    case PROP_ROW_SPACING:
      table_data->dimensions[VERT].default_spacing = g_value_get_double (value);
      break;
    case PROP_COLUMN_SPACING:
      table_data->dimensions[HORZ].default_spacing = g_value_get_double (value);
      break;
    case PROP_HOMOGENEOUS_ROWS:
      table_data->dimensions[VERT].homogeneous = g_value_get_boolean (value);
      break;
    case PROP_HOMOGENEOUS_COLUMNS:
      table_data->dimensions[HORZ].homogeneous = g_value_get_boolean (value);
      break;
    case PROP_X_BORDER_SPACING:
      table_data->layout_data->border_spacing[HORZ] = g_value_get_double (value);
      break;
    case PROP_Y_BORDER_SPACING:
      table_data->layout_data->border_spacing[VERT] = g_value_get_double (value);
      break;
    case PROP_HORZ_GRID_LINE_WIDTH:
      table_data->layout_data->prop_grid_line_width[HORZ] = g_value_get_double (value);
      break;
    case PROP_VERT_GRID_LINE_WIDTH:
      table_data->layout_data->prop_grid_line_width[VERT] = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  return recompute_bounds;
}


static void
goo_canvas_table_set_property (GObject              *object,
			       guint                 prop_id,
			       const GValue         *value,
			       GParamSpec           *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasTable *table = (GooCanvasTable*) object;
  gboolean recompute_bounds;

  if (simple->model)
    {
      g_warning ("Can't set property of a canvas item with a model - set the model property instead");
      return;
    }

  recompute_bounds = goo_canvas_table_set_common_property (object,
							   table->table_data,
							   prop_id, value,
							   pspec);
  goo_canvas_item_simple_changed (simple, recompute_bounds);
}


static void
goo_canvas_table_update_dimensions (GooCanvasTableData    *table_data,
				    GooCanvasTableChild   *table_child)
{
  GooCanvasTableLayoutData *layout_data;
  gint d, size[2], i;
  layout_data = table_data->layout_data;
  size[0] = table_child->start[0] + table_child->size[0];
  size[1] = table_child->start[1] + table_child->size[1];

  for (d = 0; d < 2; d++)
    {
      if (size[d] > table_data->dimensions[d].size)
        {
	  /* Resize the spacings array and the layout data array. */
          table_data->dimensions[d].spacings = g_realloc (table_data->dimensions[d].spacings, size[d] * sizeof (gdouble));
          layout_data->dldata[d] = g_renew (GooCanvasTableDimensionLayoutData, layout_data->dldata[d], size[d]);

          /* Initialize new spacings to -1.0 so the default is used, and
	     set the new grid_line_visibility arrays to NULL. */
          for (i = table_data->dimensions[d].size; i < size[d]; i++)
	    {
	      table_data->dimensions[d].spacings[i] = -1.0;
	      layout_data->dldata[d][i].grid_line_visibility = NULL;
	    }
        }
    }

  table_data->dimensions[0].size = MAX (size[0], table_data->dimensions[0].size);
  table_data->dimensions[1].size = MAX (size[1], table_data->dimensions[1].size);
}

/* Sets or unsets grid line visibility for a given child */
static void
goo_canvas_update_grid_line_visibility (GooCanvasTableData *table_data)
{
  GooCanvasTableLayoutData *layout_data;
  GooCanvasTableChild *table_child;
  guint32 grid_line_size;
  gint d, d2, child_num, i, j;

  layout_data = table_data->layout_data;
  for (d = 0; d < 2; d++)
    {
      /* This is the opposite dimension. */
      d2 = 1 - d;

      /* The amount of guint32s we need to store grid line visibility
         for a particular row or column. In case of vertical grid lines
         we store the grid lines (right to) a particular column for each row.
         Therefore we need one bit for each row. This is also why this depends
         on dimensions[1-d] instead of dimensions[d]. */
      grid_line_size = ((table_data->dimensions[d2].size + 31) / 32);

      /* Allocate or reallocate the grid_line_visibility arrays and initialize
	 all grid lines to visible (by setting to the arrays to all 0xFF). */
      for (i = 0; i + 1 < table_data->dimensions[d].size; i++)
	{
	  layout_data->dldata[d][i].grid_line_visibility
	    = g_realloc (layout_data->dldata[d][i].grid_line_visibility,
			 grid_line_size * sizeof (guint32));
	  memset (layout_data->dldata[d][i].grid_line_visibility, 0xff,
		  grid_line_size * sizeof (guint32));
	}

      /* Remove lines for each child spanning multiple rows/colmuns */
      for (child_num = 0; child_num < table_data->children->len; child_num++)
        {
          table_child = &g_array_index (table_data->children,
					GooCanvasTableChild, child_num);

          /* Foreach cell the child is spanning */
          for (i = table_child->start[d];
	       i < table_child->start[d] + table_child->size[d] - 1;
	       i++)
            {
              /* Iterate through occupied cells in the other dimension */
              for (j = table_child->start[d2];
		   j < table_child->start[d2] + table_child->size[d2];
		   j++)
                {
                  GOO_CANVAS_TABLE_UNSET_GRID_LINE_VISIBILITY (layout_data->dldata[d], i, j);
                }
            }
        }
    }
}

static void
goo_canvas_table_add_child_internal (GooCanvasTableData *table_data,
				     gint                position)
{
  GooCanvasTableChild table_child;
  gint d;

  /* Initialize a table child struct. */
  for (d = 0; d < 2; d++)
    {
      table_child.start_pad[d] = 0.0;
      table_child.end_pad[d] = 0.0;
      table_child.align[d] = 0.5;
      table_child.start[d] = 0;
      table_child.size[d] = 1;

      /* Unlike GtkTable our children do not expand & fill by default,
	 as that makes more sense with the builtin simple items. */
      table_child.flags[d] = 0;
    }

  if (position < 0)
    position = table_data->children->len;
  g_array_insert_val (table_data->children, position, table_child);

  goo_canvas_table_update_dimensions (table_data, &table_child);
}


static void
goo_canvas_table_add_child     (GooCanvasItem  *item,
				GooCanvasItem  *child,
				gint            position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;

  if (!simple->model)
    goo_canvas_table_add_child_internal (table->table_data, position);

  /* Let the parent GooCanvasGroup code do the rest. */
  goo_canvas_table_parent_iface->add_child (item, child, position);
}


static void
goo_canvas_table_move_child_internal    (GooCanvasTableData *table_data,
					 gint	             old_position,
					 gint                new_position)
{
  GooCanvasTableChild *table_child, tmp_child;

  /* Copy the child temporarily. */
  table_child = &g_array_index (table_data->children, GooCanvasTableChild,
				old_position);
  tmp_child = *table_child;

  /* Move the array data up or down. */
  if (new_position > old_position)
    {
      /* Move the items down one place. */
      g_memmove (table_child,
		 &g_array_index (table_data->children, GooCanvasTableChild,
				 old_position + 1),
		 sizeof (GooCanvasTableChild) * (new_position - old_position));
    }
  else
    {
      /* Move the items up one place. */
      g_memmove (&g_array_index (table_data->children, GooCanvasTableChild,
				 new_position + 1),
		 &g_array_index (table_data->children, GooCanvasTableChild,
				 new_position),
		 sizeof (GooCanvasTableChild) * (old_position - new_position));
    }

  /* Copy the child into its new position. */
  table_child = &g_array_index (table_data->children, GooCanvasTableChild,
				new_position);
  *table_child = tmp_child;
}


static void
goo_canvas_table_move_child    (GooCanvasItem  *item,
				gint	        old_position,
				gint            new_position)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;

  if (!simple->model)
    goo_canvas_table_move_child_internal (table->table_data, old_position,
					  new_position);

  /* Let the parent GooCanvasGroup code do the rest. */
  goo_canvas_table_parent_iface->move_child (item, old_position, new_position);
}


static void
goo_canvas_table_remove_child  (GooCanvasItem  *item,
				gint            child_num)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;

  g_return_if_fail (child_num < group->items->len);

  if (!simple->model)
    g_array_remove_index (table->table_data->children, child_num);

  /* Let the parent GooCanvasGroup code do the rest. */
  goo_canvas_table_parent_iface->remove_child (item, child_num);
}


static void
goo_canvas_table_get_common_child_property (GObject             *object,
					    GooCanvasTableChild *table_child,
					    guint                property_id,
					    GValue              *value,
					    GParamSpec          *pspec)
{
  switch (property_id)
    {
    case CHILD_PROP_LEFT_PADDING:
      g_value_set_double (value, table_child->start_pad[HORZ]);
      break;
    case CHILD_PROP_RIGHT_PADDING:
      g_value_set_double (value, table_child->end_pad[HORZ]);
      break;
    case CHILD_PROP_TOP_PADDING:
      g_value_set_double (value, table_child->start_pad[VERT]);
      break;
    case CHILD_PROP_BOTTOM_PADDING:
      g_value_set_double (value, table_child->end_pad[VERT]);
      break;
    case CHILD_PROP_X_ALIGN:
      g_value_set_double (value, table_child->align[HORZ]);
      break;
    case CHILD_PROP_Y_ALIGN:
      g_value_set_double (value, table_child->align[VERT]);
      break;
    case CHILD_PROP_ROW:
      g_value_set_uint (value, table_child->start[VERT]);
      break;
    case CHILD_PROP_COLUMN:
      g_value_set_uint (value, table_child->start[HORZ]);
      break;
    case CHILD_PROP_ROWS:
      g_value_set_uint (value, table_child->size[VERT]);
      break;
    case CHILD_PROP_COLUMNS:
      g_value_set_uint (value, table_child->size[HORZ]);
      break;
    case CHILD_PROP_X_EXPAND:
      g_value_set_boolean (value, table_child->flags[HORZ] & GOO_CANVAS_TABLE_CHILD_EXPAND);
      break;
    case CHILD_PROP_X_FILL:
      g_value_set_boolean (value, table_child->flags[HORZ] & GOO_CANVAS_TABLE_CHILD_FILL);
      break;
    case CHILD_PROP_X_SHRINK:
      g_value_set_boolean (value, table_child->flags[HORZ] & GOO_CANVAS_TABLE_CHILD_SHRINK);
      break;
    case CHILD_PROP_Y_EXPAND:
      g_value_set_boolean (value, table_child->flags[VERT] & GOO_CANVAS_TABLE_CHILD_EXPAND);
      break;
    case CHILD_PROP_Y_FILL:
      g_value_set_boolean (value, table_child->flags[VERT] & GOO_CANVAS_TABLE_CHILD_FILL);
      break;
    case CHILD_PROP_Y_SHRINK:
      g_value_set_boolean (value, table_child->flags[VERT] & GOO_CANVAS_TABLE_CHILD_SHRINK);
      break;
    default:
      G_OBJECT_WARN_INVALID_PSPEC ((object), "child property id",
				   (property_id), (pspec));
      break;
    }
}


static void
goo_canvas_table_get_child_property (GooCanvasItem     *item,
				     GooCanvasItem     *child,
				     guint              property_id,
				     GValue            *value,
				     GParamSpec        *pspec)
{
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableChild *table_child;
  gint child_num;

  for (child_num = 0; child_num < group->items->len; child_num++)
    {
      if (group->items->pdata[child_num] == child)
	{
	  table_child = &g_array_index (table->table_data->children,
					GooCanvasTableChild, child_num);
	  goo_canvas_table_get_common_child_property ((GObject*) table,
						      table_child,
						      property_id, value,
						      pspec);
	  break;
	}
    }
}


static void
goo_canvas_table_set_common_child_property (GObject             *object,
					    GooCanvasTableData  *table_data,
					    GooCanvasTableChild *table_child,
					    guint                property_id,
					    const GValue        *value,
					    GParamSpec          *pspec)
{
  switch (property_id)
    {
    case CHILD_PROP_LEFT_PADDING:
      table_child->start_pad[HORZ] = g_value_get_double (value);
      break;
    case CHILD_PROP_RIGHT_PADDING:
      table_child->end_pad[HORZ] = g_value_get_double (value);
      break;
    case CHILD_PROP_TOP_PADDING:
      table_child->start_pad[VERT] = g_value_get_double (value);
      break;
    case CHILD_PROP_BOTTOM_PADDING:
      table_child->end_pad[VERT] = g_value_get_double (value);
      break;
    case CHILD_PROP_X_ALIGN:
      table_child->align[HORZ] = g_value_get_double (value);
      break;
    case CHILD_PROP_Y_ALIGN:
      table_child->align[VERT] = g_value_get_double (value);
      break;
    case CHILD_PROP_ROW:
      table_child->start[VERT] = g_value_get_uint (value);
      break;
    case CHILD_PROP_COLUMN:
      table_child->start[HORZ] = g_value_get_uint (value);
      break;
    case CHILD_PROP_ROWS:
      table_child->size[VERT] = g_value_get_uint (value);
      break;
    case CHILD_PROP_COLUMNS:
      table_child->size[HORZ] = g_value_get_uint (value);
      break;

    case CHILD_PROP_X_EXPAND:
      if (g_value_get_boolean (value))
	table_child->flags[HORZ] |= GOO_CANVAS_TABLE_CHILD_EXPAND;
      else
	table_child->flags[HORZ] &= ~GOO_CANVAS_TABLE_CHILD_EXPAND;
      break;
    case CHILD_PROP_X_FILL:
      if (g_value_get_boolean (value))
	table_child->flags[HORZ] |= GOO_CANVAS_TABLE_CHILD_FILL;
      else
	table_child->flags[HORZ] &= ~GOO_CANVAS_TABLE_CHILD_FILL;
      break;
    case CHILD_PROP_X_SHRINK:
      if (g_value_get_boolean (value))
	table_child->flags[HORZ] |= GOO_CANVAS_TABLE_CHILD_SHRINK;
      else
	table_child->flags[HORZ] &= ~GOO_CANVAS_TABLE_CHILD_SHRINK;
      break;

    case CHILD_PROP_Y_EXPAND:
      if (g_value_get_boolean (value))
	table_child->flags[VERT] |= GOO_CANVAS_TABLE_CHILD_EXPAND;
      else
	table_child->flags[VERT] &= ~GOO_CANVAS_TABLE_CHILD_EXPAND;
      break;
    case CHILD_PROP_Y_FILL:
      if (g_value_get_boolean (value))
	table_child->flags[VERT] |= GOO_CANVAS_TABLE_CHILD_FILL;
      else
	table_child->flags[VERT] &= ~GOO_CANVAS_TABLE_CHILD_FILL;
      break;
    case CHILD_PROP_Y_SHRINK:
      if (g_value_get_boolean (value))
	table_child->flags[VERT] |= GOO_CANVAS_TABLE_CHILD_SHRINK;
      else
	table_child->flags[VERT] &= ~GOO_CANVAS_TABLE_CHILD_SHRINK;
      break;

    default:
      G_OBJECT_WARN_INVALID_PSPEC ((object), "child property id",
				   (property_id), (pspec));
      break;
    }

  goo_canvas_table_update_dimensions (table_data, table_child);
}


static void
goo_canvas_table_set_child_property (GooCanvasItem     *item,
				     GooCanvasItem     *child,
				     guint              property_id,
				     const GValue      *value,
				     GParamSpec        *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableChild *table_child;
  gint child_num;

  for (child_num = 0; child_num < group->items->len; child_num++)
    {
      if (group->items->pdata[child_num] == child)
	{
	  table_child = &g_array_index (table->table_data->children,
					GooCanvasTableChild, child_num);
	  goo_canvas_table_set_common_child_property ((GObject*) table,
						      table->table_data,
						      table_child,
						      property_id, value,
						      pspec);
	  break;
	}
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_table_set_model    (GooCanvasItem      *item,
			       GooCanvasItemModel *model)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;

  /* If our table_data was allocated, free it. */
  if (!simple->model)
    {
      goo_canvas_table_free_data (table->table_data);
      g_slice_free (GooCanvasTableData, table->table_data);
    }

  /* Now use the new model's table_data instead. */
  table->table_data = &tmodel->table_data;

  /* Let the parent GooCanvasGroup code do the rest. */
  goo_canvas_table_parent_iface->set_model (item, model);
}


/*
 * Size requisition/allocation code.
 */

static void
goo_canvas_table_free_layout_data (GooCanvasTableData *table_data)
{
  gint i;

  if (table_data->layout_data)
    {
      for (i = 0; i < table_data->dimensions[VERT].size; i++)
        g_free (table_data->layout_data->dldata[VERT][i].grid_line_visibility);
      for (i = 0; i < table_data->dimensions[HORZ].size; i++)
        g_free (table_data->layout_data->dldata[HORZ][i].grid_line_visibility);
      g_free (table_data->layout_data->dldata[HORZ]);
      g_free (table_data->layout_data->dldata[VERT]);
      g_free (table_data->layout_data->children);
      g_slice_free (GooCanvasTableLayoutData, table_data->layout_data);
      table_data->layout_data = NULL;
    }
}


/* This allocates the layout data, and initializes the row and column data. */
static void
goo_canvas_table_init_layout_data (GooCanvasTable *table)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) table;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableDimension *dimension;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *dldata;
  gint d, i;

  layout_data->children = g_new (GooCanvasTableChildLayoutData,
				 table_data->children->len);
  layout_data->last_width = -1;

  /* If we are not yet added to a canvas, integer layout is irrelevant anyway.
     We still need the layout_data for private fields, such as border spacing
     and grid line visibility. */
  if (simple->canvas)
    layout_data->integer_layout = simple->canvas->integer_layout;
  else
    layout_data->integer_layout = FALSE;
  layout_data->border_width = table_data->border_width;
  if (layout_data->integer_layout)
    layout_data->border_width = floor (layout_data->border_width + 0.5);

  layout_data->grid_line_width[0] = layout_data->prop_grid_line_width[0];
  layout_data->grid_line_width[1] = layout_data->prop_grid_line_width[1];
  if (layout_data->integer_layout)
    {
      layout_data->grid_line_width[0] = floor (layout_data->grid_line_width[0] + 0.5);
      layout_data->grid_line_width[1] = floor (layout_data->grid_line_width[1] + 0.5);
    }

  for (d = 0; d < 2; d++)
    {
      dimension = &table_data->dimensions[d];

      /* Already allocated in goo_canvas_table_update_dimensions() */
      /*layout_data->dldata[d] = g_renew (GooCanvasTableDimensionLayoutData,
                                        layout_data->dldata[d],
				        dimension->size);*/
      dldata = layout_data->dldata[d];

      for (i = 0; i < dimension->size; i++)
	{
	  /* Calculate the actual spacings. If -ve use the default. */
	  if (dimension->spacings[i] > 0)
	    dldata[i].spacing = dimension->spacings[i];
	  else
	    dldata[i].spacing = dimension->default_spacing;

          /* Add grid line widths to spacing */
          dldata[i].spacing += layout_data->prop_grid_line_width[1-d];

	  /* In integer layout mode, round spacings to the nearest integer. */
	  if (layout_data->integer_layout)
	    dldata[i].spacing = floor (dldata[i].spacing + 0.5);

	  dldata[i].need_expand = FALSE;
	  dldata[i].need_shrink = TRUE;
	  dldata[i].expand = FALSE;
	  dldata[i].shrink = TRUE;
	  dldata[i].empty = TRUE;
	}
    }

  /* Update grid line visibility */
  goo_canvas_update_grid_line_visibility (table_data);
}


/* This gets the requested size of all child items, and sets the expand,
   shrink and empty flags for each row and column.
   It should only be called once in the entire size_request/allocate procedure
   and nothing should change the values it sets (except the requested height
   of children may change after calling get_requested_height() on them). */
static void
goo_canvas_table_size_request_init (GooCanvasTable *table,
				    cairo_t        *cr)
{
  GooCanvasGroup *group = (GooCanvasGroup*) table;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimension *dimension;
  GooCanvasTableDimensionLayoutData *dldata;
  GooCanvasTableChild *child;
  GooCanvasItem *child_item;
  GooCanvasBounds bounds;
  gboolean allocate, has_expand, has_shrink;
  gint i, j, d, start, end;
  guint8 flags;

  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);
      child_item = group->items->pdata[i];

      /* Children will return FALSE if they don't need space allocated. */
      allocate = goo_canvas_item_get_requested_area (child_item, cr, &bounds);

      /* Remember the requested position and size of the child. */
      layout_data->children[i].requested_position[HORZ] = bounds.x1;
      layout_data->children[i].requested_position[VERT] = bounds.y1;

      if (!allocate)
	{
	  layout_data->children[i].requested_size[HORZ] = -1.0;
	  layout_data->children[i].requested_size[VERT] = -1.0;
	  continue;
	}

      layout_data->children[i].requested_size[HORZ] = bounds.x2 - bounds.x1;
      layout_data->children[i].requested_size[VERT] = bounds.y2 - bounds.y1;

      layout_data->children[i].start_pad[HORZ] = child->start_pad[HORZ];
      layout_data->children[i].end_pad[HORZ] = child->end_pad[HORZ];
      layout_data->children[i].start_pad[VERT] = child->start_pad[VERT];
      layout_data->children[i].end_pad[VERT] = child->end_pad[VERT];

      if (layout_data->integer_layout)
	{
	  layout_data->children[i].requested_size[HORZ] = ceil (layout_data->children[i].requested_size[HORZ]);
	  layout_data->children[i].requested_size[VERT] = ceil (layout_data->children[i].requested_size[VERT]);

	  layout_data->children[i].start_pad[HORZ] = floor (layout_data->children[i].start_pad[HORZ] + 0.5);
	  layout_data->children[i].end_pad[HORZ] = floor (layout_data->children[i].end_pad[HORZ] + 0.5);
	  layout_data->children[i].start_pad[VERT] = floor (layout_data->children[i].start_pad[VERT] + 0.5);
	  layout_data->children[i].end_pad[VERT] = floor (layout_data->children[i].end_pad[VERT] + 0.5);
	}

      /* Set the expand, shrink & empty flags in the
	 GooCanvasTableDimensionLayoutData for the row/column, if the item
	 only spans 1 row/column. */
      for (d = 0; d < 2; d++)
	{
	  dldata = layout_data->dldata[d];
	  start = child->start[d];
	  flags = child->flags[d];

	  if (child->size[d] == 1)
	    {
	      if (flags & GOO_CANVAS_TABLE_CHILD_EXPAND)
		dldata[start].expand = TRUE;
	      if (!(flags & GOO_CANVAS_TABLE_CHILD_SHRINK))
		dldata[start].shrink = FALSE;
	      dldata[start].empty = FALSE;
	    }
	}
    }

  /* Now handle children that span more than one row or column. */
  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);

      if (layout_data->children[i].requested_size[HORZ] < 0.0)
	continue;

      for (d = 0; d < 2; d++)
	{
	  dldata = layout_data->dldata[d];
	  start = child->start[d];
	  end = start + child->size[d] - 1;
	  flags = child->flags[d];

	  if (child->size[d] != 1)
	    {
	      has_expand = FALSE;
	      has_shrink = TRUE;

	      for (j = start; j <= end; j++)
		{
		  dldata[j].empty = FALSE;
		  if (dldata[j].expand)
		    has_expand = TRUE;
		  if (!dldata[j].shrink)
		    has_shrink = FALSE;
		}

	      /* If the child expands, but none of the rows/columns already
		 has expand set, we set need_expand for all of them. */
	      if (!has_expand && (flags & GOO_CANVAS_TABLE_CHILD_EXPAND))
		{
		  for (j = start; j <= end; j++)
		    dldata[j].need_expand = TRUE;
		}

	      /* If the child doesn't shrink, but all of the rows/columns have
		 shrink set, we set need_shrink to FALSE for all of them. */
	      if (has_shrink && !(flags & GOO_CANVAS_TABLE_CHILD_SHRINK))
		{
		  for (j = start; j <= end; j++)
		    dldata[j].need_shrink = FALSE;
		}
	    }
	}
    }

  /* Now set the final expand, shrink & empty flags. They shouldn't be
     changed after this point. */
  for (d = 0; d < 2; d++)
    {
      dimension = &table_data->dimensions[d];
      dldata = layout_data->dldata[d];

      for (i = 0; i < dimension->size; i++)
	{
	  if (dldata[i].empty)
	    {
	      dldata[i].expand = FALSE;
	      dldata[i].shrink = FALSE;
	    }
	  else
	    {
	      if (dldata[i].need_expand)
		dldata[i].expand = TRUE;
	      if (!dldata[i].need_shrink)
		dldata[i].shrink = FALSE;
	    }
	}
    }
}


/* This calculates the initial size request for each row & column, which is
   the maximum size of all items that are in that row or column. (It skips
   items that span more than one row or column.) */
static void
goo_canvas_table_size_request_pass1 (GooCanvasTable *table,
				     gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableDimension *dimension = &table_data->dimensions[d];
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *dldata = layout_data->dldata[d];
  GooCanvasTableChild *child;
  gdouble requested_size;
  gint i, start;

  for (i = 0; i < dimension->size; i++)
    dldata[i].requisition = 0.0;

  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);

      requested_size = layout_data->children[i].requested_size[d];

      /* Child needs allocation & spans a single row or column. */
      if (requested_size >= 0.0 && child->size[d] == 1)
	{
	  requested_size += layout_data->children[i].start_pad[d]
	    + layout_data->children[i].end_pad[d];
	  start = child->start[d];
	  dldata[start].requisition = MAX (dldata[start].requisition,
					   requested_size);
	}
    }
}


/* This ensures that homogeneous tables have all rows/columns the same size.
   Note that it is called once after items in a single row/column have been
   calculated, and then again after items spanning several rows/columns have
   been calculated. */
static void
goo_canvas_table_size_request_pass2 (GooCanvasTable *table,
				     gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *dldata = layout_data->dldata[d];
  gdouble max_size = 0.0;
  gint i;
  
  if (table_data->dimensions[d].homogeneous)
    {
      /* Calculate the maximum row or column size. */
      for (i = 0; i < table_data->dimensions[d].size; i++)
	max_size = MAX (max_size, dldata[i].requisition);

      /* Use the maximum size for all rows or columns. */
      for (i = 0; i < table_data->dimensions[d].size; i++)
	dldata[i].requisition = max_size;
    }
}


/* This handles items that span more than one row or column, by making sure
   there is enough space for them and expanding the row/column size request
   if needed. */
static void
goo_canvas_table_size_request_pass3 (GooCanvasTable *table,
				     gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *dldata;
  GooCanvasTableChild *child;
  gint i, j;
  
  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);
      
      if (layout_data->children[i].requested_size[HORZ] <= 0.0)
	continue;

      /* Child spans multiple rows/columns. */
      if (child->size[d] != 1)
	{
	  gint start = child->start[d];
	  gint end = start + child->size[d] - 1;
	  gdouble total_space = 0.0, space_needed;

	  dldata = layout_data->dldata[d];

	  /* Check if there is already enough space for the child. */
	  for (j = start; j <= end; j++)
	    {
	      total_space += dldata[j].requisition;
	      if (j < end)
		total_space += dldata[j].spacing;
	    }
	      
	  /* If we need to request more space for this child to fill
	     its requisition, then divide up the needed space amongst the
	     columns it spans, favoring expandable columns if any. */
	  space_needed = layout_data->children[i].requested_size[d]
	    + layout_data->children[i].start_pad[d]
	    + layout_data->children[i].end_pad[d];

	  if (total_space < space_needed)
	    {
	      gint n_expand = 0;
	      gboolean force_expand = FALSE;
	      gdouble expand = space_needed - total_space;
	      gdouble extra;

	      for (j = start; j <= end; j++)
		{
		  if (dldata[j].expand)
		    n_expand++;
		}

	      if (n_expand == 0)
		{
		  n_expand = child->size[d];
		  force_expand = TRUE;
		}
		    
	      if (layout_data->integer_layout)
		{
		  for (j = start; j <= end; j++)
		    {
		      if (force_expand || dldata[j].expand)
			{
			  extra = floor (expand / n_expand + 0.5);
			  dldata[j].requisition += extra;
			  expand -= extra;
			  n_expand--;
			}
		    }
		}
	      else
		{
		  extra = expand / n_expand;
		  for (j = start; j <= end; j++)
		    {
		      if (force_expand || dldata[j].expand)
			dldata[j].requisition += extra;
		    }
		}
	    }
	}
    }
}


static void
goo_canvas_table_size_allocate_init (GooCanvasTable *table,
				     gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimension *dimension = &table_data->dimensions[d];
  GooCanvasTableDimensionLayoutData *dldata = layout_data->dldata[d];
  gint i;
  
  /* Set the initial allocation, by copying over the requisition.
     Also set the final expand & shrink flags. */
  for (i = 0; i < dimension->size; i++)
    dldata[i].allocation = dldata[i].requisition;
}


static void
goo_canvas_table_size_allocate_pass1 (GooCanvasTable *table,
				      gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimension *dimension;
  GooCanvasTableDimensionLayoutData *dldata;
  gdouble total_size, size_to_allocate, natural_size, extra, old_extra;
  gint i, nexpand, nshrink;
  
  /* If we were allocated more space than we requested
   *  then we have to expand any expandable rows and columns
   *  to fill in the extra space.
   */
  dimension = &table_data->dimensions[d];
  dldata = layout_data->dldata[d];

  natural_size = 0;
  nexpand = 0;
  nshrink = 0;

  for (i = 0; i < dimension->size; i++)
    {
      natural_size += dldata[i].requisition;
      if (dldata[i].expand)
	nexpand += 1;
      if (dldata[i].shrink && dldata[i].allocation > 0.0)
	nshrink += 1;
    }
  for (i = 0; i + 1 < dimension->size; i++)
    natural_size += dldata[i].spacing;

  /* Note: natural_size does not contain without border width and
     border spacing here. */

  /* total_size is the size available for allocating widgets. We always
     allocate space for border_width, but for right/bottom border_spacing only
     if all children can be allocated without being shrunk. */
  if (layout_data->allocated_size[d] < layout_data->border_width * 2.0 + layout_data->border_spacing[d] + layout_data->grid_line_width[1-d])
    total_size = 0;
  else if (layout_data->allocated_size[d] < layout_data->border_width * 2.0 + layout_data->border_spacing[d] + layout_data->grid_line_width[1-d] + natural_size)
    total_size = layout_data->allocated_size[d] - layout_data->border_width * 2.0 - layout_data->border_spacing[d] - layout_data->grid_line_width[1-d];
  else if (layout_data->allocated_size[d] < layout_data->border_width * 2.0 + (layout_data->border_spacing[d] + layout_data->grid_line_width[1-d]) * 2.0 + natural_size)
    total_size = natural_size;
  else
    total_size = layout_data->allocated_size[d] - layout_data->border_width * 2.0 - (layout_data->border_spacing[d] + layout_data->grid_line_width[1-d])* 2.0;

  if (dimension->homogeneous)
    {
      /* If the table is homogeneous in this dimension we check if any of
	 the children expand, or if there are no children, or if we were
	 allocated less space than we requested and any children shrink.
	 If so, we divide up all the allocated space. */
      if (nexpand || table_data->children->len == 0
	  || (natural_size > total_size && nshrink))
	{
	  size_to_allocate = total_size;
	  for (i = 0; i + 1 < dimension->size; i++)
	    size_to_allocate -= dldata[i].spacing;
	  
	  if (layout_data->integer_layout)
	    {
	      gint n_elements = dimension->size;
	      for (i = 0; i < dimension->size; i++)
		{
		  dldata[i].allocation = floor (size_to_allocate / n_elements + 0.5);
		  size_to_allocate -= dldata[i].allocation;
		  n_elements--;
		}
	    }
	  else
	    {
	      size_to_allocate /= dimension->size;
	      for (i = 0; i < dimension->size; i++)
		dldata[i].allocation = size_to_allocate;
	    }
	}
    }
  else
    {
      /* Check to see if we were allocated more width than we requested.
       */
      if ((natural_size < total_size) && (nexpand >= 1))
	{
	  if (layout_data->integer_layout)
	    {
	      gdouble expand = total_size - natural_size;
	      for (i = 0; i < dimension->size; i++)
		{
		  if (dldata[i].expand)
		    {
		      extra = floor (expand / nexpand + 0.5);
		      dldata[i].allocation += extra;
		      expand -= extra;
		      nexpand--;
		    }
		}
	    }
	  else
	    {
	      extra = (total_size - natural_size) / nexpand;
	      for (i = 0; i < dimension->size; i++)
		{
		  if (dldata[i].expand)
		    dldata[i].allocation += extra;
		}
	    }
	}
	  
      /* Check to see if we were allocated less width than we requested,
       * then shrink until we fit the size give.
       */
      if (natural_size > total_size)
	{
	  gint total_nshrink = nshrink;
	      
	  extra = natural_size - total_size;
	  while (total_nshrink > 0 && extra > 0)
	    {
	      nshrink = total_nshrink;
	      old_extra = extra;
	      for (i = 0; i < dimension->size; i++)
		{
		  if (dldata[i].shrink && dldata[i].allocation > 0.0)
		    {
		      gdouble old_allocation = dldata[i].allocation;
		      gdouble shrink_amount = extra / nshrink;
		      gdouble new_allocation;

		      if (layout_data->integer_layout)
			shrink_amount = floor (shrink_amount + 0.5);

		      new_allocation = old_allocation - shrink_amount;
		      dldata[i].allocation = MAX (0.0, new_allocation);
		      extra -= old_allocation - dldata[i].allocation;
		      nshrink -= 1;
		      if (dldata[i].allocation <= 0.0)
			total_nshrink -= 1;
		    }
		}
	      if (extra >= old_extra)
		break;
	    }
	}
    }
}


/* Calculates the start and end position of each row/column. */
static void
goo_canvas_table_size_allocate_pass2 (GooCanvasTable *table,
				      gint            d)
{
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimension *dimension;
  GooCanvasTableDimensionLayoutData *dldata;
  gdouble pos;
  gint i;

  dimension = &table_data->dimensions[d];
  dldata = layout_data->dldata[d];

  pos = layout_data->border_width + layout_data->border_spacing[d] + layout_data->grid_line_width[1-d];
  for (i = 0; i < dimension->size; i++)
    {
      dldata[i].start = pos;
      pos += dldata[i].allocation;
      dldata[i].end = pos;
      pos += dldata[i].spacing;

#if 0
      g_print ("%s %i: %g - %g\n", d ? "Row" : "Column", i,
	       dldata[i].start, dldata[i].end);
#endif
    }
}


/* This does the actual allocation of each child, calculating its size and
   position according to the rows & columns it spans. */
static void
goo_canvas_table_size_allocate_pass3 (GooCanvasTable *table,
				      cairo_t        *cr,
				      gdouble         table_x_offset,
				      gdouble         table_y_offset)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) table;
  GooCanvasGroup *group = (GooCanvasGroup*) table;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *rows = layout_data->dldata[VERT];
  GooCanvasTableDimensionLayoutData *columns = layout_data->dldata[HORZ];
  GooCanvasTableChild *child;
  GooCanvasItem *child_item;
  GooCanvasTableChildLayoutData *child_data;
  GooCanvasBounds requested_area, allocated_area;
  GtkTextDirection direction = GTK_TEXT_DIR_NONE;
  gint start_column, end_column, start_row, end_row, i;
  gdouble x, y, max_width, max_height, width, height;
  gdouble requested_width, requested_height;
  gdouble x_offset, y_offset;

  if (simple->canvas)
    direction = gtk_widget_get_direction (GTK_WIDGET (simple->canvas));

  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);
      child_item = group->items->pdata[i];
      child_data = &layout_data->children[i];

      requested_width = child_data->requested_size[HORZ];
      requested_height = child_data->requested_size[VERT];

      if (requested_width <= 0.0)
	continue;
      
      start_column = child->start[HORZ];
      end_column = child->start[HORZ] + child->size[HORZ] - 1;
      x = columns[start_column].start + layout_data->children[i].start_pad[HORZ];
      max_width = columns[end_column].end - layout_data->children[i].end_pad[HORZ] - x;

      start_row = child->start[VERT];
      end_row = child->start[VERT] + child->size[VERT] - 1;
      y = rows[start_row].start + layout_data->children[i].start_pad[VERT];
      max_height = rows[end_row].end - layout_data->children[i].end_pad[VERT] - y;

      width = max_width = MAX (0.0, max_width);
      height = max_height = MAX (0.0, max_height);

      if (!(child->flags[HORZ] & GOO_CANVAS_TABLE_CHILD_FILL))
	{
	  width = MIN (max_width, requested_width);
	  x += (max_width - width) * child->align[HORZ];

	  if (layout_data->integer_layout)
	    x = floor (x + 0.5);
	}
	  
      if (!(child->flags[VERT] & GOO_CANVAS_TABLE_CHILD_FILL))
	{
	  height = MIN (max_height, requested_height);
	  y += (max_height - height) * child->align[VERT];

	  if (layout_data->integer_layout)
	    y = floor (y + 0.5);
	}

      if (direction == GTK_TEXT_DIR_RTL)
	x = layout_data->allocated_size[HORZ] - width;

      requested_area.x1 = layout_data->children[i].requested_position[HORZ];
      requested_area.y1 = layout_data->children[i].requested_position[VERT];
      requested_area.x2 = requested_area.x1 + requested_width;
      requested_area.y2 = requested_area.y1 + requested_height;

      allocated_area.x1 = x;
      allocated_area.y1 = y;
      allocated_area.x2 = allocated_area.x1 + width;
      allocated_area.y2 = allocated_area.y1 + height;

      /* Calculate how much we will have to translate the child by. */
      child->position[HORZ] = allocated_area.x1 - requested_area.x1;
      child->position[VERT] = allocated_area.y1 - requested_area.y1;

      /* Translate the cairo context so the item's user space is correct. */
      cairo_translate (cr, child->position[HORZ], child->position[VERT]);

      x_offset = allocated_area.x1 - requested_area.x1;
      y_offset = allocated_area.y1 - requested_area.y1;
      cairo_user_to_device_distance (cr, &x_offset, &y_offset);
      x_offset += table_x_offset;
      y_offset += table_y_offset;

      goo_canvas_item_allocate_area (child_item, cr, &requested_area,
				     &allocated_area, x_offset, y_offset);

      cairo_translate (cr, -child->position[HORZ], -child->position[VERT]);
    }
}


static void
goo_canvas_table_update_requested_heights (GooCanvasItem       *item,
					   cairo_t	       *cr)
{
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *rows = layout_data->dldata[VERT];
  GooCanvasTableDimensionLayoutData *columns = layout_data->dldata[HORZ];
  GooCanvasTableChild *child;
  GooCanvasItem *child_item;
  GooCanvasTableChildLayoutData *child_data;
  gint start_column, end_column, i, row, end;
  gdouble x, max_width, width, requested_width, requested_height, height = 0.0;

  /* Just return if we've already calculated requested heights for this exact
     width. */
  if (layout_data->last_width == layout_data->allocated_size[HORZ])
    return;
  layout_data->last_width = layout_data->allocated_size[HORZ];

  /* First calculate the allocations for each column. */
  goo_canvas_table_size_allocate_init (table, HORZ);
  goo_canvas_table_size_allocate_pass1 (table, HORZ);
  goo_canvas_table_size_allocate_pass2 (table, HORZ);

  /* Now check if any child changes height based on their allocated width. */
  for (i = 0; i < table_data->children->len; i++)
    {
      child = &g_array_index (table_data->children, GooCanvasTableChild, i);
      child_item = group->items->pdata[i];
      child_data = &layout_data->children[i];

      requested_width = child_data->requested_size[HORZ];

      /* Skip children that won't be allocated. */
      if (requested_width <= 0.0)
	continue;

      start_column = child->start[HORZ];
      end_column = child->start[HORZ] + child->size[HORZ] - 1;
      x = columns[start_column].start + layout_data->children[i].start_pad[HORZ];
      max_width = columns[end_column].end - layout_data->children[i].end_pad[HORZ] - x;

      width = max_width = MAX (0.0, max_width);

      if (!(child->flags[HORZ] & GOO_CANVAS_TABLE_CHILD_FILL))
	width = MIN (max_width, requested_width);

      requested_height = goo_canvas_item_get_requested_height (child_item, cr,
							       width);
      if (requested_height >= 0.0)
	child_data->requested_size[VERT] = requested_height;
    }

  /* Now recalculate the requested heights of each row. */
  goo_canvas_table_size_request_pass1 (table, VERT);
  goo_canvas_table_size_request_pass2 (table, VERT);
  goo_canvas_table_size_request_pass3 (table, VERT);
  goo_canvas_table_size_request_pass2 (table, VERT);

  end = table_data->dimensions[VERT].size - 1;
  for (row = 0; row <= end; row++)
    {
      height += rows[row].requisition;
      if (row < end)
	height += rows[row].spacing;
    }
  height += (layout_data->border_width + layout_data->border_spacing[VERT] + layout_data->grid_line_width[HORZ]) * 2.0;

  layout_data->natural_size[VERT] = height;
}


/* Returns FALSE if item shouldn't be allocated. */
static gboolean
goo_canvas_table_get_requested_area (GooCanvasItem        *item,
				     cairo_t              *cr,
				     GooCanvasBounds      *requested_area)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *rows, *columns;
  gdouble width = 0.0, height = 0.0;
  gint row, column, end;

  /* Request a redraw of the existing bounds */
  goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple_data->is_static);
  
  /* We reset the bounds to 0, just in case we are hidden or aren't allocated
     any area. */
  simple->bounds.x1 = simple->bounds.x2 = 0.0;
  simple->bounds.y1 = simple->bounds.y2 = 0.0;

  simple->need_update = FALSE;

  goo_canvas_item_simple_check_style (simple);

  if (simple_data->visibility == GOO_CANVAS_ITEM_HIDDEN)
    return FALSE;

  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);

  cairo_translate (cr, layout_data->x, layout_data->y);

  /* Initialize the layout data, get the requested sizes of all children, and
     set the expand, shrink and empty flags. */
  goo_canvas_table_init_layout_data (table);
  goo_canvas_table_size_request_init (table, cr);

  /* Calculate the requested width of the table. */
  goo_canvas_table_size_request_pass1 (table, HORZ);
  goo_canvas_table_size_request_pass2 (table, HORZ);
  goo_canvas_table_size_request_pass3 (table, HORZ);
  goo_canvas_table_size_request_pass2 (table, HORZ);

  rows = layout_data->dldata[VERT];
  columns = layout_data->dldata[HORZ];

  end = table_data->dimensions[HORZ].size - 1;
  for (column = 0; column <= end; column++)
    {
      width += columns[column].requisition;
      if (column < end)
	width += columns[column].spacing;
    }
  width += (layout_data->border_width + layout_data->border_spacing[HORZ] + layout_data->grid_line_width[VERT]) * 2.0;
  
  /* Save the natural size, so we know if we have to clip children. */
  layout_data->natural_size[HORZ] = width;

  /* If the width has been set, that overrides the calculations. */
  if (table_data->width > 0.0)
    width = table_data->width;

  layout_data->requested_size[HORZ] = width;
  layout_data->allocated_size[HORZ] = width;

  /* Update the requested heights, based on the requested column widths. */
  goo_canvas_table_update_requested_heights (item, cr);

  goo_canvas_table_size_request_pass1 (table, VERT);
  goo_canvas_table_size_request_pass2 (table, VERT);
  goo_canvas_table_size_request_pass3 (table, VERT);
  goo_canvas_table_size_request_pass2 (table, VERT);

  end = table_data->dimensions[VERT].size - 1;
  for (row = 0; row <= end; row++)
    {
      height += rows[row].requisition;
      if (row < end)
	height += rows[row].spacing;
    }
  height += (layout_data->border_width + layout_data->border_spacing[VERT] + layout_data->grid_line_width[HORZ]) * 2.0;

  /* Save the natural size, so we know if we have to clip children. */
  layout_data->natural_size[VERT] = height;

  /* If the height has been set, that overrides the calculations. */
  if (table_data->height > 0.0)
    height = table_data->height;

  layout_data->requested_size[VERT] = height;

  /* Copy the user bounds to the requested area. */
  requested_area->x1 = requested_area->y1 = 0.0;
  requested_area->x2 = width;
  requested_area->y2 = height;

  /* Convert to the parent's coordinate space. */
  goo_canvas_item_simple_user_bounds_to_parent (simple, cr, requested_area);

  cairo_restore (cr);

  return TRUE;
}


static gdouble
goo_canvas_table_get_requested_height (GooCanvasItem    *item,
				       cairo_t		*cr,
				       gdouble           width)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  gdouble allocated_width = width, height;

  /* If we have a transformation besides a simple scale & translation, just
     return -1 as we can't adjust the height in that case. */
  if (simple_data->transform && (simple_data->transform->xy != 0.0
				 || simple_data->transform->yx != 0.0))
    return -1;

  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);

  cairo_translate (cr, layout_data->x, layout_data->y);

  /* Convert the width from the parent's coordinate space. Note that we only
     need to support a simple scale operation here. */
  if (simple_data->transform)
    allocated_width /= simple_data->transform->xx;
  layout_data->allocated_size[HORZ] = allocated_width;

  if (layout_data->integer_layout)
    layout_data->allocated_size[HORZ] = floor (layout_data->allocated_size[HORZ]);

  goo_canvas_table_update_requested_heights (item, cr);

  cairo_restore (cr);

  /* Convert to the parent's coordinate space. As above,  we only need to
     support a simple scale operation here. */
  height = layout_data->natural_size[VERT];
  if (simple_data->transform)
    height *= simple_data->transform->yy;

  /* Return the new requested height of the table. */
  return height;
}


static void
goo_canvas_table_allocate_area (GooCanvasItem         *item,
				cairo_t               *cr,
				const GooCanvasBounds *requested_area,
				const GooCanvasBounds *allocated_area,
				gdouble                x_offset,
				gdouble                y_offset)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  gdouble requested_width, requested_height, allocated_width, allocated_height;
  gdouble width_proportion, height_proportion, min_proportion;

  requested_width = requested_area->x2 - requested_area->x1;
  requested_height = requested_area->y2 - requested_area->y1;
  allocated_width = allocated_area->x2 - allocated_area->x1;
  allocated_height = allocated_area->y2 - allocated_area->y1;

  /* Calculate what proportion of our requested size we were allocated. */
  width_proportion = allocated_width / requested_width;
  height_proportion = allocated_height / requested_height;

  /* If the table is rotated we have to scale it to make sure it fits in the
     allocated area. */
  if (simple_data->transform && (simple_data->transform->xy != 0.0
				 || simple_data->transform->yx != 0.0))
    {
      /* Calculate the minimum proportion, which we'll use to scale our
	 original width & height requests. */
      min_proportion = MIN (width_proportion, height_proportion);

      layout_data->allocated_size[HORZ] = layout_data->requested_size[HORZ] * min_proportion;
      layout_data->allocated_size[VERT] = layout_data->requested_size[VERT] * min_proportion;
    }
  else
    {
      /* If the table isn't rotated we can just scale according to the
	 allocated proportions. */
      layout_data->allocated_size[HORZ] = layout_data->requested_size[HORZ] * width_proportion;
      layout_data->allocated_size[VERT] = layout_data->requested_size[VERT] * height_proportion;
    }

  if (layout_data->integer_layout)
    {
      layout_data->allocated_size[HORZ] = floor (layout_data->requested_size[HORZ]);
      layout_data->allocated_size[VERT] = floor (layout_data->requested_size[VERT]);
    }

  /* We have to remove the translation that our parent is giving us while we
     update the size requests, otherwise child items' bounds will include the
     translation which we result in incorrect bounds after allocation. */
  cairo_save (cr);
  cairo_translate (cr, -(allocated_area->x1 - requested_area->x1),
		   -(allocated_area->y1 - requested_area->y1));
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);
  cairo_translate (cr, layout_data->x, layout_data->y);
  goo_canvas_table_update_requested_heights (item, cr);
  cairo_restore (cr);

  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);
  cairo_translate (cr, layout_data->x, layout_data->y);

  /* Calculate the table's bounds. */
  simple->bounds.x1 = simple->bounds.y1 = 0.0;
  simple->bounds.x2 = layout_data->allocated_size[HORZ];
  simple->bounds.y2 = layout_data->allocated_size[VERT];
  goo_canvas_item_simple_user_bounds_to_device (simple, cr, &simple->bounds);

  goo_canvas_table_size_allocate_init (table, VERT);
  goo_canvas_table_size_allocate_pass1 (table, VERT);
  goo_canvas_table_size_allocate_pass2 (table, VERT);

  goo_canvas_table_size_allocate_pass3 (table, cr, x_offset, y_offset);

  /* We free the children array but keep the dimension layout data, since
     we may need that for clipping children. */
  g_free (layout_data->children);
  layout_data->children = NULL;

  cairo_restore (cr);

  goo_canvas_request_item_redraw (simple->canvas, &simple->bounds, simple_data->is_static);
}


static void
goo_canvas_table_update  (GooCanvasItem   *item,
			  gboolean         entire_tree,
			  cairo_t         *cr,
			  GooCanvasBounds *bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasBounds tmp_bounds;

  if (entire_tree || simple->need_update)
    {
      simple->need_update = FALSE;
      simple->need_entire_subtree_update = FALSE;

      goo_canvas_item_simple_check_style (simple);

      /* We just allocate exactly what is requested. */
      if (goo_canvas_table_get_requested_area (item, cr, &tmp_bounds))
	{
	  goo_canvas_table_allocate_area (item, cr, &tmp_bounds, &tmp_bounds,
					  0, 0);
	}
    }

  *bounds = simple->bounds;
}


static void
goo_canvas_table_paint (GooCanvasItem         *item,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                scale)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasStyle *style = simple_data->style;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *rows = layout_data->dldata[VERT];
  GooCanvasTableDimensionLayoutData *columns = layout_data->dldata[HORZ];
  gdouble vert_grid_line_width = layout_data->grid_line_width[VERT];
  gdouble horz_grid_line_width = layout_data->grid_line_width[HORZ];
  GArray *children = table_data->children;
  GooCanvasTableChild *table_child;
  GooCanvasItem *child;
  gboolean check_clip = FALSE, clip;
  gint start_column, end_column, start_row, end_row, i, j;
  gdouble x, y, end_x, end_y;
  gdouble frame_width, frame_height;
  gdouble line_start, line_end;
  gdouble spacing, half_spacing_before, half_spacing_after;
  gboolean old_grid_line_visibility, cur_grid_line_visibility;

  /* Skip the item if the bounds don't intersect the expose rectangle. */
  if (simple->bounds.x1 > bounds->x2 || simple->bounds.x2 < bounds->x1
      || simple->bounds.y1 > bounds->y2 || simple->bounds.y2 < bounds->y1)
    return;

  /* Check if the item should be visible. */
  if (simple_data->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple_data->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple_data->visibility_threshold))
    return;

  /* Paint all the items in the group. */
  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);
  cairo_translate (cr, layout_data->x, layout_data->y);

  /* Clip with the table's clip path, if it is set. */
  if (simple_data->clip_path_commands)
    {
      goo_canvas_create_path (simple_data->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple_data->clip_fill_rule);
      cairo_clip (cr);
    }

  /* Check if the table was allocated less space than it requested, in which
     case we may need to clip children. */
  if (layout_data->allocated_size[HORZ] < layout_data->natural_size[HORZ]
      || layout_data->allocated_size[VERT] < layout_data->natural_size[VERT])
    check_clip = TRUE;

  /* frame_width/frame_height is the size of the table we draw the grid lines
     around. This normally is the allocated size, except when we are shrunk
     in which case we use the natural size. The grid will be clipped in that
     case. */
  frame_width = MAX (layout_data->allocated_size[HORZ], layout_data->natural_size[HORZ]);
  frame_height = MAX (layout_data->allocated_size[VERT], layout_data->natural_size[VERT]);

  /* Draw border and grid lines */
  if (check_clip)
    {
      cairo_rectangle (cr,
                       layout_data->border_width,
                       layout_data->border_width,
                       layout_data->allocated_size[HORZ] - 2*layout_data->border_width,
                       layout_data->allocated_size[VERT] - 2*layout_data->border_width);
      cairo_clip (cr);
    }

  /* Save current line width, line cap etc. for drawing items after having
     drawn grid lines */
  cairo_save (cr);

  /* Fill the table, if desired. */
  if (goo_canvas_style_set_fill_options (style, cr))
    {
      cairo_rectangle (cr,
                       layout_data->border_width + vert_grid_line_width,
                       layout_data->border_width + horz_grid_line_width,
                       layout_data->allocated_size[HORZ] - 2 * (layout_data->border_width + vert_grid_line_width),
                       layout_data->allocated_size[VERT] - 2 * (layout_data->border_width + horz_grid_line_width));
      cairo_fill (cr);
    }

  /* We use the style for the stroke color, but the line cap style and line
     width are overridden here. */
  goo_canvas_style_set_stroke_options (style, cr);

  cairo_set_line_cap (cr, CAIRO_LINE_CAP_BUTT);

  /* Horizontal grid lines */
  if (horz_grid_line_width > 0.0)
    {
      cairo_set_line_width (cr, horz_grid_line_width);

      /* Outer lines */
      line_start = layout_data->border_width;
      line_end = frame_width - layout_data->border_width;

      cairo_move_to (cr, line_start, layout_data->border_width + horz_grid_line_width/2);
      cairo_rel_line_to (cr, line_end - line_start, 0);

      cairo_move_to (cr, line_start, frame_height - layout_data->border_width - horz_grid_line_width/2);
      cairo_rel_line_to (cr, line_end - line_start, 0);

      /* Inner lines. Make sure we don't do overlapping drawing operations,
         so we could easily draw alpha transparent borders */
      for (i = 0; i + 1 < table_data->dimensions[VERT].size; i++)
        {
          for (j = 0; j < table_data->dimensions[HORZ].size; j++)
            {
              cur_grid_line_visibility = GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[VERT], i, j);
              if (cur_grid_line_visibility)
                {
                  spacing = (columns[j].spacing - vert_grid_line_width);
		  half_spacing_before = spacing / 2.0;
                  if (simple->canvas->integer_layout)
		    half_spacing_before = floor (half_spacing_before);
		  half_spacing_after = spacing - half_spacing_before;

                  if (j == 0)
                    line_start = layout_data->border_width + vert_grid_line_width;
                  else
                    if (old_grid_line_visibility)
                      line_start = columns[j].start - half_spacing_after;
                    else
                      line_start = columns[j-1].end + half_spacing_before;

                  half_spacing_after = (columns[j].spacing - vert_grid_line_width)/2.0;
                  if (simple->canvas->integer_layout)
                    half_spacing_after = ceil (half_spacing_after);

                  if (j == table_data->dimensions[HORZ].size - 1)
                    line_end = frame_width - layout_data->border_width - vert_grid_line_width;
                  else
                    line_end = columns[j + 1].start - half_spacing_after;

                  cairo_move_to (cr, line_start, rows[i].end + rows[i].spacing/2.0);
                  cairo_rel_line_to (cr, line_end - line_start, 0);
                }

              old_grid_line_visibility = cur_grid_line_visibility;
            }
        }
      
      cairo_stroke (cr);
    }

  /* Vertical grid lines */
  if (vert_grid_line_width > 0.0)
    {
      cairo_set_line_width (cr, vert_grid_line_width);

      /* Outer lines */
      line_start = layout_data->border_width + horz_grid_line_width;
      line_end = frame_height - layout_data->border_width - horz_grid_line_width;

      cairo_move_to (cr, layout_data->border_width + vert_grid_line_width/2, line_start);
      cairo_rel_line_to (cr, 0, line_end - line_start);

      cairo_move_to (cr, frame_width - layout_data->border_width - vert_grid_line_width/2, line_start);
      cairo_rel_line_to (cr, 0, line_end - line_start);

      /* Inner lines. Make sure we don't do overlapping drawing operations,
         so we could easily draw alpha transparent borders. We need to
         take additionally care that we don't cross already drawn
         horizontal lines. */
      for (i = 0; i + 1 < table_data->dimensions[HORZ].size; i++)
        {
          for (j = 0; j < table_data->dimensions[VERT].size; j++)
            {
              cur_grid_line_visibility = GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[HORZ], i, j);
              if (cur_grid_line_visibility)
                {
                  spacing = (rows[j].spacing - horz_grid_line_width);
		  half_spacing_before = spacing / 2.0;
                  if (simple->canvas->integer_layout)
		    half_spacing_before = floor (half_spacing_before);
		  half_spacing_after = spacing - half_spacing_before;

                  if (j == 0)
                    line_start = layout_data->border_width + horz_grid_line_width;
                  else
                    if (old_grid_line_visibility)
                      line_start = rows[j].start - half_spacing_after;
                    else
                      /* Don't draw top part if already drawn by horizontal grid line */
                      if (GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[VERT], j-1, i)
                          || GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[VERT], j-1, i+1))
                        line_start = rows[j].start - half_spacing_after;
                      else
                        line_start = rows[j-1].end + half_spacing_before;

                  half_spacing_before = half_spacing_after = (rows[j].spacing - horz_grid_line_width)/2.0;
                  if (simple->canvas->integer_layout)
                    {
                      half_spacing_before = floor (half_spacing_before);
                      half_spacing_after = ceil (half_spacing_after);
                    }

                  if (j == table_data->dimensions[VERT].size - 1)
                    line_end = frame_height - layout_data->border_width - horz_grid_line_width;
                  else
                    /* Don't draw bottom part if already drawn by horizontal grid line */
                    if (GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[VERT], j, i)
                        || GOO_CANVAS_TABLE_IS_GRID_LINE_VISIBLE(layout_data->dldata[VERT], j, i+1))
                      line_end = rows[j].end + half_spacing_before;
                    else
                      line_end = rows[j + 1].start - half_spacing_after;

                  cairo_move_to (cr, columns[i].end + columns[i].spacing/2.0, line_start);
                  cairo_rel_line_to (cr, 0, line_end - line_start);
                }

              old_grid_line_visibility = cur_grid_line_visibility;
            }
        }

      cairo_stroke (cr);
    }

  cairo_restore (cr);

  for (i = 0; i < group->items->len; i++)
    {
      child = group->items->pdata[i];

      table_child = &g_array_index (children, GooCanvasTableChild, i);
      clip = FALSE;

      /* Clip the child to make sure it doesn't go outside its area. */
      if (check_clip)
	{
	  /* Calculate the position of the child and how much space it has. */
	  start_column = table_child->start[HORZ];
	  end_column = table_child->start[HORZ] + table_child->size[HORZ] - 1;
	  x = columns[start_column].start + table_child->start_pad[HORZ];
	  end_x = columns[end_column].end - table_child->end_pad[HORZ];

	  start_row = table_child->start[VERT];
	  end_row = table_child->start[VERT] + table_child->size[VERT] - 1;
	  y = rows[start_row].start + table_child->start_pad[VERT];
	  end_y = rows[end_row].end - table_child->end_pad[VERT];

	  if (simple->canvas->integer_layout)
	    {
	      x = floor (x + 0.5);
	      end_x = floor (end_x + 0.5);
	      y = floor (y + 0.5);
	      end_y = floor (end_y + 0.5);
	    }

	  /* Make sure it doesn't go outside the table's allocated size. */
	  end_x = MIN (end_x, layout_data->allocated_size[HORZ]);
	  end_y = MIN (end_y, layout_data->allocated_size[VERT]);

	  /* Only try to paint the child if it has some allocated space. */
	  if (end_x <= x || end_y <= y)
	    continue;

	  /* Check if any of the rows/columns the child is in can shrink. */
	  for (j = start_column; j <= end_column; j++)
	    if (columns[j].shrink)
	      clip = TRUE;

	  for (j = start_row; j <= end_row; j++)
	    if (rows[j].shrink)
	      clip = TRUE;
			 
	  /* Only clip the child if it may have been shrunk. */
	  if (clip)
	    {
	      cairo_save (cr);
	      cairo_rectangle (cr, x, y, end_x - x, end_y - y);
	      cairo_clip (cr);
	    }
	}

      cairo_translate (cr, table_child->position[HORZ],
		       table_child->position[VERT]);
      goo_canvas_item_paint (child, cr, bounds, scale);
      cairo_translate (cr, -table_child->position[HORZ],
		       -table_child->position[VERT]);

      if (clip)
	cairo_restore (cr);
    }
  cairo_restore (cr);
}


static GList*
goo_canvas_table_get_items_at (GooCanvasItem  *item,
			       gdouble         x,
			       gdouble         y,
			       cairo_t        *cr,
			       gboolean        is_pointer_event,
			       gboolean        parent_visible,
			       GList          *found_items)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasItemSimpleData *simple_data = simple->simple_data;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableData *table_data = table->table_data;
  GooCanvasTableLayoutData *layout_data = table_data->layout_data;
  GooCanvasTableDimensionLayoutData *rows = layout_data->dldata[VERT];
  GooCanvasTableDimensionLayoutData *columns = layout_data->dldata[HORZ];
  GArray *children = table_data->children;
  GooCanvasTableChild *table_child;
  GooCanvasItem *child;
  gboolean visible = parent_visible, check_clip = FALSE;
  double user_x = x, user_y = y;
  gint start_column, end_column, start_row, end_row, i;
  gdouble start_x, end_x, start_y, end_y;

  if (simple->need_update)
    goo_canvas_item_ensure_updated (item);

  /* Skip the item if the point isn't in the item's bounds. */
  if (simple->bounds.x1 > x || simple->bounds.x2 < x
      || simple->bounds.y1 > y || simple->bounds.y2 < y)
    return found_items;

  if (simple_data->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple_data->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple_data->visibility_threshold))
    visible = FALSE;

  /* Check if the group should receive events. */
  if (is_pointer_event
      && (simple_data->pointer_events == GOO_CANVAS_EVENTS_NONE
	  || ((simple_data->pointer_events & GOO_CANVAS_EVENTS_VISIBLE_MASK)
	      && !visible)))
    return found_items;

  cairo_save (cr);
  if (simple_data->transform)
    cairo_transform (cr, simple_data->transform);
  cairo_translate (cr, layout_data->x, layout_data->y);

  cairo_device_to_user (cr, &user_x, &user_y);

  /* If the table has a clip path, check if the point is inside it. */
  if (simple_data->clip_path_commands)
    {
      goo_canvas_create_path (simple_data->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple_data->clip_fill_rule);
      if (!cairo_in_fill (cr, user_x, user_y))
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  /* Check if the table was allocated less space than it requested, in which
     case we may need to clip children. */
  if (layout_data->allocated_size[HORZ] < layout_data->natural_size[HORZ]
      || layout_data->allocated_size[VERT] < layout_data->natural_size[VERT])
    check_clip = TRUE;

  /* Step up from the bottom of the children to the top, adding any items
     found to the start of the list. */
  for (i = 0; i < group->items->len; i++)
    {
      child = group->items->pdata[i];

      table_child = &g_array_index (children, GooCanvasTableChild, i);

      /* If the child may have been clipped, check the point is in the child's
	 allocated area. */
      if (check_clip)
	{
	  /* Calculate the position of the child and how much space it has. */
	  start_column = table_child->start[HORZ];
	  end_column = table_child->start[HORZ] + table_child->size[HORZ] - 1;
	  start_x = columns[start_column].start + table_child->start_pad[HORZ];
	  end_x = columns[end_column].end - table_child->end_pad[HORZ];

	  start_row = table_child->start[VERT];
	  end_row = table_child->start[VERT] + table_child->size[VERT] - 1;
	  start_y = rows[start_row].start + table_child->start_pad[VERT];
	  end_y = rows[end_row].end - table_child->end_pad[VERT];

	  if (simple->canvas->integer_layout)
	    {
	      start_x = floor (start_x + 0.5);
	      end_x = floor (end_x + 0.5);
	      start_y = floor (start_y + 0.5);
	      end_y = floor (end_y + 0.5);
	    }

	  if (user_x < start_x || user_x > end_x
	      || user_y < start_y || user_y > end_y)
	    continue;
	}

      cairo_translate (cr, table_child->position[HORZ],
		       table_child->position[VERT]);

      found_items = goo_canvas_item_get_items_at (child, x, y, cr,
						  is_pointer_event, visible,
						  found_items);

      cairo_translate (cr, -table_child->position[HORZ],
		       -table_child->position[VERT]);
    }
  cairo_restore (cr);

  return found_items;
}


gboolean
goo_canvas_table_get_transform_for_child  (GooCanvasItem  *item,
					   GooCanvasItem  *child,
					   cairo_matrix_t *transform)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasTable *table = (GooCanvasTable*) item;
  GooCanvasTableChild *table_child;
  gboolean has_transform = FALSE;
  gint child_num;


  if (simple->simple_data->transform)
    {
      *transform = *simple->simple_data->transform;
      has_transform = TRUE;
    }
  else
    {
      cairo_matrix_init_identity (transform);
    }

  for (child_num = 0; child_num < group->items->len; child_num++)
    {
      if (group->items->pdata[child_num] == child)
	{
	  table_child = &g_array_index (table->table_data->children,
					GooCanvasTableChild, child_num);
	  cairo_matrix_translate (transform, table_child->position[HORZ],
				  table_child->position[VERT]);
	  has_transform = TRUE;
	  break;
	}
    }

  return has_transform;
}


static void
item_interface_init (GooCanvasItemIface *iface)
{
  iface->add_child               = goo_canvas_table_add_child;
  iface->move_child              = goo_canvas_table_move_child;
  iface->remove_child            = goo_canvas_table_remove_child;
  iface->get_child_property      = goo_canvas_table_get_child_property;
  iface->set_child_property      = goo_canvas_table_set_child_property;
  iface->get_transform_for_child = goo_canvas_table_get_transform_for_child;

  iface->update                  = goo_canvas_table_update;
  iface->get_requested_area      = goo_canvas_table_get_requested_area;
  iface->get_requested_height    = goo_canvas_table_get_requested_height;
  iface->allocate_area           = goo_canvas_table_allocate_area;
  iface->paint                   = goo_canvas_table_paint;
  iface->get_items_at	         = goo_canvas_table_get_items_at;

  iface->set_model               = goo_canvas_table_set_model;
}



/**
 * SECTION:goocanvastablemodel
 * @Title: GooCanvasTableModel
 * @Short_Description: a model for a table container to layout items.
 *
 * #GooCanvasTableModel is a model for a table container used to lay out other
 * canvas items. It is used in a similar way to how the GtkTable widget is used
 * to lay out GTK+ widgets.
 *
 * Item models are added to the table using the normal methods, then
 * goo_canvas_item_model_set_child_properties() is used to specify how each
 * child item is to be positioned within the table (i.e. which row and column
 * it is in, how much padding it should have and whether it should expand or
 * shrink).
 *
 * #GooCanvasTableModel is a subclass of #GooCanvasItemModelSimple and so
 * inherits all of the style properties such as "stroke-color", "fill-color"
 * and "line-width". Setting a style property on a #GooCanvasTableModel will
 * affect all children of the #GooCanvasTableModel (unless the children
 * override the property setting).
 *
 * #GooCanvasTableModel implements the #GooCanvasItemModel interface, so you
 * can use the #GooCanvasItemModel functions such as
 * goo_canvas_item_model_raise() and goo_canvas_item_rotate(), and the
 * properties such as "visibility" and "pointer-events".
 *
 * To create a #GooCanvasTableModel use goo_canvas_table_model_new().
 *
 * To get or set the properties of an existing #GooCanvasTableModel, use
 * g_object_get() and g_object_set().
 */

static GooCanvasItemModelIface *goo_canvas_table_model_parent_iface;

static void item_model_interface_init (GooCanvasItemModelIface *iface);
static void goo_canvas_table_model_finalize (GObject *object);
static void goo_canvas_table_model_get_property (GObject            *object,
						 guint               param_id,
						 GValue             *value,
						 GParamSpec         *pspec);
static void goo_canvas_table_model_set_property (GObject            *object,
						 guint               param_id,
						 const GValue       *value,
						 GParamSpec         *pspec);

G_DEFINE_TYPE_WITH_CODE (GooCanvasTableModel, goo_canvas_table_model,
			 GOO_TYPE_CANVAS_GROUP_MODEL,
			 G_IMPLEMENT_INTERFACE (GOO_TYPE_CANVAS_ITEM_MODEL,
						item_model_interface_init))


static void
goo_canvas_table_model_class_init (GooCanvasTableModelClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;

  goo_canvas_table_model_parent_iface = g_type_interface_peek (goo_canvas_table_model_parent_class, GOO_TYPE_CANVAS_ITEM_MODEL);

  gobject_class->finalize = goo_canvas_table_model_finalize;

  gobject_class->get_property = goo_canvas_table_model_get_property;
  gobject_class->set_property = goo_canvas_table_model_set_property;

  goo_canvas_table_install_common_properties (gobject_class, goo_canvas_item_model_class_install_child_property);
}


static void
goo_canvas_table_model_init (GooCanvasTableModel *tmodel)
{
  goo_canvas_table_init_data (&tmodel->table_data);
}


/**
 * goo_canvas_table_model_new:
 * @parent: the parent model, or %NULL. If a parent is specified, it will
 *  assume ownership of the item, and the item will automatically be freed when
 *  it is removed from the parent. Otherwise call g_object_unref() to free it.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new table model.
 *
 * <!--PARAMETERS-->
 *
 * Here's an example showing how to create a table with a square, a circle and
 * a triangle in it:
 *
 * <informalexample><programlisting>
 *  GooCanvasItemModel *table, *square, *circle, *triangle;
 *
 *  table = goo_canvas_table_model_new (root,
 *                                      "row-spacing", 4.0,
 *                                      "column-spacing", 4.0,
 *                                      NULL);
 *  goo_canvas_item_model_translate (table, 400, 200);
 *
 *  square = goo_canvas_rect_model_new (table, 0.0, 0.0, 50.0, 50.0,
 *                                      "fill-color", "red",
 *                                      NULL);
 *  goo_canvas_item_model_set_child_properties (table, square,
 *                                              "row", 0,
 *                                              "column", 0,
 *                                              NULL);
 *
 *  circle = goo_canvas_ellipse_model_new (table, 0.0, 0.0, 25.0, 25.0,
 *                                         "fill-color", "blue",
 *                                         NULL);
 *  goo_canvas_item_model_set_child_properties (table, circle,
 *                                              "row", 0,
 *                                              "column", 1,
 *                                              NULL);
 *
 *  triangle = goo_canvas_polyline_model_new (table, TRUE, 3,
 *                                            25.0, 0.0, 0.0, 50.0, 50.0, 50.0,
 *                                            "fill-color", "yellow",
 *                                            NULL);
 *  goo_canvas_item_model_set_child_properties (table, triangle,
 *                                              "row", 0,
 *                                              "column", 2,
 *                                              NULL);
 * </programlisting></informalexample>
 * 
 * Returns: a new table model.
 **/
GooCanvasItemModel*
goo_canvas_table_model_new (GooCanvasItemModel *parent,
			    ...)
{
  GooCanvasItemModel *model;
  va_list var_args;
  const char *first_property;

  model = g_object_new (GOO_TYPE_CANVAS_TABLE_MODEL, NULL);

  va_start (var_args, parent);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (model), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_model_add_child (parent, model, -1);
      g_object_unref (model);
    }

  return model;
}


static void
goo_canvas_table_model_finalize (GObject *object)
{
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) object;

  goo_canvas_table_free_data (&tmodel->table_data);

  G_OBJECT_CLASS (goo_canvas_table_model_parent_class)->finalize (object);
}


static void
goo_canvas_table_model_get_property (GObject              *object,
				     guint                 prop_id,
				     GValue               *value,
				     GParamSpec           *pspec)
{
  GooCanvasTableModel *emodel = (GooCanvasTableModel*) object;

  goo_canvas_table_get_common_property (object, &emodel->table_data,
					prop_id, value, pspec);
}


static void
goo_canvas_table_model_set_property (GObject              *object,
				     guint                 prop_id,
				     const GValue         *value,
				     GParamSpec           *pspec)
{
  GooCanvasTableModel *emodel = (GooCanvasTableModel*) object;
  gboolean recompute_bounds;

  recompute_bounds = goo_canvas_table_set_common_property (object,
							   &emodel->table_data,
							   prop_id, value,
							   pspec);
  g_signal_emit_by_name (emodel, "changed", recompute_bounds);
}


static void
goo_canvas_table_model_add_child     (GooCanvasItemModel *model,
				      GooCanvasItemModel *child,
				      gint                position)
{
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;

  goo_canvas_table_add_child_internal (&tmodel->table_data, position);

  /* Let the parent GooCanvasGroupModel code do the rest. */
  goo_canvas_table_model_parent_iface->add_child (model, child, position);
}


static void
goo_canvas_table_model_move_child    (GooCanvasItemModel *model,
				      gint	          old_position,
				      gint                new_position)
{
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;

  goo_canvas_table_move_child_internal (&tmodel->table_data, old_position,
					new_position);

  /* Let the parent GooCanvasGroupModel code do the rest. */
  goo_canvas_table_model_parent_iface->move_child (model, old_position,
						   new_position);
}


static void
goo_canvas_table_model_remove_child  (GooCanvasItemModel *model,
				      gint                child_num)
{
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;

  g_array_remove_index (tmodel->table_data.children, child_num);

  /* Let the parent GooCanvasGroupModel code do the rest. */
  goo_canvas_table_model_parent_iface->remove_child (model, child_num);
}


static void
goo_canvas_table_model_get_child_property (GooCanvasItemModel *model,
					   GooCanvasItemModel *child,
					   guint               property_id,
					   GValue             *value,
					   GParamSpec         *pspec)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;
  GooCanvasTableChild *table_child;
  gint child_num;

  for (child_num = 0; child_num < gmodel->children->len; child_num++)
    {
      if (gmodel->children->pdata[child_num] == child)
	{
	  table_child = &g_array_index (tmodel->table_data.children,
					GooCanvasTableChild, child_num);
	  goo_canvas_table_get_common_child_property ((GObject*) tmodel,
						      table_child,
						      property_id, value,
						      pspec);
	  break;
	}
    }
}


static void
goo_canvas_table_model_set_child_property (GooCanvasItemModel *model,
					   GooCanvasItemModel *child,
					   guint               property_id,
					   const GValue       *value,
					   GParamSpec         *pspec)
{
  GooCanvasGroupModel *gmodel = (GooCanvasGroupModel*) model;
  GooCanvasTableModel *tmodel = (GooCanvasTableModel*) model;
  GooCanvasTableChild *table_child;
  gint child_num;

  for (child_num = 0; child_num < gmodel->children->len; child_num++)
    {
      if (gmodel->children->pdata[child_num] == child)
	{
	  table_child = &g_array_index (tmodel->table_data.children,
					GooCanvasTableChild, child_num);
	  goo_canvas_table_set_common_child_property ((GObject*) tmodel,
						      &tmodel->table_data,
						      table_child,
						      property_id, value,
						      pspec);
	  break;
	}
    }

  g_signal_emit_by_name (tmodel, "changed", TRUE);
}


static GooCanvasItem*
goo_canvas_table_model_create_item (GooCanvasItemModel *model,
				    GooCanvas          *canvas)
{
  GooCanvasItem *item;

  item = goo_canvas_table_new (NULL, NULL);
  /* Note that we set the canvas before the model, since we may need the
     canvas to create any child items. */
  goo_canvas_item_set_canvas (item, canvas);
  goo_canvas_item_set_model (item, model);

  return item;
}


static void
item_model_interface_init (GooCanvasItemModelIface *iface)
{
  iface->add_child          = goo_canvas_table_model_add_child;
  iface->move_child         = goo_canvas_table_model_move_child;
  iface->remove_child       = goo_canvas_table_model_remove_child;
  iface->get_child_property = goo_canvas_table_model_get_child_property;
  iface->set_child_property = goo_canvas_table_model_set_child_property;

  iface->create_item        = goo_canvas_table_model_create_item;
}
