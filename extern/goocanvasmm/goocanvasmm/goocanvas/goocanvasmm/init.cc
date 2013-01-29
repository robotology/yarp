// -*- c++ -*-
/* $Id: init.cc,v 1.4 2004/03/29 17:59:17 murrayc Exp $ */

/* init.cc
 *
 * Copyright 2001      Free Software Foundation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <glibmm/init.h>
#include <glibmm/ustring.h>
#include <goocanvasmmconfig.h> //For LIBGOOCANVASMM_VERSION
#include <goocanvasmm/wrap_init.h>

namespace Goocanvas
{

void init(const Glib::ustring& /* app_id */, const Glib::ustring& /* version */, int /* nargs */, gchar * /* args */[])
{
  Glib::init(); //Sets up the g type system and the Glib::wrap() table.
  wrap_init(); //Tells the Glib::wrap() table about the libgnomecanvasmm classes.
}

} //namespace Goocanvas
