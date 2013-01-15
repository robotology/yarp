// -*- c++ -*-
#ifndef _LIBGOOCANVASMM_INIT_H
#define _LIBGOOCANVASMM_INIT_H
/* $Id: init.h,v 1.4 2003/10/12 09:50:13 murrayc Exp $ */

/* init.h
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

#include <glibmm.h>

namespace Goocanvas
{

//TODO: Remove these paramters.
void init(const Glib::ustring& app_id, const Glib::ustring& version, int nargs, gchar *args[]);

} //namespace Goocanvas

#endif //_LIBGOOCANVASMM_INIT_H

