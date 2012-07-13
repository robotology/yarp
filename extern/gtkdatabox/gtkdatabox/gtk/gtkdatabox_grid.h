/* $Id: gtkdatabox_grid.h 4 2008-06-22 09:19:11Z rbock $ */
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
 * SECTION:gtkdatabox_grid
 * @short_description: A #GtkDataboxGraph used for displaying a grid (like in an oscilloscope).
 * @include: gtkdatabox_grid.h
 * @see_also: #GtkDatabox, #GtkDataboxGraph, #GtkDataboxPoints, #GtkDataboxLines,  #GtkDataboxBars, #GtkDataboxMarkers
 *
 * #GtkDataboxGrid is a #GtkDataboxGraph class for displaying a grid (like in an oscilloscope). You can determine 
 * the number of horizontal and vertical lines. When you zoom in, the grid is also zoomed along with the data.
 *
 */

#ifndef __GTK_DATABOX_GRID_H__
#define __GTK_DATABOX_GRID_H__

#include <gtkdatabox_graph.h>

G_BEGIN_DECLS
#define GTK_DATABOX_TYPE_GRID		  (gtk_databox_grid_get_type ())
#define GTK_DATABOX_GRID(obj)		  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                           GTK_DATABOX_TYPE_GRID, \
                                           GtkDataboxGrid))
#define GTK_DATABOX_GRID_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                           GTK_DATABOX_TYPE_GRID, \
                                           GtkDataboxGridClass))
#define GTK_DATABOX_IS_GRID(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                           GTK_DATABOX_TYPE_GRID))
#define GTK_DATABOX_IS_GRID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                           GTK_DATABOX_TYPE_GRID))
#define GTK_DATABOX_GRID_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                           GTK_DATABOX_TYPE_GRID, \
                                           GtkDataboxGridClass))

/**
 * GtkDataboxGrid
 *
 * #GtkDataboxGrid is a #GtkDataboxGraph class for displaying a grid (like in an oscilloscope). You can determine 
 * the number of horizontal and vertical lines. When you zoom in, the grid is also zoomed along with the data.
 *
 */

   typedef struct _GtkDataboxGrid GtkDataboxGrid;

   typedef struct _GtkDataboxGridClass GtkDataboxGridClass;

   /**
    * GtkDataboxGridPrivate
    *
    * A private data structure used by the #GtkDataboxGrid. It shields all internal things
    * from developers who are just using the object. 
    *
    **/
   typedef struct _GtkDataboxGridPrivate GtkDataboxGridPrivate;

   struct _GtkDataboxGrid
   {
      GtkDataboxGraph parent;

      GtkDataboxGridPrivate *priv;
   };

   struct _GtkDataboxGridClass
   {
      GtkDataboxGraphClass parent_class;
   };

   GType gtk_databox_grid_get_type (void);

   GtkDataboxGraph *gtk_databox_grid_new (gint hlines, gint vlines,
					  GdkColor * color, guint size);
   GtkDataboxGraph *gtk_databox_grid_array_new (gint hlines, gint vlines, gfloat *hline_vals, gfloat *vline_vals,
					  GdkColor * color, guint size);

   void gtk_databox_grid_set_hlines (GtkDataboxGrid * grid, gint hlines);
   gint gtk_databox_grid_get_hlines (GtkDataboxGrid * grid);

   void gtk_databox_grid_set_vlines (GtkDataboxGrid * grid, gint vlines);
   gint gtk_databox_grid_get_vlines (GtkDataboxGrid * grid);

G_END_DECLS
#endif				/* __GTK_DATABOX_GRID_H__ */
