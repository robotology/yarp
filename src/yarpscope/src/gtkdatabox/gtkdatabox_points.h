/* $Id: gtkdatabox_points.h 4 2008-06-22 09:19:11Z rbock $ */
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
 * SECTION:gtkdatabox_points
 * @short_description: A #GtkDataboxGraph used for displaying xy-values as dots.
 * @include: gtkdatabox_points.h
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxLines, #GtkDataboxBars, #GtkDataboxMarkers
 *
 * #GtkDataboxPoints is a #GtkDataboxGraph class for displaying xy-values as dots.
 *
 */

#ifndef __GTK_DATABOX_POINTS_H__
#define __GTK_DATABOX_POINTS_H__

#include <gtkdatabox_xyc_graph.h>

G_BEGIN_DECLS
#define GTK_DATABOX_TYPE_POINTS		  (gtk_databox_points_get_type ())
#define GTK_DATABOX_POINTS(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                           GTK_DATABOX_TYPE_POINTS, \
                                           GtkDataboxPoints))
#define GTK_DATABOX_POINTS_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                           GTK_DATABOX_TYPE_POINTS, \
                                           GtkDataboxPointsClass))
#define GTK_DATABOX_IS_POINTS(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           GTK_DATABOX_TYPE_POINTS))
#define GTK_DATABOX_IS_POINTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                           GTK_DATABOX_TYPE_POINTS))
#define GTK_DATABOX_POINTS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                           GTK_DATABOX_TYPE_POINTS, \
                                           GtkDataboxPointsClass))

/**
 * GtkDataboxPoints:
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxLines, #GtkDataboxBars, #GtkDataboxMarkers
 *
 * #GtkDataboxPoints is a #GtkDataboxGraph class for displaying xy-values as dots.
 *
 */

   typedef struct _GtkDataboxPoints GtkDataboxPoints;

   typedef struct _GtkDataboxPointsClass GtkDataboxPointsClass;

   /**
    * GtkDataboxPointsPrivate
    *
    * A private data structure used by the #GtkDataboxPoints. It shields all internal things
    * from developers who are just using the object. 
    *
    **/
   typedef struct _GtkDataboxPointsPrivate GtkDataboxPointsPrivate;

   struct _GtkDataboxPoints
   {
      /*< private >*/
      GtkDataboxXYCGraph parent;

      GtkDataboxPointsPrivate *priv;
   };

   struct _GtkDataboxPointsClass
   {
      GtkDataboxXYCGraphClass parent_class;
   };

   GType gtk_databox_points_get_type (void);

   GtkDataboxGraph *gtk_databox_points_new (guint len, gfloat * X, gfloat * Y,
					    GdkColor * color, gint size);

G_END_DECLS
#endif				/* __GTK_DATABOX_POINTS_H__ */
