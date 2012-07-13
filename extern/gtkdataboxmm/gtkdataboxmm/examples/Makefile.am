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

AUTOMAKE_OPTIONS = subdir-objects

check_PROGRAMS = example/example

local_includes = -I$(top_builddir)/gtkdatabox $(if $(srcdir:.=),-I$(top_srcdir)/gtkdatabox)
local_libs = $(top_builddir)/gtkdatabox/gtkdataboxmm/libgtkdataboxmm-$(GTKDATABOXMM_API_VERSION).la

AM_CPPFLAGS = -I$(top_builddir) $(local_includes) $(GTHREAD_CFLAGS) $(GTKDATABOXMM_CFLAGS)
AM_CXXFLAGS = $(GTKDATABOXMM_WXXFLAGS)
LDADD = $(GTKDATABOXMM_LIBS) $(local_libs)

example_example_SOURCES = example/example.cc
