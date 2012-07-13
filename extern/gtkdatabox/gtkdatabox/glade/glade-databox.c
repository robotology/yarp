/* $Id: glade-databox.c 4 2008-06-22 09:19:11Z rbock $ */
/* -*- Mode: C; c-basic-offset: 4 -*-
 * libglade - a library for building interfaces from XML files at runtime
 * Copyright (C) 1998-2001  James Henstridge <james@daa.com.au>
 * Copyright 2001 Ximian, Inc.
 *
 * glade-databox.c: support for canvas widgets in libglade.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * Authors:
 *    Jacob Berkman <jacob@ximian.com>
 *    James Henstridge <james@daa.com.au>
 *
 * Modified for gtkdatabox by (based on gnome-canvas glade interface): 
 *    H. Nieuwenhuis <vzzbx@xs4all.nl>
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <glade/glade-init.h>
#include <glade/glade-build.h>
#include <gtkdatabox.h>
#include <gtkdatabox_ruler.h>

/* this macro puts a version check function into the module */
GLADE_MODULE_CHECK_INIT void
glade_module_register_widgets (void)
{
   glade_require ("gtk");

   glade_register_widget (GTK_TYPE_DATABOX, glade_standard_build_widget, NULL,
			  NULL);
   glade_register_widget (GTK_DATABOX_TYPE_RULER, glade_standard_build_widget,
			  NULL, NULL);
   glade_provide ("databox");
}
