/* $Id: gtkdatabox_bars.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_bars.h>

static void gtk_databox_bars_real_draw (GtkDataboxGraph * bars,
					GtkDatabox* box);

struct _GtkDataboxBarsPrivate
{
   GdkSegment *data;
};

static gpointer parent_class = NULL;

static void
bars_finalize (GObject * object)
{
   GtkDataboxBars *bars = GTK_DATABOX_BARS (object);

   g_free (bars->priv->data);
   g_free (bars->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_databox_bars_class_init (gpointer g_class /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *graph_class = GTK_DATABOX_GRAPH_CLASS (g_class);
   GtkDataboxBarsClass *klass = GTK_DATABOX_BARS_CLASS (g_class);

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->finalize = bars_finalize;

   graph_class->draw = gtk_databox_bars_real_draw;
}

static void
gtk_databox_bars_complete (GtkDataboxBars * bars)
{
   bars->priv->data =
      g_new0 (GdkSegment,
	      gtk_databox_xyc_graph_get_length
	      (GTK_DATABOX_XYC_GRAPH (bars)));

}

static void
gtk_databox_bars_instance_init (GTypeInstance * instance	/*,
								   gpointer         g_class */ )
{
   GtkDataboxBars *bars = GTK_DATABOX_BARS (instance);

   bars->priv = g_new0 (GtkDataboxBarsPrivate, 1);

   g_signal_connect (bars, "notify::length",
		     G_CALLBACK (gtk_databox_bars_complete), NULL);
}

GType
gtk_databox_bars_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxBarsClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_bars_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxBars),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_bars_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (GTK_DATABOX_TYPE_XYC_GRAPH,
				     "GtkDataboxBars", &info, 0);
   }

   return type;
}

/**
 * gtk_databox_bars_new:
 * @len: length of @X and @Y
 * @X: array of horizontal position values of markers
 * @Y: array of vertical position values of markers
 * @color: color of the markers
 * @size: marker size or line width (depending on the @type)
 *
 * Creates a new #GtkDataboxBars object which can be added to a #GtkDatabox widget
 *
 * Return value: A new #GtkDataboxBars object
 **/
GtkDataboxGraph *
gtk_databox_bars_new (guint len, gfloat * X, gfloat * Y,
		      GdkColor * color, guint size)
{
   GtkDataboxBars *bars;
   g_return_val_if_fail (X, NULL);
   g_return_val_if_fail (Y, NULL);
   g_return_val_if_fail ((len > 0), NULL);

   bars = g_object_new (GTK_DATABOX_TYPE_BARS,
			"X-Values", X,
			"Y-Values", Y,
			"length", len, "color", color, "size", size, NULL);

   return GTK_DATABOX_GRAPH (bars);
}

static void
gtk_databox_bars_real_draw (GtkDataboxGraph * graph,
			    GtkDatabox* box)
{
   GtkDataboxBars *bars = GTK_DATABOX_BARS (graph);
   GdkSegment *data;
   GdkGC *gc;
   GdkPixmap *pixmap;
   guint i = 0;
   gfloat *X;
   gfloat *Y;
   guint len;
   gint size = 0;
   gint16 zero = 0;

   g_return_if_fail (GTK_DATABOX_IS_BARS (bars));
   g_return_if_fail (GTK_IS_DATABOX (box));

   pixmap = gtk_databox_get_backing_pixmap (box);

   if (gtk_databox_get_scale_type_y (box) == GTK_DATABOX_SCALE_LOG)
      g_warning
	 ("gtk_databox_bars do not work well with logarithmic scale in Y axis");

   if (!(gc = gtk_databox_graph_get_gc(graph)))
      gc = gtk_databox_graph_create_gc (graph, box);

   len = gtk_databox_xyc_graph_get_length (GTK_DATABOX_XYC_GRAPH (graph));
   X = gtk_databox_xyc_graph_get_X (GTK_DATABOX_XYC_GRAPH (graph));
   Y = gtk_databox_xyc_graph_get_Y (GTK_DATABOX_XYC_GRAPH (graph));
   size = gtk_databox_graph_get_size (graph);

   data = bars->priv->data;

   zero = gtk_databox_value_to_pixel_y (box, 0);

   for (i = 0; i < len; i++, data++, X++, Y++)
   {
      data->x1 = data->x2 = gtk_databox_value_to_pixel_x (box, *X);
      data->y1 = zero;
      data->y2 = gtk_databox_value_to_pixel_y (box, *Y);
   }

   /* More than 2^16 bars will cause X IO error on most XServers
      (Hint from Paul Barton-Davis) */
   for (i = 0; i < len; i += 65536)
   {
      gdk_draw_segments (pixmap, gc,
			 bars->priv->data + i, MIN (65536, len - i));
   }

   return;
}
