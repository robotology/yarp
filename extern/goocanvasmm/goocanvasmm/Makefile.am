## Copyright (c) 2009  Joe Hacker <joe@example.com>
##
## This file is part of goocanvasmm.
##
## goocanvasmm is free software: you can redistribute it and/or modify it
## under the terms of the GNU Lesser General Public License as published
## by the Free Software Foundation, either version 2.1 of the License,
## or (at your option) any later version.
##
## goocanvasmm is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
## See the GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS = -I build ${ACLOCAL_FLAGS}
DISTCHECK_CONFIGURE_FLAGS = --enable-warnings=fatal

if MAINTAINER_MODE
src_subdirs = codegen goocanvas/src
else
src_subdirs =
endif
if ENABLE_DOCUMENTATION
doc_subdirs = doc
else
doc_subdirs =
endif
SUBDIRS = $(src_subdirs) goocanvas/goocanvasmm $(doc_subdirs)

goocanvasmm_includedir = $(includedir)/$(GOOCANVASMM_MODULE_NAME)
goocanvasmm_include_HEADERS = goocanvas/goocanvasmm.h
goocanvasmm_libincludedir = $(libdir)/$(GOOCANVASMM_MODULE_NAME)/include
nodist_goocanvasmm_libinclude_HEADERS = goocanvas/goocanvasmmconfig.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = goocanvas/$(GOOCANVASMM_MODULE_NAME).pc

dist_noinst_SCRIPTS = autogen.sh

# I do not want some crappy generated ChangeLog based on random vague commit 
# messages that can't be corrected later. I fight entropy. murrayc.
#include $(top_srcdir)/build/dist-changelog.am
