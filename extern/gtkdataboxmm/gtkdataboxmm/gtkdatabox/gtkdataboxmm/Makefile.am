## Copyright (c) 2012  iCub Facility, Istituto Italiano di Tecnologia
##   @author Daniele E. Domenichelli <daniele.domenichelli@iit.it>
##
## This file is part of gtkdataboxmm.
##
## gtkdataboxmm is free software: you can redistribute it and/or modify it
## under the terms of the GNU Lesser General Public License as published
## by the Free Software Foundation, either version 2.1 of the License,
## or (at your option) any later version.
##
## gtkdataboxmm is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

lib_LTLIBRARIES = libgtkdataboxmm-@GTKDATABOXMM_API_VERSION@.la

include $(srcdir)/../src/filelist.am
include $(srcdir)/filelist.am
include $(top_srcdir)/build/compile-binding.am

local_cppflags = $(binding_includes) $(binding_cppflags) -DGTKDATABOXMM_BUILD=1

AM_CPPFLAGS = $(local_cppflags) $(GTHREAD_CFLAGS) $(GTKDATABOXMM_CFLAGS)
AM_CXXFLAGS = $(GTKDATABOXMM_WXXFLAGS)

libgtkdataboxmm_@GTKDATABOXMM_API_VERSION@_la_SOURCES = $(binding_sources)
libgtkdataboxmm_@GTKDATABOXMM_API_VERSION@_la_LDFLAGS = -no-undefined -version-info $(GTKDATABOXMM_SO_VERSION)
libgtkdataboxmm_@GTKDATABOXMM_API_VERSION@_la_LIBADD  = $(GTKDATABOXMM_LIBS)
