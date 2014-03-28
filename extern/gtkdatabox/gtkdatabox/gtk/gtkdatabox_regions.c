/* $Id: gtkdatabox_regions.c 4 2008-06-22 09:19:11Z rbock $ */
/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 2011 - 2012  Dr. Matt Flax <flatmax@>
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

#include <gtkdatabox_regions.h>

static void gtk_databox_regions_real_draw (GtkDataboxGraph * regions,
					GtkDatabox* box);

struct _GtkDataboxRegionsPrivate
{
   GdkPoint *data;
};

static gpointer parent_class = NULL;

static void
regions_finalize (GObject * object)
{
   GtkDataboxRegions *regions = GTK_DATABOX_REGIONS (object);

   g_free (regions->priv->data);
   g_free (regions->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_databox_regions_class_init (gpointer g_class /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *graph_class = GTK_DATABOX_GRAPH_CLASS (g_class);
   GtkDataboxRegionsClass *klass = GTK_DATABOX_REGIONS_CLASS (g_class);

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->finalize = regions_finalize;

   graph_class->draw = gtk_databox_regions_real_draw;
}

static void
gtk_databox_regions_complete (GtkDataboxRegions * regions)
{
   regions->priv->data = g_new0 (GdkPoint, 4);
}

static void
gtk_databox_regions_instance_init (GTypeInstance * instance)
{
   GtkDataboxRegions *regions = GTK_DATABOX_REGIONS (instance);

   regions->priv = g_new0 (GtkDataboxRegionsPrivate, 1);

   g_signal_connect (regions, "notify::length",
		     G_CALLBACK (gtk_databox_regions_complete), NULL);
}

GType
gtk_databox_regions_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxRegionsClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_regions_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxRegions),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_regions_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (GTK_DATABOX_TYPE_XYYC_GRAPH,
				     "GtkDataboxRegions", &info, (GTypeFlags)0);
   }

   return type;
}

/**
 * gtk_databox_regions_new:
 * @len: length of @X, @Y1  and @Y2
 * @X: array of ordinates
 * @Y1: array of co-ordinates
 * @Y2: array of co-ordinates
 * @color: color of the markers
 *
 * Creates a new #GtkDataboxRegions object which can be added to a #GtkDatabox widget
 *
 * Return value: A new #GtkDataboxRegions object
 **/
GtkDataboxGraph *
gtk_databox_regions_new (guint len, gfloat * X, gfloat * Y1, gfloat * Y2, GdkColor * color)
{
   GtkDataboxRegions *regions;
   g_return_val_if_fail (X, NULL);
   g_return_val_if_fail (Y1, NULL);
   g_return_val_if_fail (Y2, NULL);
   g_return_val_if_fail ((len > 0), NULL);

   regions = g_object_new (GTK_DATABOX_TYPE_REGIONS,
			"X-Values", X,
			"Y1-Values", Y1,
			"Y2-Values", Y2,
			"length", len, "color", color,NULL);

   return GTK_DATABOX_GRAPH (regions);
}

static void
gtk_databox_regions_real_draw (GtkDataboxGraph * graph,
			    GtkDatabox* box)
{
   GtkDataboxRegions *regions = GTK_DATABOX_REGIONS (graph);
   GdkPoint *data1, *data2, *data3, *data4;
   GdkGC *gc;
   GdkPixmap *pixmap;
   guint i = 0;
   gfloat *X;
   gfloat *Y1;
   gfloat *Y2;
   guint len;

   g_return_if_fail (GTK_DATABOX_IS_REGIONS (regions));
   g_return_if_fail (GTK_IS_DATABOX (box));

   pixmap = gtk_databox_get_backing_pixmap (box);

   if (gtk_databox_get_scale_type_y (box) == GTK_DATABOX_SCALE_LOG)
      g_warning
	 ("gtk_databox_regions do not work well with logarithmic scale in Y axis");

   if (!(gc = gtk_databox_graph_get_gc(graph)))
      gc = gtk_databox_graph_create_gc (graph, box);

   len = gtk_databox_xyyc_graph_get_length (GTK_DATABOX_XYYC_GRAPH (graph));
   X = gtk_databox_xyyc_graph_get_X (GTK_DATABOX_XYYC_GRAPH (graph));
   Y1 = gtk_databox_xyyc_graph_get_Y1 (GTK_DATABOX_XYYC_GRAPH (graph));
   Y2 = gtk_databox_xyyc_graph_get_Y2 (GTK_DATABOX_XYYC_GRAPH (graph));

   data1 = regions->priv->data+1;
   data2 = regions->priv->data;
   data3 = regions->priv->data+2;
   data4 = regions->priv->data+3;

   data3->x = gtk_databox_value_to_pixel_x (box, *X);
   data3->y = gtk_databox_value_to_pixel_y (box, *Y2);
   data4->x = gtk_databox_value_to_pixel_x (box, *X);
   data4->y = gtk_databox_value_to_pixel_y (box, *Y1);
   X++; Y1++; Y2++;
   for (i = 0; i < len-1; i++, X++, Y1++, Y2++)
   {
      data1->x = data3->x; /* 4 points in the polygon */
      data1->y = data3->y;
      data2->x = data4->x;
      data2->y = data4->y;
      data3->x = gtk_databox_value_to_pixel_x (box, *X);
      data3->y = gtk_databox_value_to_pixel_y (box, *Y2);
      data4->x = gtk_databox_value_to_pixel_x (box, *X);
      data4->y = gtk_databox_value_to_pixel_y (box, *Y1);
      gdk_draw_polygon (pixmap, gc, 1, /* 1 for a filled polygon*/
			 regions->priv->data,4);
   }
   return;
}
