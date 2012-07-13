/* $Id: gtkdatabox_scale.c 4 2008-06-22 09:19:11Z rbock $ */
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

#include <gtkdatabox_scale.h>

GType
gtk_databox_scale_type_get_type (void)
{
   static GType type = 0;

   if (type == 0)
   {
      static const GEnumValue values[] = {
         { GTK_DATABOX_SCALE_LINEAR, "GTK_DATABOX_SCALE_LINEAR", "linear" },
         { GTK_DATABOX_SCALE_LOG, "GTK_DATABOX_SCALE_LOG", "log" },
         { GTK_DATABOX_SCALE_LOG2, "GTK_DATABOX_SCALE_LOG2", "log2" },
         { 0, NULL, NULL }};
      type = g_enum_register_static (g_intern_static_string ("GtkDataboxScaleType"), values);
   }

   return type;
}


