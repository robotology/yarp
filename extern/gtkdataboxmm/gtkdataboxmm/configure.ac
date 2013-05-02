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

AC_INIT([gtkdataboxmm], [0.9.4], [https://sourceforge.net/p/gtkdataboxmm/tickets/],
        [gtkdataboxmm], [https/sourceforge.net/projects/gtkdataboxmm/])
AC_PREREQ([2.59])

AC_CONFIG_SRCDIR([gtkdatabox/gtkdataboxmm.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_HEADERS([build/config.h gtkdatabox/gtkdataboxmmconfig.h])

AM_INIT_AUTOMAKE([1.9 -Wno-portability check-news dist-bzip2 no-define nostdinc tar-pax])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AM_MAINTAINER_MODE
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

MM_PREREQ([0.9])
MM_INIT_MODULE([gtkdataboxmm-0.9])

# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_SUBST([GTKDATABOXMM_SO_VERSION], [0:2:0])

AC_PROG_CXX
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_SUBST([GTKDATABOXMM_MODULES], ['gtkdatabox >= 0.9 glibmm-2.4 >= 2.16 gtkmm-2.4 > 2.24'])
PKG_CHECK_MODULES([GTKDATABOXMM], [$GTKDATABOXMM_MODULES])

MM_PKG_CONFIG_SUBST([GTHREAD_CFLAGS], [--cflags-only-other gthread-2.0])
MM_PKG_CONFIG_SUBST([GMMPROC_DIR], [--variable=gmmprocdir glibmm-2.4])

MM_ARG_ENABLE_DOCUMENTATION
MM_ARG_WITH_TAGFILE_DOC([libstdc++.tag], [mm-common-libstdc++])
MM_ARG_WITH_TAGFILE_DOC([libsigc++-2.0.tag], [sigc++-2.0])
MM_ARG_WITH_TAGFILE_DOC([glibmm-2.4.tag], [glibmm-2.4])
MM_ARG_WITH_TAGFILE_DOC([gtkmm-2.4.tag], [gtkmm-2.4])

AC_LANG([C++])
MM_ARG_ENABLE_WARNINGS([GTKDATABOXMM_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra],
                       [G GTKDATABOX])

AC_CONFIG_FILES([Makefile
                 codegen/Makefile
                 gtkdatabox/${GTKDATABOXMM_MODULE_NAME}.pc:gtkdatabox/gtkdataboxmm.pc.in
                 gtkdatabox/${GTKDATABOXMM_MODULE_NAME}-uninstalled.pc:gtkdatabox/gtkdataboxmm-uninstalled.pc.in
                 gtkdatabox/src/Makefile
                 gtkdatabox/gtkdataboxmm/Makefile
                 examples/Makefile
                 doc/Makefile
                 doc/reference/Doxyfile])
AC_OUTPUT
