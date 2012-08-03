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

include $(srcdir)/m4/filelist.am

dist_noinst_DATA = $(addprefix m4/,$(files_codegen_m4))
noinst_PROGRAMS  = extradefs/generate_extra_defs

extradefs_generate_extra_defs_SOURCES = extradefs/generate_extra_defs_gtkdatabox.cc
extradefs_generate_extra_defs_LDADD   = $(GTKDATABOXMM_LIBS) -lglibmm_generate_extra_defs-2.4

AM_CPPFLAGS = -I$(top_builddir) $(GTHREAD_CFLAGS) $(GTKDATABOXMM_CFLAGS)
AM_CXXFLAGS = $(GTKDATABOXMM_WXXFLAGS)

# Instruct GNU make to delete the targets of a rule after it failed, in
# order to avoid the complication of handling that situation manually.
.DELETE_ON_ERROR:
