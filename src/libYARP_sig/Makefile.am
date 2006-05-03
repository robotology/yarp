
SUBDIRS = include src harness

includedir = @includedir@/yarp/sig/
pkgconfigdir = @libdir@/pkgconfig

pkgconfig_DATA = libYARP_sig.pc

libYARP_sig.pc: ${top_builddir}/config.status ${srcdir}/libYARP_sig.pc.in
	${top_builddir}/config.status --file $@:${srcdir}/libYARP_sig.pc.in

EXTRA_DIST = \
  libYARP_sig.pc.in

DISTCLEANFILES = libYARP_sig.pc config.log

