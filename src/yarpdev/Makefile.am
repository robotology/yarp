
bin_PROGRAMS = yarpdev

man_MANS = yarpdev.1

yarpdev_SOURCES = yarpdev.cpp

yarpdev_DEPENDENCIES = $(top_builddir)/src/libYARP_dev/src/libYARP_dev.la $(top_builddir)/src/libYARP_sig/src/libYARP_sig.la $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

yarpdev_LDADD = $(yarpdev_DEPENDENCIES)

INCLUDES = -I$(srcdir)/../libYARP_OS/include -I$(srcdir)/../libYARP_sig/include -I$(srcdir)/../libYARP_dev/include

