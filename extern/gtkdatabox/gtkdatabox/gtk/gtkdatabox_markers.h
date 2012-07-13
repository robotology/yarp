/* $Id: gtkdatabox_markers.h 4 2008-06-22 09:19:11Z rbock $ */
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

/**
 * SECTION:gtkdatabox_markers
 * @short_description: A #GtkDataboxGraph used for displaying labeled markers for xy-values.
 * @include: gtkdatabox_markers.h
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxPoints, #GtkDataboxLines,  #GtkDataboxBars
 *
 * #GtkDataboxMarkers is a #GtkDataboxGraph class for displaying labeled markers (circles, triangles, lines) for xy-values.
 *
 */

#ifndef __GTK_DATABOX_MARKERS_H__
#define __GTK_DATABOX_MARKERS_H__

#include <gtkdatabox_xyc_graph.h>

G_BEGIN_DECLS
#define GTK_DATABOX_TYPE_MARKERS		  (gtk_databox_markers_get_type ())
#define GTK_DATABOX_MARKERS(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                           GTK_DATABOX_TYPE_MARKERS, \
                                           GtkDataboxMarkers))
#define GTK_DATABOX_MARKERS_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                           GTK_DATABOX_TYPE_MARKERS, \
                                           GtkDataboxMarkersClass))
#define GTK_DATABOX_IS_MARKERS(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           GTK_DATABOX_TYPE_MARKERS))
#define GTK_DATABOX_IS_MARKERS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                           GTK_DATABOX_TYPE_MARKERS))
#define GTK_DATABOX_MARKERS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                           GTK_DATABOX_TYPE_MARKERS, \
                                           GtkDataboxMarkersClass))

/**
 * GtkDataboxMarkers:
 * @parent: The parent object 
 * @priv: A private structure containing internal data.
 * 
 * #GtkDataboxMarkers is a #GtkDataboxGraph class for displaying labeled markers (circles, triangles, lines) for xy-values.
 *
 */
   typedef struct _GtkDataboxMarkers GtkDataboxMarkers;

   typedef struct _GtkDataboxMarkersClass GtkDataboxMarkersClass;

   /**
    * GtkDataboxMarkersPrivate:
    *
    * A private data structure used by the #GtkDataboxMarkers. It shields all internal things
    * from developers who are just using the object. 
    *
    **/
   typedef struct _GtkDataboxMarkersPrivate GtkDataboxMarkersPrivate;

   /**
    * GtkDataboxMarkersPosition:
    * @GTK_DATABOX_MARKERS_C: centered on data
    * @GTK_DATABOX_MARKERS_N: marker is positioned north of data
    * @GTK_DATABOX_MARKERS_E: marker is positioned east of data
    * @GTK_DATABOX_MARKERS_S: marker is positioned south of data
    * @GTK_DATABOX_MARKERS_W: marker is positioned west of data
    *
    * Position of a marker relative to its respective X/Y value.
    */
   typedef enum
   {
      GTK_DATABOX_MARKERS_C = 0,
      GTK_DATABOX_MARKERS_N,
      GTK_DATABOX_MARKERS_E,
      GTK_DATABOX_MARKERS_S,
      GTK_DATABOX_MARKERS_W
   }
   GtkDataboxMarkersPosition;

   /**
    * GtkDataboxMarkersTextPosition:
    * @GTK_DATABOX_MARKERS_TEXT_CENTER:   text centered   on marker
    * @GTK_DATABOX_MARKERS_TEXT_N:	  text north      of marker
    * @GTK_DATABOX_MARKERS_TEXT_NE:	  text north-east of marker
    * @GTK_DATABOX_MARKERS_TEXT_E:	  text east       of marker
    * @GTK_DATABOX_MARKERS_TEXT_SE:	  text south-east of marker
    * @GTK_DATABOX_MARKERS_TEXT_S:	  text south      of marker
    * @GTK_DATABOX_MARKERS_TEXT_SW:	  text south-west of marker
    * @GTK_DATABOX_MARKERS_TEXT_W:	  text west       of marker
    * @GTK_DATABOX_MARKERS_TEXT_NW:       text north-west of marker
    *
    * Position of a label relative to its repective marker.
    */
   typedef enum
   {
      GTK_DATABOX_MARKERS_TEXT_CENTER = 0,/* text centered   on marker */
      GTK_DATABOX_MARKERS_TEXT_N,	  /* text north      of marker */
      GTK_DATABOX_MARKERS_TEXT_NE,	  /* text north-east of marker */
      GTK_DATABOX_MARKERS_TEXT_E,	  /* text east       of marker */
      GTK_DATABOX_MARKERS_TEXT_SE,	  /* text south-east of marker */
      GTK_DATABOX_MARKERS_TEXT_S,	  /* text south      of marker */
      GTK_DATABOX_MARKERS_TEXT_SW,	  /* text south-west of marker */
      GTK_DATABOX_MARKERS_TEXT_W,	  /* text west       of marker */
      GTK_DATABOX_MARKERS_TEXT_NW	  /* text north-west of marker */
   }
   GtkDataboxMarkersTextPosition;

   /**
    * GtkDataboxMarkersType:
    * @GTK_DATABOX_MARKERS_NONE:	No Marker (just text)
    * @GTK_DATABOX_MARKERS_TRIANGLE:	Marker is a triangle
    * @GTK_DATABOX_MARKERS_SOLID_LINE:	Marker is a solid line
    * @GTK_DATABOX_MARKERS_DASHED_LINE:	Marker is a dashed line
    *
    * Type of the marker
    */
   typedef enum
   {
      GTK_DATABOX_MARKERS_NONE = 0,	/* No Marker (just text) */
      GTK_DATABOX_MARKERS_TRIANGLE,	/* Marker is a triangle */
      GTK_DATABOX_MARKERS_SOLID_LINE,	/* Marker is a solid line */
      GTK_DATABOX_MARKERS_DASHED_LINE	/* Marker is a dashed line */
   }
   GtkDataboxMarkersType;

   struct _GtkDataboxMarkers
   {
      /*< private >*/
      GtkDataboxXYCGraph parent;

      GtkDataboxMarkersPrivate *priv;
   };

   struct _GtkDataboxMarkersClass
   {
      GtkDataboxXYCGraphClass parent_class;
   };

   GType gtk_databox_markers_get_type (void);

   GtkDataboxGraph *gtk_databox_markers_new (guint len, gfloat * X, gfloat * Y,
					    GdkColor * color, guint size,
					    GtkDataboxMarkersType type);
   void gtk_databox_markers_set_position (GtkDataboxMarkers * markers,
					 guint index,
					 GtkDataboxMarkersPosition position);

   void gtk_databox_markers_set_label (GtkDataboxMarkers * markers,
				      guint index,
				      GtkDataboxMarkersTextPosition label_position,
				      gchar * text, gboolean boxed);

G_END_DECLS
#endif				/* __GTK_DATABOX_MARKERS_H__ */
