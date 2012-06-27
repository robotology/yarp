/* $Id: gtkdatabox_cross_simple.h 4 2008-06-22 09:19:11Z rbock $ */
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
 * SECTION:gtkdatabox_cross_simple
 * @short_description: A #GtkDataboxGraph used for displaying a simple coordinate cross.
 * @include: gtkdatabox_cross_simple.h
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxPoints, #GtkDataboxLines,  #GtkDataboxBars, #GtkDataboxMarkers
 *
 * #GtkDataboxCrossSimple is a #GtkDataboxGraph class for displaying a simple coordinate cross (a vertical line 
 * at x=0 and a horizontal line at y=0). 
 *
 * A more sophisticated coordinate cross would be welcome :-)
 *
 */

#ifndef __GTK_DATABOX_CROSS_SIMPLE_H__
#define __GTK_DATABOX_CROSS_SIMPLE_H__

#include <gtkdatabox_markers.h>

G_BEGIN_DECLS
#define GTK_DATABOX_TYPE_CROSS_SIMPLE		  (gtk_databox_cross_simple_get_type ())
#define GTK_DATABOX_CROSS_SIMPLE(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                           GTK_DATABOX_TYPE_CROSS_SIMPLE, \
                                           GtkDataboxCrossSimple))
#define GTK_DATABOX_CROSS_SIMPLE_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                           GTK_DATABOX_TYPE_CROSS_SIMPLE, \
                                           GtkDataboxCrossSimpleClass))
#define GTK_DATABOX_IS_CROSS_SIMPLE(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           GTK_DATABOX_TYPE_CROSS_SIMPLE))
#define GTK_DATABOX_IS_CROSS_SIMPLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                           GTK_DATABOX_TYPE_CROSS_SIMPLE))
#define GTK_DATABOX_CROSS_SIMPLE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                           GTK_DATABOX_TYPE_CROSS_SIMPLE, \
                                           GtkDataboxCrossSimpleClass))

/**
 * GtkDataboxCrossSimple:
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxPoints, #GtkDataboxLines,  #GtkDataboxBars, #GtkDataboxMarkers
 *
 * #GtkDataboxCrossSimple is a #GtkDataboxGraph class for displaying a simple coordinate cross (a vertical line 
 * at x=0 and a horizontal line at y=0). 
 *
 * A more sophisticated coordinate cross would be welcome :-)
 *
 */

   typedef struct _GtkDataboxCrossSimple GtkDataboxCrossSimple;

   typedef struct _GtkDataboxCrossSimpleClass GtkDataboxCrossSimpleClass;

   /**
    * GtkDataboxCrossSimplePrivate
    *
    * A private data structure used by the #GtkDataboxCrossSimple. It shields all internal things
    * from developers who are just using the object. 
    *
    **/
   typedef struct _GtkDataboxCrossSimplePrivate GtkDataboxCrossSimplePrivate;

   struct _GtkDataboxCrossSimple
   {
      /*< private >*/
      GtkDataboxMarkers parent;

      GtkDataboxCrossSimplePrivate *priv;
   };

   struct _GtkDataboxCrossSimpleClass
   {
      GtkDataboxMarkersClass parent_class;
   };

   GType gtk_databox_cross_simple_get_type (void);

   GtkDataboxGraph *gtk_databox_cross_simple_new (GdkColor * color,
						  guint size);

G_END_DECLS
#endif				/* __GTK_DATABOX_CROSS_SIMPLE_H__ */
