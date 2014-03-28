/* $Id: gtkdatabox_xyc_graph.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_xyc_graph.h>

static gint gtk_databox_xyc_graph_real_calculate_extrema (GtkDataboxGraph *
							  xyc_graph,
							  gfloat * min_x,
							  gfloat * max_x,
							  gfloat * min_y,
							  gfloat * max_y);

/* IDs of properties */
enum
{
   PROP_X = 1,
   PROP_Y,
   PROP_LEN
};

struct _GtkDataboxXYCGraphPrivate
{
   guint len;
   gfloat *X;
   gfloat *Y;
   GdkPoint *xyc_graph;
};

static gpointer parent_class = NULL;

static void
gtk_databox_xyc_graph_set_X (GtkDataboxXYCGraph * xyc_graph, gfloat * X)
{
   g_return_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph));
   g_return_if_fail (X);

   xyc_graph->priv->X = X;

   g_object_notify (G_OBJECT (xyc_graph), "X-Values");
}

static void
gtk_databox_xyc_graph_set_Y (GtkDataboxXYCGraph * xyc_graph, gfloat * Y)
{
   g_return_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph));
   g_return_if_fail (Y);

   xyc_graph->priv->Y = Y;

   g_object_notify (G_OBJECT (xyc_graph), "Y-Values");
}

static void
gtk_databox_xyc_graph_set_length (GtkDataboxXYCGraph * xyc_graph, guint len)
{
   g_return_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph));
   g_return_if_fail (len > 0);

   xyc_graph->priv->len = len;

   g_object_notify (G_OBJECT (xyc_graph), "length");
}

static void
gtk_databox_xyc_graph_set_property (GObject * object,
				    guint property_id,
				    const GValue * value, GParamSpec * pspec)
{
   GtkDataboxXYCGraph *xyc_graph = GTK_DATABOX_XYC_GRAPH (object);

   switch (property_id)
   {
   case PROP_X:
      {
	 gtk_databox_xyc_graph_set_X (xyc_graph,
				      (gfloat *) g_value_get_pointer (value));
      }
      break;
   case PROP_Y:
      {
	 gtk_databox_xyc_graph_set_Y (xyc_graph,
				      (gfloat *) g_value_get_pointer (value));
      }
      break;
   case PROP_LEN:
      {
	 gtk_databox_xyc_graph_set_length (xyc_graph,
					   g_value_get_int (value));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

/** 
 * gtk_databox_xyc_graph_get_X:
 * @xyc_graph: A #GtkDataboxXYCGraph object
 *
 * Gets the X values of the @xzc_graph.
 *
 * Return value: Pointer to X values
 */
gfloat *
gtk_databox_xyc_graph_get_X (GtkDataboxXYCGraph * xyc_graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph), NULL);

   return xyc_graph->priv->X;
}

/** 
 * gtk_databox_xyc_graph_get_Y:
 * @xyc_graph: A #GtkDataboxXYCGraph object
 *
 * Gets the Y values of the @xzc_graph.
 *
 * Return value: Pointer to Y values
 */
gfloat *
gtk_databox_xyc_graph_get_Y (GtkDataboxXYCGraph * xyc_graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph), NULL);

   return xyc_graph->priv->Y;
}

/** 
 * gtk_databox_xyc_graph_get_length:
 * @xyc_graph: A #GtkDataboxXYCGraph object
 *
 * Gets the the length of the X and Y values arrays.
 *
 * Return value: Length of X/Y arrays.
 */
guint
gtk_databox_xyc_graph_get_length (GtkDataboxXYCGraph * xyc_graph)
{
   g_return_val_if_fail (GTK_DATABOX_IS_XYC_GRAPH (xyc_graph), 0);

   return xyc_graph->priv->len;
}

static void
gtk_databox_xyc_graph_get_property (GObject * object,
				    guint property_id,
				    GValue * value, GParamSpec * pspec)
{
   GtkDataboxXYCGraph *xyc_graph = GTK_DATABOX_XYC_GRAPH (object);

   switch (property_id)
   {
   case PROP_X:
      {
	 g_value_set_pointer (value, gtk_databox_xyc_graph_get_X (xyc_graph));
      }
      break;
   case PROP_Y:
      {
	 g_value_set_pointer (value, gtk_databox_xyc_graph_get_Y (xyc_graph));
      }
      break;
   case PROP_LEN:
      {
	 g_value_set_int (value,
			  gtk_databox_xyc_graph_get_length (xyc_graph));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
xyc_graph_finalize (GObject * object)
{
   GtkDataboxXYCGraph *xyc_graph = GTK_DATABOX_XYC_GRAPH (object);

   g_free (xyc_graph->priv->xyc_graph);
   g_free (xyc_graph->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_databox_xyc_graph_class_init (gpointer g_class
				  /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *graph_class = GTK_DATABOX_GRAPH_CLASS (g_class);
   GtkDataboxXYCGraphClass *klass = GTK_DATABOX_XYC_GRAPH_CLASS (g_class);
   GParamSpec *xyc_graph_param_spec;

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->set_property = gtk_databox_xyc_graph_set_property;
   gobject_class->get_property = gtk_databox_xyc_graph_get_property;
   gobject_class->finalize = xyc_graph_finalize;

   xyc_graph_param_spec = g_param_spec_pointer ("X-Values",
						"X coordinates",
						"X values of data",
						G_PARAM_CONSTRUCT_ONLY |
						G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    PROP_X, xyc_graph_param_spec);

   xyc_graph_param_spec = g_param_spec_pointer ("Y-Values",
						"Y coordinates",
						"Y values of data",
						G_PARAM_CONSTRUCT_ONLY |
						G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    PROP_Y, xyc_graph_param_spec);

   xyc_graph_param_spec = g_param_spec_int ("length", "length of X and Y", "number of data points", G_MININT, G_MAXINT, 0,	/* default value */
					    G_PARAM_CONSTRUCT_ONLY |
					    G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    PROP_LEN, xyc_graph_param_spec);

   graph_class->calculate_extrema =
      gtk_databox_xyc_graph_real_calculate_extrema;
}

static void
gtk_databox_xyc_graph_instance_init (GTypeInstance * instance
				     /*, gpointer g_class */ )
{
   GtkDataboxXYCGraph *xyc_graph = GTK_DATABOX_XYC_GRAPH (instance);

   xyc_graph->priv = g_new0 (GtkDataboxXYCGraphPrivate, 1);
}

GType
gtk_databox_xyc_graph_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxXYCGraphClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_xyc_graph_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxXYCGraph),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_xyc_graph_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (GTK_DATABOX_TYPE_GRAPH,
				     "GtkDataboxXYCGraph", &info, 0);
   }

   return type;
}

static gint
gtk_databox_xyc_graph_real_calculate_extrema (GtkDataboxGraph * graph,
					      gfloat * min_x, gfloat * max_x,
					      gfloat * min_y, gfloat * max_y)
{
   GtkDataboxXYCGraph *xyc_graph = GTK_DATABOX_XYC_GRAPH (graph);
   guint i;

   g_return_val_if_fail (GTK_DATABOX_IS_XYC_GRAPH (graph), -1);
   g_return_val_if_fail (min_x, -1);
   g_return_val_if_fail (max_x, -1);
   g_return_val_if_fail (min_y, -1);
   g_return_val_if_fail (max_y, -1);
   g_return_val_if_fail (xyc_graph->priv->len, -1);

   *min_x = *max_x = xyc_graph->priv->X[0];
   *min_y = *max_y = xyc_graph->priv->Y[0];

   for (i = 1; i < xyc_graph->priv->len; ++i)
   {
      if (xyc_graph->priv->X[i] < *min_x)
	 *min_x = xyc_graph->priv->X[i];
      else if (xyc_graph->priv->X[i] > *max_x)
	 *max_x = xyc_graph->priv->X[i];
      if (xyc_graph->priv->Y[i] < *min_y)
	 *min_y = xyc_graph->priv->Y[i];
      else if (xyc_graph->priv->Y[i] > *max_y)
	 *max_y = xyc_graph->priv->Y[i];

   }

   return 0;
}
