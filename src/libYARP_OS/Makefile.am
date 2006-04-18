SUBDIRS = include src yarp harness

includedir = @includedir@/yarp
pkgconfigdir = @libdir@/pkgconfig

pkgconfig_DATA = libYARP_OS.pc

libYARP_OS.pc: ${top_builddir}/config.status ${srcdir}/libYARP_OS.pc.in
	${top_builddir}/config.status --file $@:${srcdir}/libYARP_OS.pc.in

EXTRA_DIST = \
  libYARP_OS.pc.in

