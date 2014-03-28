# $Id: configure.ac 4 2008-06-22 09:19:11Z rbock $
#
# This configure.in file has been started by looking at the tutorial of
# Ralph Levien
# at http://www.levien.com/gnome/cvs-lib-howto.html
#
# Additional features where taken from gtk+-1.1.1/configure.in and allied 
# files.
# 
# Thank's to all the gimp-glib-gtk-gnome-etc.-people
#
# If you know automake and friends better then me:
# Please send any helpful comments to rbock at eudoxos de
#
# Initialize autoconf by any header file of the source
#


m4_define([databox_major_version], [0]) # Change in case of really major changes
m4_define([databox_minor_version], [9]) # Change in case of changes in exisiting API
m4_define([databox_micro_version], [1]) # Change in case of new functions being added
m4_define([databox_nano_version],  [1]) # Change for bug fixes, code cleanup, etc.
m4_define([databox_version],       [databox_major_version.databox_minor_version.databox_micro_version.databox_nano_version])

AC_INIT(gtkdatabox, databox_version)

# This configures autoheader to generate a config.h file
#
AM_CONFIG_HEADER(config.h)

AC_SUBST(LT_RELEASE, [databox_major_version.databox_minor_version.databox_micro_version])
AC_SUBST(LT_CURRENT, [databox_nano_version])
AC_SUBST(DATABOX_VERSION, [databox_version])

AM_INIT_AUTOMAKE(gtkdatabox, databox_version)

AM_PROG_LIBTOOL

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

m4_define([gtk_required_version], [2.8.0])
m4_define([pango_required_version], [1.16.0])
m4_define([cairo_required_version], [1.4.0])

AM_PATH_GTK_2_0([gtk_required_version])
PKG_CHECK_MODULES(PANGO, pango >= [pango_required_version])
PKG_CHECK_MODULES(CAIRO, cairo >= [cairo_required_version]) 
AC_SUBST(GTK_REQUIRED, [gtk_required_version])
AC_SUBST(PANGO_REQUIRED, [pango_required_version])
AC_SUBST(CAIRO_REQUIRED, [cairo_required_version])

# ------------------ Libglade configuration ------------------------
AC_ARG_ENABLE(libglade,
	[  --enable-libglade       enable libglade support],
	[ac_cv_enable_libglade=$enableval], [ac_cv_enable_libglade=no])
AC_MSG_CHECKING([whether to build libglade support module])
if test x"$ac_cv_enable_libglade" = xyes; then
	AC_MSG_RESULT(yes)
	PKG_CHECK_MODULES(LIBGLADE, libglade-2.0 >= 2.0.0,
			ac_cv_enable_libglade=yes, ac_cv_enable_libglade=no)
	if test x"$ac_cv_enable_libglade" = xyes; then
		AC_DEFINE(USE_LIBGLADE, 1, Define if you want libglade support)
		libglade_moduledir=`pkg-config libglade-2.0 --variable=moduledir`
		AC_DEFINE_UNQUOTED(LIBGLADE_MODULEDIR, [$libglade_moduledir], [Libglade module directory])
	else
		AC_MSG_RESULT(not found)
	fi

else
	AC_MSG_RESULT(no)
fi
AC_SUBST(LIBGLADE_CFLAGS)
AC_SUBST(LIBGLADE_LIBS)
AC_SUBST(libglade_moduledir)

AM_CONDITIONAL(BUILD_LIBGLADE, test x"$ac_cv_enable_libglade" = xyes)

# ------------------ Glade3 configuration ------------------------

AC_ARG_ENABLE(glade,
	[  --enable-glade	  enable glade-3 support],
	[ac_cv_enable_glade=$enableval], [ac_cv_enable_glade=no])
AC_MSG_CHECKING([whether to build glade-3 support module])
if test x"$ac_cv_enable_glade" = xyes; then
	AC_MSG_RESULT(yes)
	PKG_CHECK_MODULES(GLADE3, gladeui-1.0 >= 3.4.0,
			ac_cv_enable_glade=yes, ac_cv_enable_glade=no)
	if test x"$ac_cv_enable_glade" = xyes; then
		AC_DEFINE(USE_GLADE, 1, Define if you want glade interface builder support)
		glade_moduledir=`pkg-config gladeui-1.0 --variable=moduledir`
		AC_DEFINE_UNQUOTED(GLADE_MODULEDIR, [$glade_moduledir], [Glade-3 module directory])
		glade_catalogdir=`pkg-config gladeui-1.0 --variable=catalogdir`
		AC_DEFINE_UNQUOTED(GLADE_CATALOGDIR, [$glade_catalogdir], [Glade-3 catalog directory])
	else
		AC_MSG_RESULT(not found)
	fi
else
	AC_MSG_RESULT(no)
fi

AC_SUBST(GLADE3_CFLAGS)
AC_SUBST(GLADE3_LIBS)
AC_SUBST(glade_moduledir)
AC_SUBST(glade_catalogdir)

AM_CONDITIONAL(BUILD_GLADE, test x"$ac_cv_enable_glade" = xyes)

# ------------------ End of Glade 3 configuration ------------------------

GTK_DOC_CHECK([1.4])

AC_OUTPUT(Makefile              \
	gtk/Makefile            \
	glade/Makefile		\
	examples/Makefile       \
	docs/Makefile       \
	docs/reference/Makefile       \
        gtkdatabox.pc           \
        gtkdatabox.spec         \
	)

AC_MSG_RESULT([

	Install libglade-2.0 module:	${ac_cv_enable_libglade}
	Install glade-3 module:         ${ac_cv_enable_glade}
])

