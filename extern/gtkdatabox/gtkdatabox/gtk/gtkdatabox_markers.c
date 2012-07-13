/* $Id: gtkdatabox_markers.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_markers.h>
#include <pango/pango.h>

static void gtk_databox_markers_real_draw (GtkDataboxGraph * markers,
					  GtkDatabox* box);
static GdkGC* gtk_databox_markers_real_create_gc (GtkDataboxGraph * graph,
					       GtkDatabox* box);

/* IDs of properties */
enum
{
   PROP_TYPE = 1
};


typedef struct
{
   GtkDataboxMarkersPosition position;	/* relative to data point */
   gchar *text;
   PangoLayout *label;		/* the label for markers */
   GtkDataboxMarkersTextPosition label_position;	/* position relative to markers */
   gboolean boxed;		/* label in a box? */
}
GtkDataboxMarkersInfo;

struct _GtkDataboxMarkersPrivate
{
   GtkDataboxMarkersType type;
   GtkDataboxMarkersInfo *markers_info;
   GdkGC *label_gc;
};

static gpointer parent_class = NULL;

static void
gtk_databox_markers_set_mtype (GtkDataboxMarkers * markers, gint type)
{
   g_return_if_fail (GTK_DATABOX_IS_MARKERS (markers));

   markers->priv->type = type;

   g_object_notify (G_OBJECT (markers), "markers-type");
}

static void
gtk_databox_markers_set_property (GObject * object,
				 guint property_id,
				 const GValue * value, GParamSpec * pspec)
{
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (object);

   switch (property_id)
   {
   case PROP_TYPE:
      {
	 gtk_databox_markers_set_mtype (markers, g_value_get_int (value));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static gint
gtk_databox_markers_get_mtype (GtkDataboxMarkers * markers)
{
   g_return_val_if_fail (GTK_DATABOX_IS_MARKERS (markers), 0);

   return markers->priv->type;
}

static void
gtk_databox_markers_get_property (GObject * object,
				 guint property_id,
				 GValue * value, GParamSpec * pspec)
{
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (object);

   switch (property_id)
   {
   case PROP_TYPE:
      {
	 g_value_set_int (value, gtk_databox_markers_get_mtype (markers));
      }
      break;
   default:
      /* We don't have any other property... */
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
   }
}

static void
markers_finalize (GObject * object)
{
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (object);
   int i;
   int len;

   len = gtk_databox_xyc_graph_get_length (GTK_DATABOX_XYC_GRAPH (markers));

   for (i = 0; i < len; ++i)
   {
      if (markers->priv->markers_info[i].label)
	 g_object_unref (markers->priv->markers_info[i].label);
      if (markers->priv->markers_info[i].text)
	 g_free (markers->priv->markers_info[i].text);
   }
   g_free (markers->priv->markers_info);
   g_free (markers->priv);

   /* Chain up to the parent class */
   G_OBJECT_CLASS (parent_class)->finalize (object);
}

static GdkGC *
gtk_databox_markers_real_create_gc (GtkDataboxGraph * graph,
				   GtkDatabox* box)
{
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (graph);
   GdkGC *gc;
   GdkGCValues values;

   g_return_val_if_fail (GTK_DATABOX_IS_MARKERS (graph), NULL);

   gc = GTK_DATABOX_GRAPH_CLASS (parent_class)->create_gc (graph, box);

   if (gc)
   {
      if (markers->priv->type == GTK_DATABOX_MARKERS_DASHED_LINE)
      {
         values.line_style = GDK_LINE_ON_OFF_DASH;
         values.cap_style = GDK_CAP_BUTT;
         values.join_style = GDK_JOIN_MITER;
         gdk_gc_set_values (gc, &values,
			    GDK_GC_LINE_STYLE |
			    GDK_GC_CAP_STYLE | GDK_GC_JOIN_STYLE);
      }
   
      if (markers->priv->label_gc)
         g_object_unref (markers->priv->label_gc);
   
      markers->priv->label_gc = gdk_gc_new (gtk_databox_get_backing_pixmap (box));
      gdk_gc_copy (markers->priv->label_gc, gc);
      gdk_gc_set_line_attributes (markers->priv->label_gc, 1,
			          GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
   }

   return gc;
}

static void
gtk_databox_markers_class_init (gpointer g_class /*, gpointer g_class_data */ )
{
   GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
   GtkDataboxGraphClass *graph_class = GTK_DATABOX_GRAPH_CLASS (g_class);
   GtkDataboxMarkersClass *klass = GTK_DATABOX_MARKERS_CLASS (g_class);
   GParamSpec *markers_param_spec;

   parent_class = g_type_class_peek_parent (klass);

   gobject_class->set_property = gtk_databox_markers_set_property;
   gobject_class->get_property = gtk_databox_markers_get_property;
   gobject_class->finalize = markers_finalize;

   markers_param_spec = g_param_spec_int ("markers-type", "Type of markers", "Type of markers for this graph, e.g. triangles or lines", G_MININT, G_MAXINT, 0,	/*  default value */
					 G_PARAM_CONSTRUCT |
					 G_PARAM_READWRITE);

   g_object_class_install_property (gobject_class,
				    PROP_TYPE, markers_param_spec);
   graph_class->draw = gtk_databox_markers_real_draw;
   graph_class->create_gc = gtk_databox_markers_real_create_gc;
}

static void
complete (GtkDataboxMarkers * markers)
{
   markers->priv->markers_info =
      g_new0 (GtkDataboxMarkersInfo,
	      gtk_databox_xyc_graph_get_length
	      (GTK_DATABOX_XYC_GRAPH (markers)));

}

static void
gtk_databox_markers_instance_init (GTypeInstance * instance	/*,
								   gpointer g_class */ )
{
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (instance);

   markers->priv = g_new0 (GtkDataboxMarkersPrivate, 1);

   g_signal_connect (markers, "notify::length", G_CALLBACK (complete), NULL);
}

GType
gtk_databox_markers_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GTypeInfo info = {
	 sizeof (GtkDataboxMarkersClass),
	 NULL,			/* base_init */
	 NULL,			/* base_finalize */
	 (GClassInitFunc) gtk_databox_markers_class_init,	/* class_init */
	 NULL,			/* class_finalize */
	 NULL,			/* class_data */
	 sizeof (GtkDataboxMarkers),	/* instance_size */
	 0,			/* n_preallocs */
	 (GInstanceInitFunc) gtk_databox_markers_instance_init,	/* instance_init */
	 NULL,			/* value_table */
      };
      type = g_type_register_static (GTK_DATABOX_TYPE_XYC_GRAPH,
				     "GtkDataboxMarkers", &info, 0);
   }

   return type;
}

/**
 * gtk_databox_markers_new:
 * @len: length of @X and @Y
 * @X: array of horizontal position values of markers
 * @Y: array of vertical position values of markers
 * @color: color of the markers
 * @size: marker size or line width (depending on the @type)
 * @type: type of markers (e.g. triangle or circle)
 *
 * Creates a new #GtkDataboxMarkers object which can be added to a #GtkDatabox widget as nice decoration for other graphs.
 *
 * Return value: A new #GtkDataboxMarkers object
 **/
GtkDataboxGraph *
gtk_databox_markers_new (guint len, gfloat * X, gfloat * Y,
			GdkColor * color, guint size,
			GtkDataboxMarkersType type)
{
   GtkDataboxMarkers *markers;
   g_return_val_if_fail (X, NULL);
   g_return_val_if_fail (Y, NULL);
   g_return_val_if_fail ((len > 0), NULL);

   markers = g_object_new (GTK_DATABOX_TYPE_MARKERS,
			  "X-Values", X,
			  "Y-Values", Y,
			  "length", len,
			  "color", color,
			  "size", size, "markers-type", type, NULL);

   return GTK_DATABOX_GRAPH (markers);
}

static gint
gtk_databox_label_write_at (GdkPixmap * pixmap,
			    PangoLayout * pl,
			    GdkGC * gc,
			    GdkPoint coord,
			    GtkDataboxMarkersTextPosition position,
			    gint distance, gboolean boxed)
{
   gint hdist_text;
   gint vdist_text;
   gint hdist_box;
   gint vdist_box;

   gint width;
   gint height;

   gint offset = (boxed) ? 2 : 0;

   pango_layout_get_pixel_size (pl, &width, &height);

   switch (position)
   {
   case GTK_DATABOX_MARKERS_TEXT_N:
      hdist_text = -width / 2;
      vdist_text = -distance - offset - height;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_NE:
      hdist_text = +distance + offset;
      vdist_text = -distance - offset - height;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_E:
      hdist_text = +distance + offset;
      vdist_text = -height / 2;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_SE:
      hdist_text = +distance + offset;
      vdist_text = +distance + offset;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_S:
      hdist_text = -width / 2;
      vdist_text = +distance + offset;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_SW:
      hdist_text = -distance - offset - width;
      vdist_text = +distance + offset;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_W:
      hdist_text = -distance - offset - width;
      vdist_text = -height / 2;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   case GTK_DATABOX_MARKERS_TEXT_NW:
      hdist_text = -distance - offset - width;
      vdist_text = -distance - offset - height;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
      break;
   default:
      hdist_text = -width / 2;
      vdist_text = -height / 2;
      hdist_box = hdist_text - offset;
      vdist_box = vdist_text - offset;
   }

   gdk_draw_layout (pixmap, gc,
		    coord.x + hdist_text, coord.y + vdist_text, pl);

   if (boxed)
      gdk_draw_rectangle (pixmap, gc, FALSE,
			  coord.x + hdist_box,
			  coord.y + vdist_box, width + 3, height + 3);

   return (0);
}

static void
gtk_databox_markers_real_draw (GtkDataboxGraph * graph,
			      GtkDatabox* box)
{
   GtkWidget *widget;
   GtkDataboxMarkers *markers = GTK_DATABOX_MARKERS (graph);
   GdkPoint points[3];
   GdkPixmap *pixmap;
   GdkGC *gc;
   PangoContext *context;
   gfloat *X;
   gfloat *Y;
   guint len;
   gint16 x;
   gint16 y;
   gint16 widget_width;
   gint16 widget_height;
   GdkPoint coord;
   gint size;
   guint i;

   g_return_if_fail (GTK_DATABOX_IS_MARKERS (markers));
   g_return_if_fail (GTK_IS_DATABOX (box));

   widget = GTK_WIDGET(box);

   pixmap = gtk_databox_get_backing_pixmap (box);
   context = gtk_widget_get_pango_context(widget);

   if (!(gc = gtk_databox_graph_get_gc(graph)))
      gc = gtk_databox_graph_create_gc (graph, box);

   len = gtk_databox_xyc_graph_get_length (GTK_DATABOX_XYC_GRAPH (graph));
   X = gtk_databox_xyc_graph_get_X (GTK_DATABOX_XYC_GRAPH (graph));
   Y = gtk_databox_xyc_graph_get_Y (GTK_DATABOX_XYC_GRAPH (graph));
   size = gtk_databox_graph_get_size (graph);

   widget_width = widget->allocation.width;
   widget_height = widget->allocation.height;

   for (i = 0; i < len; ++i)
   {
      coord.x = x = gtk_databox_value_to_pixel_x (box, X[i]);
      coord.y = y = gtk_databox_value_to_pixel_y (box, Y[i]);

      switch (markers->priv->type)
      {
      case GTK_DATABOX_MARKERS_TRIANGLE:
	 switch (markers->priv->markers_info[i].position)
	 {
	 case GTK_DATABOX_MARKERS_C:
	    y = y - size / 2;
	    points[0].x = x;
	    points[0].y = y;
	    points[1].x = x - size / 2;
	    points[1].y = y + size;
	    points[2].x = x + size / 2;
	    points[2].y = y + size;
	    break;
	 case GTK_DATABOX_MARKERS_N:
	    coord.y = y - 2 - size / 2;
	    y = y - 2;
	    points[0].x = x;
	    points[0].y = y;
	    points[1].x = x - size / 2;
	    points[1].y = y - size;
	    points[2].x = x + size / 2;
	    points[2].y = y - size;
	    break;
	 case GTK_DATABOX_MARKERS_E:
	    coord.x = x + 2 + size / 2;
	    x = x + 2;
	    points[0].x = x;
	    points[0].y = y;
	    points[1].x = x + size;
	    points[1].y = y + size / 2;
	    points[2].x = x + size;
	    points[2].y = y - size / 2;
	    break;
	 case GTK_DATABOX_MARKERS_S:
	    coord.y = y + 2 + size / 2;
	    y = y + 2;
	    points[0].x = x;
	    points[0].y = y;
	    points[1].x = x - size / 2;
	    points[1].y = y + size;
	    points[2].x = x + size / 2;
	    points[2].y = y + size;
	    break;
	 case GTK_DATABOX_MARKERS_W:
	    coord.x = x - 2 - size / 2;
	    x = x - 2;
	    points[0].x = x;
	    points[0].y = y;
	    points[1].x = x - size;
	    points[1].y = y + size / 2;
	    points[2].x = x - size;
	    points[2].y = y - size / 2;
	    break;
	 }
	 gdk_draw_polygon (pixmap, gc, TRUE, points, 3);
	 break;
	 /* End of GTK_DATABOX_MARKERS_TRIANGLE */
      case GTK_DATABOX_MARKERS_SOLID_LINE:
      case GTK_DATABOX_MARKERS_DASHED_LINE:
	 switch (markers->priv->markers_info[i].position)
	 {
	 case GTK_DATABOX_MARKERS_C:
	    points[0].x = x;
	    points[0].y = 0;
	    points[1].x = x;
	    points[1].y = widget_height;
	    break;
	 case GTK_DATABOX_MARKERS_N:
	    points[0].x = x;
	    points[0].y = 0;
	    points[1].x = x;
	    points[1].y = widget_height;
	    break;
	 case GTK_DATABOX_MARKERS_E:
	    points[0].x = 0;
	    points[0].y = y;
	    points[1].x = widget_width;
	    points[1].y = y;
	    break;
	 case GTK_DATABOX_MARKERS_S:
	    points[0].x = x;
	    points[0].y = 0;
	    points[1].x = x;
	    points[1].y = widget_height;
	    break;
	 case GTK_DATABOX_MARKERS_W:
	    points[0].x = 0;
	    points[0].y = y;
	    points[1].x = widget_width;
	    points[1].y = y;
	    break;
	 }

	 gdk_draw_line (pixmap, gc,
			points[0].x, points[0].y, points[1].x, points[1].y);
	 break;
	 /* End of GTK_DATABOX_MARKERS_LINE */

      case GTK_DATABOX_MARKERS_NONE:
      default:
	 break;
      }

      if (markers->priv->markers_info[i].text)
      {
	 if (!markers->priv->markers_info[i].label)
	 {
	    markers->priv->markers_info[i].label =
	       pango_layout_new (context);
	    pango_layout_set_text (markers->priv->markers_info[i].label,
				   markers->priv->markers_info[i].text, -1);
	 }

	 if (markers->priv->type == GTK_DATABOX_MARKERS_SOLID_LINE
	     || markers->priv->type == GTK_DATABOX_MARKERS_DASHED_LINE)
	 {
	    gint width;
	    gint height;
	    pango_layout_get_pixel_size (markers->priv->markers_info[i].label,
					 &width, &height);

	    width = (width + 1) / 2 + 2;
	    height = (height + 1) / 2 + 2;
	    size = 0;

	    switch (markers->priv->markers_info[i].position)
	    {
	    case GTK_DATABOX_MARKERS_C:
	       break;
	    case GTK_DATABOX_MARKERS_N:
	       coord.y = height;
	       break;
	    case GTK_DATABOX_MARKERS_E:
	       coord.x = widget_width - width;
	       break;
	    case GTK_DATABOX_MARKERS_S:
	       coord.y = widget_height - height;
	       break;
	    case GTK_DATABOX_MARKERS_W:
	       coord.x = width;
	       break;
	    }
	 }

	 gtk_databox_label_write_at (pixmap,
				     markers->priv->markers_info[i].label,
				     markers->priv->label_gc, coord,
				     markers->priv->markers_info[i].
				     label_position, (size + 1) / 2 + 2,
				     markers->priv->markers_info[i].boxed);
      }
   }

   return;
}

/**
 * gtk_databox_markers_set_position:
 * @markers: A #GtkDataboxMarkers object
 * @index: index within the array of X/Y values
 * @position: position of the marker (e.g. circle or triangle relative to their X/Y value
 *
 * Sets a position for one of the markers.
 **/
void
gtk_databox_markers_set_position (GtkDataboxMarkers * markers,
				 guint index,
				 GtkDataboxMarkersPosition position)
{
   guint len;

   g_return_if_fail (GTK_DATABOX_IS_MARKERS (markers));
   len = gtk_databox_xyc_graph_get_length (GTK_DATABOX_XYC_GRAPH (markers));
   g_return_if_fail (index < len);

   markers->priv->markers_info[index].position = position;
}

/**
 * gtk_databox_markers_set_label:
 * @markers: A #GtkDataboxMarkers object
 * @index: index within the array of X/Y values
 * @label_position: position of the label relative to the marker
 * @text: text to be displayed in the label
 * @boxed: Whether the label is to be enclosed in a box (true) or not (false)
 *
 * Sets a label for one of the markers.
 **/
void
gtk_databox_markers_set_label (GtkDataboxMarkers * markers,
			      guint index,
			      GtkDataboxMarkersTextPosition label_position,
			      gchar * text, gboolean boxed)
{
   guint len;

   g_return_if_fail (GTK_DATABOX_IS_MARKERS (markers));
   len = gtk_databox_xyc_graph_get_length (GTK_DATABOX_XYC_GRAPH (markers));
   g_return_if_fail (index < len);

   markers->priv->markers_info[index].label_position = label_position;
   if (markers->priv->markers_info[index].text)
      g_free (markers->priv->markers_info[index].text);
   markers->priv->markers_info[index].text = g_strdup (text);
   markers->priv->markers_info[index].boxed = boxed;

   if (markers->priv->markers_info[index].label)
   {
      pango_layout_set_text (markers->priv->markers_info[index].label,
			     markers->priv->markers_info[index].text, -1);
   }
}
