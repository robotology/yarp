/* $Id: gladeui-databox.c 4 2008-06-22 09:19:11Z rbock $ */
/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2007 H. Nieuwenhuis
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
 *
 * Authors:
 *      H. Nieuwenhuis <vzzbx@xs4all.nl>
 */

#include <config.h>


#include <gladeui/glade.h>
#include <gladeui/glade-editor-property.h>
/* #include <gladeui/glade-base-editor.h> */


#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include <gtkdatabox.h>
#include <gtkdatabox_ruler.h>


static void
databox_widget_parent_changed (GtkWidget * widget,
			       GParamSpec * pspec,
			       GladeWidgetAdaptor * adaptor)
{
}

void
glade_gtk_databox_ruler_deep_post_create (GladeWidgetAdaptor * adaptor,
					  GObject * widget,
					  GladeCreateReason reason)
{
   GladeWidget *gwidget = glade_widget_get_from_gobject (widget);


   glade_widget_set_action_sensitive (gwidget, "remove_parent", FALSE);

   if (GTK_IS_WINDOW (widget))
      glade_widget_set_action_sensitive (gwidget, "add_parent", FALSE);

   /* Watch parents and set actions sensitive/insensitive */
   g_signal_connect (G_OBJECT (widget), "notify::parent",
		     G_CALLBACK (databox_widget_parent_changed), adaptor);
}
