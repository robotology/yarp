/* $Id: gtkdatabox_graph.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_graph.h>
#include <gtk/gtkstyle.h>
#include <gtk/gtkgc.h>

static void gtk_databox_graph_real_draw (GtkDataboxGraph * graph,
					 GtkDatabox * draw);
static gint gtk_databox_graph_real_calculate_extrema (GtkDataboxGraph * graph,
						      gfloat * min_x,
						      gfloat * max_x,
						      gfloat * min_y,
						      gfloat * max_y);
static GdkGC * gtk_databox_graph_real_create_gc (GtkDataboxGraph * graph,
					      GtkDatabox * box);

/* IDs of properties */
enum
{
   GRAPH_COLOR = 1,
   GRAPH_SIZE,
   GRAPH_HIDE
};

struct _GtkDataboxGraphPrivate
{
   GdkColor color;
   gint size;
   gboolean hide;
   GdkGC *gc;
};

static gpointer parent_class = NULL;

static void
gtk_databox_graph_set_property (GObject * object,
				guint property_id,
				const GValue * value, GParamSpec * pspec)
{
   GtkDataboxGraph *graph = GTK_DATABOX_GRAPH (object);

   switch (property_id)
   {
   case GRAPH_COLOR:
      {
	 gtk_databox_graph_set_color (graph,
				      (GdkColor *)
				      g_value_get_pointer (value));
      }
      break;
   case GRAPH_SIZE:
      {
	 gtk_databox_graph_set_size (graph, g_value_get_int (value));
      }
      break;
   case GRAPH_HIDE:
      {
	 gtk_databox_graph_set_hide (graph, g_value_get_boolean (value));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
gtk_databox_graph_get_property (GObject * object,
				guint property_id,
				GValue * value, GParamSpec * pspec)
{
   GtkDataboxGraph *graph = GTK_DATABOX_GRAPH (object);

   switch (property_id)
   {
   case GRAPH_COLOR:
      {
	 g_value_set_pointer (value, gtk_databox_graph_get_color (graph));
      }
      break;
   case GRAPH_SIZE:
      {
	 g_value_set_int (value, gtk_databox_graph_get_size (graph));
      }
      break;
   case GRAPH_HIDE:
      {
	 g_value_set_boolean (value, gtk_databox_graph_get_hide (graph));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
gtk_databox_graph_delete_gc (GtkDataboxGraph * graph)
{
   if (graph->priv->gc)
   {
      GdkColormap *colormap = NULL;

      colormap = gdk_gc_get_colormap (graph->priv->gc);
      gdk_colormap_free_colors (colormap, &graph->priv->color, 1);
      gtk_gc_release (graph->priv->gc);
      graph->priv->gc = NULL;
   }
}

/** 
 * gtk_databox_graph_create_gc:
 * @graph: A #GtkDataboxGraph object
 * @box: A #GtkDatabox object
 *
 * Virtual function which creates a graphics context for the @graph. 
 *
 * Typically called by derived graph objects when the graphics context is needed for the first time.
 *
 * Return value: The new graphics context.
 */
GdkGC*
gtk_databox_graph_create_gc (GtkDataboxGraph * graph,
			     GtkDatabox* box)
{
   return GTK_DATABOX_GRAPH_GET_CLASS (graph)->create_gc (graph, box);
}

static GdkGC*
gtk_databox_graph_real_create_gc (GtkDataboxGraph * graph,
				  GtkDatabox* box)
{
   GtkWidget *widget = GTK_WIDGET(box);
   GdkGCValues values;
   GdkGCValuesMask valuesMask;
   GdkColormap *colormap = NULL;
   GtkStyle *style;

   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), NULL);

   if (graph->priv->gc)
      gtk_databox_graph_delete_gc (graph);

   style = widget->style;

   colormap = style->colormap;
   g_return_val_if_fail (colormap, NULL);
   g_return_val_if_fail (gdk_colormap_alloc_color (colormap,
					       &graph->priv->color,
					       FALSE, TRUE), NULL);

   valuesMask = GDK_GC_FOREGROUND | GDK_GC_BACKGROUND
      | GDK_GC_FUNCTION
      | GDK_GC_LINE_WIDTH | GDK_GC_LINE_STYLE
      | GDK_GC_CAP_STYLE | GDK_GC_JOIN_STYLE;

   values.foreground = graph->priv->color;
   values.background = style->black;
   values.function = GDK_COPY;
   /* I am not sure, why line_width==1 is so much slower than 0, but 
    * it is (at least for my machine with gtk+-2.4) */
   values.line_width = (graph->priv->size > 1) ? graph->priv->size : 0;
   values.line_style = GDK_LINE_SOLID;
   values.cap_style = GDK_CAP_BUTT;
   values.join_style = GDK_JOIN_MITER;

   graph->priv->gc = gtk_gc_get (style->depth,
			   style->colormap, &values, valuesMask);

   return graph->priv->gc;
}

static void
graph_finalize (GObject * object)
{
   GtkDataboxGraph *graph = GTK_DATABOX_GRAPH (object);

   gtk_databox_graph_delete_gc (graph);
   g_free (graph->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_databox_graph_class_init (gpointer g_class /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *klass = GTK_DATABOX_GRAPH_CLASS (g_class);
   GParamSpec *graph_param_spec;

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->set_property = gtk_databox_graph_set_property;
   gobject_class->get_property = gtk_databox_graph_get_property;
   gobject_class->finalize = graph_finalize;

   graph_param_spec = g_param_spec_pointer ("color",
					    "Graph color",
					    "Color of graph",
					    G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    GRAPH_COLOR, graph_param_spec);

   graph_param_spec = g_param_spec_int ("size", "Graph size", "Size of displayed items", G_MININT, G_MAXINT, 0,	/* default value */
					G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    GRAPH_SIZE, graph_param_spec);

   graph_param_spec = g_param_spec_boolean ("hide", "Graph hidden", "Determine if graph is hidden or not", FALSE,	/* default value */
					    G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    GRAPH_SIZE, graph_param_spec);

   klass->draw = gtk_databox_graph_real_draw;
   klass->calculate_extrema = gtk_databox_graph_real_calculate_extrema;
   klass->create_gc = gtk_databox_graph_real_create_gc;
}

static void
gtk_databox_graph_instance_init (GTypeInstance * instance
				 /*, gpointer g_class */ )
{
   GtkDataboxGraph *graph = GTK_DATABOX_GRAPH (instance);

   graph->priv = g_new0 (GtkDataboxGraphPrivate, 1);
}

GType
gtk_databox_graph_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxGraphClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_graph_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxGraph),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_graph_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (G_TYPE_OBJECT,
				     "GtkDataboxGraph", &info, 0);
   }

   return type;
}

/** 
 * gtk_databox_graph_draw:
 * @graph: A #GtkDataboxGraph object
 * @box: A #GtkDatabox object
 *
 * Virtual function which draws the #GtkDataboxGraph on the drawing area of the GtkDatabox object.
 *
 * Typically this function is called by #GtkDatabox objects.
 *
 */
void
gtk_databox_graph_draw (GtkDataboxGraph * graph, GtkDatabox* box)
{
   if (!graph->priv->hide)
      GTK_DATABOX_GRAPH_GET_CLASS (graph)->draw (graph, box);
}

/** 
 * gtk_databox_graph_calculate_extrema:
 * @graph: A #GtkDataboxGraph object
 * @min_x: Will be filled with the lowest x value of the dataset
 * @max_x: Will be filled with the highest x value of the dataset
 * @min_y: Will be filled with the lowest y value of the dataset
 * @max_y: Will be filled with the highest y value of the dataset
 *
 * Virtual function which determines the minimum and maximum x and y values of the values of this 
 * #GtkDataboxGraph object if applicable (there are graphs which do 
 * not contain data). 
 *
 * Return value: 0 on success,
 *          -1 if no data is available,
 *
 */
gint
gtk_databox_graph_calculate_extrema (GtkDataboxGraph * graph,
				     gfloat * min_x, gfloat * max_x,
				     gfloat * min_y, gfloat * max_y)
{
   return
      GTK_DATABOX_GRAPH_GET_CLASS (graph)->calculate_extrema (graph, min_x,
							      max_x, min_y,
							      max_y);
}

static void
gtk_databox_graph_real_draw (GtkDataboxGraph * graph,
			     GtkDatabox* box)
{
   g_return_if_fail (graph);
   g_return_if_fail (box);

   /* We have no data... */
   return;
}


static gint
gtk_databox_graph_real_calculate_extrema (GtkDataboxGraph * graph,
					  gfloat * min_x, gfloat * max_x,
					  gfloat * min_y, gfloat * max_y)
{
   g_return_val_if_fail (graph, -1);
   g_return_val_if_fail (min_x, -1);
   g_return_val_if_fail (max_x, -1);
   g_return_val_if_fail (min_y, -1);
   g_return_val_if_fail (max_y, -1);

   /* We have no data... */
   return -1;
}

/** 
 * gtk_databox_graph_set_color:
 * @graph: A #GtkDataboxGraph object
 * @color: Color which is to be used by the graph object
 *
 * Sets the color which the #GtkDataboxGraph object is supposed to be using when drawing itself.
 * 
 */
void
gtk_databox_graph_set_color (GtkDataboxGraph * graph, GdkColor * color)
{
   GdkColormap *colormap = NULL;

   g_return_if_fail (GTK_DATABOX_IS_GRAPH (graph));

   if (graph->priv->gc)
   {
      colormap = gdk_gc_get_colormap (graph->priv->gc);
      gdk_colormap_free_colors (colormap, &graph->priv->color, 1);
      gdk_colormap_alloc_color (colormap, color, FALSE, TRUE);
      gdk_gc_set_foreground (graph->priv->gc, color);
   }

   graph->priv->color = *color;

   g_object_notify (G_OBJECT (graph), "color");
}

/** 
 * gtk_databox_graph_get_color:
 * @graph: A #GtkDataboxGraph object
 *
 * Gets the current color of the graph elements (e.g. points).
 *
 * Return value: The color of the graph.
 * 
 */
GdkColor *
gtk_databox_graph_get_color (GtkDataboxGraph * graph)
{
   return &graph->priv->color;
}

/** 
 * gtk_databox_graph_set_size:
 * @graph: A #GtkDataboxGraph object
 * @size: Size of graph elements for the graph object
 *
 * Sets the size (e.g. line width) which the #GtkDataboxGraph object is supposed to be using when drawing itself.
 * 
 */
void
gtk_databox_graph_set_size (GtkDataboxGraph * graph, gint size)
{
   GdkGCValues values;

   g_return_if_fail (GTK_DATABOX_IS_GRAPH (graph));

   graph->priv->size = MAX (1, size);;

   if (graph->priv->gc)
   {
      values.line_width = graph->priv->size;
      gdk_gc_set_values (graph->priv->gc, &values, GDK_GC_LINE_WIDTH);
   }

   g_object_notify (G_OBJECT (graph), "size");
}

/** 
 * gtk_databox_graph_get_size:
 * @graph: A #GtkDataboxGraph object
 *
 * Gets the size of the graph elements (e.g. the line width).
 *
 * Return value: size of the graph elements
 * 
 */
gint
gtk_databox_graph_get_size (GtkDataboxGraph * graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), -1);

   return graph->priv->size;
}

/** 
 * gtk_databox_graph_set_gc:
 * @graph: A #GtkDataboxGraph object
 * @gc: Graphics contex for the graph object
 *
 * Sets the graphics context which the #GtkDataboxGraph object is supposed to be using when drawing itself.
 * 
 */
void 
gtk_databox_graph_set_gc (GtkDataboxGraph * graph, GdkGC *gc)
{
   g_return_if_fail (GTK_DATABOX_IS_GRAPH (graph));
   g_return_if_fail (GDK_IS_GC (gc));

   graph->priv->gc = gc;
}

/** 
 * gtk_databox_graph_get_gc:
 * @graph: A #GtkDataboxGraph object
 *
 * Gets the  current graphics context of the graph.
 *
 * Return value: The current graphics context of the graph.
 * 
 */
GdkGC* 
gtk_databox_graph_get_gc (GtkDataboxGraph * graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), NULL);

   return graph->priv->gc;
}

/** 
 * gtk_databox_graph_set_hide:
 * @graph: A #GtkDataboxGraph object
 * @hide: Declares whether should be hidden (true) or not (false).
 *
 * Hidden graphs are not shown, when the #GtkDatabox containing them is redrawn.
 * 
 */
void
gtk_databox_graph_set_hide (GtkDataboxGraph * graph, gboolean hide)
{
   g_return_if_fail (GTK_DATABOX_IS_GRAPH (graph));

   graph->priv->hide = hide;

   g_object_notify (G_OBJECT (graph), "hide");
}

/** 
 * gtk_databox_graph_get_hide:
 * @graph: A #GtkDataboxGraph object
 *
 * Gets the current "hide" status.
 *
 * Return value: Whether the graph is hidden (true) or not (false).
 * 
 */
gboolean
gtk_databox_graph_get_hide (GtkDataboxGraph * graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_GRAPH (graph), -1);

   return graph->priv->hide;
}
