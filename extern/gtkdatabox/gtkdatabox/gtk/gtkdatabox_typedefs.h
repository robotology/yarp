/* $Id: gtkdatabox_typedefs.h 4 2008-06-22 09:19:11Z rbock $ */
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

#ifndef __GTK_DATABOX_TYPEDEFS_H__
#define __GTK_DATABOX_TYPEDEFS_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS
   /**
    * GtkDatabox:
    * @box: The parent object 
    * @priv: A private structure containing internal data.
    *
    * A GTK+ widget to display large amounts of numerical data quickly and easily. 
    * The numerical data is represented/displayed by #GtkDataboxGraph objects, e.g.
    * #GtkDataboxPoints.
    *
    * Implemented by #_GtkDatabox;
    *
    **/
   typedef struct _GtkDatabox GtkDatabox;

  /**
    * GtkDataboxGraph
    * @parent: The parent object 
    * @priv: A private structure containing internal data.
    *
    * The GtkDataboxGraph is the base class for all kinds of graphs (e.g. lines, points, decorations like coordinate crosses) 
    * to be shown in a #GtkDatabox wiget.
    *
    * Implemented by #_GtkDataboxGraph
    *
    **/
typedef struct _GtkDataboxGraph GtkDataboxGraph;
G_END_DECLS
#endif /* __GTK_DATABOX_TYPEDEFS_H__ */


