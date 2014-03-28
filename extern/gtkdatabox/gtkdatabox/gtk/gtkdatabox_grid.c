/* $Id: gtkdatabox_grid.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_grid.h>

static void gtk_databox_grid_real_draw (GtkDataboxGraph * grid,
					GtkDatabox* box);
static GdkGC* gtk_databox_grid_real_create_gc (GtkDataboxGraph * graph,
					     GtkDatabox* box);
static   gfloat *hline_vals=NULL;
static   gfloat *vline_vals=NULL;

/* IDs of properties */
enum
{
   GRID_HLINES = 1,
   GRID_VLINES
};

struct _GtkDataboxGridPrivate
{
   gint hlines;
   gint vlines;
};

static gpointer parent_class = NULL;

static void
gtk_databox_grid_set_property (GObject * object,
			       guint property_id,
			       const GValue * value, GParamSpec * pspec)
{
   GtkDataboxGrid *grid = GTK_DATABOX_GRID (object);

   switch (property_id)
   {
   case GRID_HLINES:
      {
	 gtk_databox_grid_set_hlines (grid, g_value_get_int (value));
      }
      break;
   case GRID_VLINES:
      {
	 gtk_databox_grid_set_vlines (grid, g_value_get_int (value));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
gtk_databox_grid_get_property (GObject * object,
			       guint property_id,
			       GValue * value, GParamSpec * pspec)
{
   GtkDataboxGrid *grid = GTK_DATABOX_GRID (object);

   switch (property_id)
   {
   case GRID_HLINES:
      {
	 g_value_set_int (value, gtk_databox_grid_get_hlines (grid));
      }
      break;
   case GRID_VLINES:
      {
	 g_value_set_int (value, gtk_databox_grid_get_vlines (grid));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static GdkGC*
gtk_databox_grid_real_create_gc (GtkDataboxGraph * graph,
				 GtkDatabox* box)
{
   GdkGC *gc;
   GdkGCValues values;

   g_return_val_if_fail (GTK_DATABOX_IS_GRID (graph), NULL);

   gc = GTK_DATABOX_GRAPH_CLASS (parent_class)->create_gc (graph, box);

   if (gc)
   {
      values.line_style = GDK_LINE_ON_OFF_DASH;
      values.cap_style = GDK_CAP_BUTT;
      values.join_style = GDK_JOIN_MITER;
      gdk_gc_set_values (gc, &values,
			 GDK_GC_LINE_STYLE |
			 GDK_GC_CAP_STYLE | GDK_GC_JOIN_STYLE);
   }

   return gc;
}

static void
grid_finalize (GObject * object)
{
   GtkDataboxGrid *grid = GTK_DATABOX_GRID (object);

   g_free (grid->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_databox_grid_class_init (gpointer g_class /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *graph_class = GTK_DATABOX_GRAPH_CLASS (g_class);
   GtkDataboxGridClass *klass = GTK_DATABOX_GRID_CLASS (g_class);
   GParamSpec *grid_param_spec;

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->set_property = gtk_databox_grid_set_property;
   gobject_class->get_property = gtk_databox_grid_get_property;
   gobject_class->finalize = grid_finalize;

   grid_param_spec = g_param_spec_int ("grid-hlines", "grid-hlines", "Number of horizontal lines", G_MININT, G_MAXINT, 0,	/* default value */
				       G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    GRID_HLINES, grid_param_spec);

   grid_param_spec = g_param_spec_int ("grid-vlines", "grid-vlines", "Number of vertical lines", G_MININT, G_MAXINT, 0,	/* default value */
				       G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    GRID_VLINES, grid_param_spec);

   graph_class->draw = gtk_databox_grid_real_draw;
   graph_class->create_gc = gtk_databox_grid_real_create_gc;
}

static void
gtk_databox_grid_instance_init (GTypeInstance * instance	/*,
								   gpointer         g_class */ )
{
   GtkDataboxGrid *grid = GTK_DATABOX_GRID (instance);

   grid->priv = g_new0 (GtkDataboxGridPrivate, 1);
}

GType
gtk_databox_grid_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxGridClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_grid_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxGrid),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_grid_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (GTK_DATABOX_TYPE_GRAPH,
				     "GtkDataboxGrid", &info, 0);
   }

   return type;
}

/**
 * gtk_databox_grid_new:
 * @hlines: number of horizontal lines in the grid
 * @vlines: number of vertical lines in the grid
 * @color: color of the grid
 * @size: line width of the grid
 *
 * Creates a new #GtkDataboxGrid object which can be added to a #GtkDatabox widget as nice decoration for other graphs.
 *
 * Return value: A new #GtkDataboxGrid object
 **/
GtkDataboxGraph *
gtk_databox_grid_new (gint hlines, gint vlines, GdkColor * color, guint size)
{
   GtkDataboxGrid *grid;

   grid = g_object_new (GTK_DATABOX_TYPE_GRID,
			"color", color,
			"size", size,
			"grid-hlines", hlines, "grid-vlines", vlines, NULL);

   return GTK_DATABOX_GRAPH (grid);
}

/**
 * gtk_databox_grid_array_new:
 * @hlines: number of horizontal lines in the grid
 * @vlines: number of vertical lines in the grid
 * @hline_vals: a pointer to an array of gfloat horizontal grid coordinate
 * @vline_vals: a pointer to an array of gfloat vertical grid coordinate
 * @color: color of the grid
 * @size: line width of the grid
 *
 * Creates a new #GtkDataboxGrid object which can be added to a #GtkDatabox widget as nice decoration for other graphs.
 *
 * Return value: A new #GtkDataboxGrid object
 **/
GtkDataboxGraph *gtk_databox_grid_array_new (gint hlines, gint vlines, gfloat * local_hline_vals, gfloat * local_vline_vals,
					  GdkColor * color, guint size)
{
   GtkDataboxGrid *grid;

   grid = g_object_new (GTK_DATABOX_TYPE_GRID,
			"color", color,
			"size", size,
			"grid-hlines", hlines, "grid-vlines", vlines, "grid-hline-vals", NULL);
			//"grid-hlines", hlines, "grid-vlines", vlines, "grid-hline-vals", hline_vals, "grid-vline-vals", vline_vals, NULL);
   hline_vals = local_hline_vals;
   vline_vals = local_vline_vals;

   return GTK_DATABOX_GRAPH (grid);
}

static void
gtk_databox_grid_real_draw (GtkDataboxGraph * graph,
			    GtkDatabox* box)
{
   GtkWidget *widget;
   GtkDataboxGrid *grid = GTK_DATABOX_GRID (graph);
   GdkGC *gc;
   GdkPixmap *pixmap;
   gint i = 0;
   gfloat x;
   gfloat y;
   gint16 width;
   gint16 height;
   gfloat offset_x;
   gfloat offset_y;
   gfloat factor_x;
   gfloat factor_y;
   gint16 pixel_x;
   gint16 pixel_y;
   gfloat left, right, top, bottom;

   g_return_if_fail (GTK_DATABOX_IS_GRID (grid));
   g_return_if_fail (GTK_IS_DATABOX (box));

   widget = GTK_WIDGET(box);

   pixmap = gtk_databox_get_backing_pixmap (box);
   gtk_databox_get_total_limits (box, &left, &right, &top, &bottom);

   if (!(gc = gtk_databox_graph_get_gc(graph)))
      gc = gtk_databox_graph_create_gc (graph, box);


   width = widget->allocation.width;
   height = widget->allocation.height;

   offset_x = left;
   factor_x =
      (right - left) / (grid->priv->vlines + 1);

   offset_y = top;
   factor_y =
      (bottom - top) / (grid->priv->hlines + 1);

   if (hline_vals == NULL)
      for (i = 0; i < grid->priv->hlines; i++)
      {
         y = offset_y + (i + 1) * factor_y;
         pixel_y = gtk_databox_value_to_pixel_y (box, y);
         gdk_draw_line (pixmap, gc, 0, pixel_y, width, pixel_y);
      }
   else
      for (i = 0; i < grid->priv->hlines; i++)
      {
         y = hline_vals[i];
         pixel_y = gtk_databox_value_to_pixel_y (box, y);
         gdk_draw_line (pixmap, gc, 0, pixel_y, width, pixel_y);
      }

   if (vline_vals == NULL)
      for (i = 0; i < grid->priv->vlines; i++)
      {
         x = offset_x + (i + 1) * factor_x;
         pixel_x = gtk_databox_value_to_pixel_x (box, x);
         gdk_draw_line (pixmap, gc, pixel_x, 0, pixel_x, height);
      }
   else
      for (i = 0; i < grid->priv->vlines; i++)
      {
         x = vline_vals[i];
         pixel_x = gtk_databox_value_to_pixel_x (box, x);
         gdk_draw_line (pixmap, gc, pixel_x, 0, pixel_x, height);
      }


   return;
}

/**
 * gtk_databox_grid_set_hlines:
 * @grid: a #GtkDataboxGrid graph object
 * @hlines: number of vertical lines in the grid
 *
 * Sets the number of horizontal lines in the @grid.
 **/
void
gtk_databox_grid_set_hlines (GtkDataboxGrid * grid, gint hlines)
{
   g_return_if_fail (GTK_DATABOX_IS_GRID (grid));

   grid->priv->hlines = MAX (1, hlines);

   g_object_notify (G_OBJECT (grid), "grid-hlines");
}

/**
 * gtk_databox_grid_get_hlines:
 * @grid: a #GtkDataboxGrid graph object
 *
 * Gets the number of horizontal lines in the @grid.
 *
 * Return value: Number of horizontal lines in the @grid.
 **/
gint
gtk_databox_grid_get_hlines (GtkDataboxGrid * grid)
{
   g_return_val_if_fail (GTK_DATABOX_IS_GRID (grid), -1);

   return grid->priv->hlines;
}

/**
 * gtk_databox_grid_set_vlines:
 * @grid: a #GtkDataboxGrid graph object
 * @vlines: number of vertical lines in the grid
 *
 * Sets the number of vertical lines in the @grid.
 **/
void
gtk_databox_grid_set_vlines (GtkDataboxGrid * grid, gint vlines)
{
   g_return_if_fail (GTK_DATABOX_IS_GRID (grid));

   grid->priv->vlines = MAX (1, vlines);

   g_object_notify (G_OBJECT (grid), "grid-vlines");
}

/**
 * gtk_databox_grid_get_vlines:
 * @grid: a #GtkDataboxGrid graph object
 *
 * Gets the number of vertical lines in the @grid.
 *
 * Return value: Number of vertical lines in the @grid.
 **/
gint
gtk_databox_grid_get_vlines (GtkDataboxGrid * grid)
{
   g_return_val_if_fail (GTK_DATABOX_IS_GRID (grid), -1);

   return grid->priv->vlines;
}
