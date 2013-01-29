## Copyright (c) 2009  goocanvasmm Development Team
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

AC_INIT([goocanvasmm], [0.15.4], [http://bugzilla.gnome.org/], [goocanvasmm], [http://www.gtkmm.org/])
AC_PREREQ([2.59])

AC_CONFIG_SRCDIR([goocanvas/src/canvas.hg])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_HEADERS([build/config.h goocanvas/goocanvasmmconfig.h])

AM_INIT_AUTOMAKE([1.9 -Wno-portability check-news dist-bzip2 no-define nostdinc tar-pax])
AM_MAINTAINER_MODE
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

MM_PREREQ([0.7.2])
MM_INIT_MODULE([goocanvasmm-1.0])

# http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
AC_SUBST([GOOCANVASMM_SO_VERSION], [5:0:0])

AC_PROG_CXX
AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

AC_SUBST([GOOCANVASMM_MODULES], ['glibmm-2.4 >= 2.14.2 gtkmm-2.4 >= 2.22.0 goocanvas >= 0.15'])
PKG_CHECK_MODULES([GOOCANVASMM], [$GOOCANVASMM_MODULES])

MM_PKG_CONFIG_SUBST([GTHREAD_CFLAGS], [--cflags-only-other gthread-2.0])
MM_PKG_CONFIG_SUBST([GMMPROC_DIR], [--variable=gmmprocdir glibmm-2.4])

MM_ARG_ENABLE_DOCUMENTATION
MM_ARG_WITH_TAGFILE_DOC([libstdc++.tag], [mm-common-libstdc++])
MM_ARG_WITH_TAGFILE_DOC([libsigc++-2.0.tag], [sigc++-2.0])
MM_ARG_WITH_TAGFILE_DOC([glibmm-2.4.tag], [glibmm-2.4])
MM_ARG_WITH_TAGFILE_DOC([cairomm-1.0.tag], [cairomm-1.0])
MM_ARG_WITH_TAGFILE_DOC([pangomm-1.4.tag], [pangomm-1.4])
MM_ARG_WITH_TAGFILE_DOC([gtkmm-2.4.tag], [gtkmm-2.4])

AC_LANG([C++])
MM_ARG_ENABLE_WARNINGS([GOOCANVASMM_WXXFLAGS],
                       [-Wall],
                       [-pedantic -Wall -Wextra],
                       [G ATK PANGO GDK GDK_PIXBUF GTK])

AC_CONFIG_FILES([Makefile
                 codegen/Makefile
                 goocanvas/${GOOCANVASMM_MODULE_NAME}.pc:goocanvas/goocanvasmm.pc.in
                 goocanvas/${GOOCANVASMM_MODULE_NAME}-uninstalled.pc:goocanvas/goocanvasmm-uninstalled.pc.in
                 goocanvas/src/Makefile
                 goocanvas/goocanvasmm/Makefile
                 doc/Makefile
                 doc/reference/Doxyfile])
AC_OUTPUT
