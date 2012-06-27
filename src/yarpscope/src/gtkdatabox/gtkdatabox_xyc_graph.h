/* $Id: gtkdatabox_xyc_graph.h 4 2008-06-22 09:19:11Z rbock $ */
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
 * SECTION:gtkdatabox_xyc_graph
 * @short_description: An abstract anchestor for all graphs which display xy-values in one color.
 * @include: gtkdatabox_xyc_graph.h
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxPoints, #GtkDataboxLines, #GtkDataboxBars
 *
 * GtkDataboxXYCGraphs are an abstract class for displaying XY-data in one color. The values for the data are represented 
 * as an array of X values and a second array of Y values. In order to actually display data, you should
 * use one of the derived classes.
 *
 */

#ifndef __GTK_DATABOX_XYC_GRAPH_H__
#define __GTK_DATABOX_XYC_GRAPH_H__

#include <gtkdatabox_graph.h>

G_BEGIN_DECLS
#define GTK_DATABOX_TYPE_XYC_GRAPH		  (gtk_databox_xyc_graph_get_type ())
#define GTK_DATABOX_XYC_GRAPH(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                           GTK_DATABOX_TYPE_XYC_GRAPH, \
                                           GtkDataboxXYCGraph))
#define GTK_DATABOX_XYC_GRAPH_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                           GTK_DATABOX_TYPE_XYC_GRAPH, \
                                           GtkDataboxXYCGraphClass))
#define GTK_DATABOX_IS_XYC_GRAPH(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           GTK_DATABOX_TYPE_XYC_GRAPH))
#define GTK_DATABOX_IS_XYC_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                           GTK_DATABOX_TYPE_XYC_GRAPH))
#define GTK_DATABOX_XYC_GRAPH_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                           GTK_DATABOX_TYPE_XYC_GRAPH, \
                                           GtkDataboxXYCGraphClass))

/**
 * GtkDataboxXYCGraph:
 *
 * GtkDataboxXYCGraphs are an abstract class for displaying XY-data in one color. The values for the data are represented 
 * as an array of X values and a second array of Y values. In order to actually display data, you should
 * use one of the derived classes.
 *
 */
   typedef struct _GtkDataboxXYCGraph GtkDataboxXYCGraph;

   typedef struct _GtkDataboxXYCGraphClass GtkDataboxXYCGraphClass;

   /**
    * GtkDataboxXYCGraphPrivate
    *
    * A private data structure used by the #GtkDataboxXYCGraph. It shields all internal things
    * from developers who are just using the object. 
    *
    **/
   typedef struct _GtkDataboxXYCGraphPrivate GtkDataboxXYCGraphPrivate;

   struct _GtkDataboxXYCGraph
   {
      /*< private >*/
      GtkDataboxGraph parent;

      GtkDataboxXYCGraphPrivate *priv;
   };

   struct _GtkDataboxXYCGraphClass
   {
      GtkDataboxGraphClass parent_class;
   };

   GType gtk_databox_xyc_graph_get_type (void);

   guint gtk_databox_xyc_graph_get_length (GtkDataboxXYCGraph * xyc_graph);
   gfloat *gtk_databox_xyc_graph_get_X (GtkDataboxXYCGraph * xyc_graph);
   gfloat *gtk_databox_xyc_graph_get_Y (GtkDataboxXYCGraph * xyc_graph);

G_END_DECLS
#endif				/* __GTK_DATABOX_XYC_GRAPH_H__ */
