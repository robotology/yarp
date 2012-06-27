/* Copyright (c) 2012  Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * This file is part of gtkdataboxmm.
 *
 * gtkdataboxmm is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * gtkdataboxmm is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GTKDATABOXMM_H_INCLUDED
#define GTKDATABOXMM_H_INCLUDED

/** @mainpage gtkdataboxmm Reference Manual
 *
 * @section description Description
 *
 * The gtkdataboxmm C++ binding provides a C++ interface on top of the gtkdatabox
 * C library.
 *
 * @section overview Overview
 *
 * [...]
 *
 * @section use Use
 *
 * To use gtkdataboxmm in your C++ application, include the central header file
 * <tt>\<gtkdataboxmm.h\></tt>.  The gtkdataboxmm package ships a @c pkg-config
 * file with the correct include path and link command-line for the compiler.
 */

#include <gtkdataboxmmconfig.h>
#include <gtkdataboxmm/databox.h>
#include <gtkdataboxmm/bars.h>
#include <gtkdataboxmm/cross_simple.h>
#include <gtkdataboxmm/graph.h>
#include <gtkdataboxmm/lines.h>
#include <gtkdataboxmm/points.h>
#include <gtkdataboxmm/scale.h>
#include <gtkdataboxmm/grid.h>
#include <gtkdataboxmm/markers.h>
#include <gtkdataboxmm/ruler.h>
#include <gtkdataboxmm/xyc_graph.h>
#include <gtkdataboxmm/init.h>

/** @example example/example.cc
 * A gtkdataboxmm example program.
 */

#endif /* !GTKDATABOXMM_H_INCLUDED */
