/* goocanvasmm - a C++ wrapper for libgoocanvas
 *
 * Copyright 1999-2001 Free Software Foundation
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

#ifndef LIBGOOCANVASMM_H
#define LIBGOOCANVASMM_H

/** @mainpage goocanvasmm Reference Manual
 *
 * @section description Description
 *
 * goocanvasmm is a C++ binding for <a href="http://live.gnome.org/GooCanvas">goocanvas</a>.
 * See the Goocanvas::Canvas widget.
 *
 * @section basics Basic Usage
 * Include the goocanvasmm header:
 * @code
 * #include <goocanvasmm.h>
 * @endcode
 * (You may include individual headers, such as goocanvasmm/rect.h instead.)
 *
 * If your source file is @c program.cc, you can compile it with:
 * @code
 * g++ program.cc -o program `pkg-config --cflags --libs goocanvasmm-1.0`
 * @endcode
 *
 * Alternatively, if using autoconf, use the following in @c configure.ac:
 * @code
 * PKG_CHECK_MODULES([GOOCANVASMM], [goocanvasmm-1.0])
 * @endcode
 * Then use the generated @c GOOCANVASMM_CFLAGS and @c GOOCANVASMM_LIBS
 * variables in the project @c Makefile.am files. For example:
 * @code
 * program_CPPFLAGS = $(GOOCANVASMM_CFLAGS)
 * program_LDADD = $(GOOCANVASMM_LIBS)
 * @endcode
 */

/* goocanvasmm version.  */
extern const int goocanvasmm_major_version;
extern const int goocanvasmm_minor_version;
extern const int goocanvasmm_micro_version;

#include <glibmm.h>

#include <goocanvasmm/bounds.h>
#include <goocanvasmm/canvas.h>
#include <goocanvasmm/ellipse.h>
#include <goocanvasmm/ellipsemodel.h>
#include <goocanvasmm/enums.h>
#include <goocanvasmm/grid.h>
#include <goocanvasmm/group.h>
#include <goocanvasmm/groupmodel.h>
#include <goocanvasmm/image.h>
#include <goocanvasmm/imagemodel.h>
#include <goocanvasmm/init.h>
#include <goocanvasmm/item.h>
#include <goocanvasmm/itemmodel.h>
#include <goocanvasmm/itemmodelsimple.h>
#include <goocanvasmm/itemsimple.h>
#include <goocanvasmm/linedash.h>
#include <goocanvasmm/path.h>
#include <goocanvasmm/pathmodel.h>
#include <goocanvasmm/points.h>
#include <goocanvasmm/polyline.h>
#include <goocanvasmm/polylinemodel.h>
#include <goocanvasmm/rect.h>
#include <goocanvasmm/rectmodel.h>
#include <goocanvasmm/style.h>
#include <goocanvasmm/table.h>
#include <goocanvasmm/tablemodel.h>
#include <goocanvasmm/text.h>
#include <goocanvasmm/textmodel.h>
#include <goocanvasmm/types.h>
#include <goocanvasmm/widget.h>

#endif /* #ifndef LIBGOOCANVASMM_H */
