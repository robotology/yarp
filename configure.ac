#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.5)
AC_INIT(yarp, 2.1.7, [paulfitz@alum.mit.edu])
AC_CONFIG_SRCDIR([src/libYARP_OS/src/Address.cpp])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([src/libYARP_OS/include/yarp/conf/yarp-config.h])

# we need to be reentrant
CPPFLAGS="$CPPFLAGS -D_REENTRANT"

# we need to be avoid a few CMAKE-isms in PopulateDrivers.cpp
CPPFLAGS="$CPPFLAGS -DYARP_AUTOCONF"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

AC_C_BIGENDIAN

AC_CHECK_SIZEOF(short int)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long int)

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])


#AC_CHECK_TYPE([int32_t],
#	      [AC_DEFINE([YARP_HAS_INT32_T], 1,
#			 [Define to 1 if the system has the type `int32_t'.])],
#	      [],
#	      [
##ifndef YARP_LACKS_STDINT_H
##include <stdint.h>
##endif
##ifndef YARP_LACKS_INTTYPES_H
##include <inttypes.h>
##endif])


AC_ARG_ENABLE(optimize, [  --disable-optimize      Compile without optimization (O2) ],
    if test "x$enableval" = "xyes"; then
        CXXFLAGS="$CXXFLAGS -O2"
        CFLAGS="$CFLAGS -O2"
    elif test "x$enableval" = "xno"; then
        CXXFLAGS=`echo $CXXFLAGS | sed -e s/-O.//g`
        CFLAGS=`echo $CFLAGS | sed -e s/-O.//g`
    else
        CXXFLAGS="$CXXFLAGS -$enableval"
        CFLAGS="$CFLAGS -$enableval"
    fi,
)

AC_ARG_ENABLE(debug,    [  --disable-debug         Compile without debugging information (-g)],
    if test "x$enableval" = "xyes"; then
        CXXFLAGS="$CXXFLAGS -g"
        CFLAGS="$CFLAGS -g"
    elif test "x$enableval" = "xno"; then
        CXXFLAGS=`echo $CXXFLAGS | sed -e s/-g//g`
        CFLAGS=`echo $CFLAGS | sed -e s/-g//g`
    fi,
)

AC_ARG_ENABLE(debug-level,
 [  --enable-debug-level[=[type]]
                      none   : no debugging code compiled
                      low    : some debugging code
                      medium : more debugging code
                      high   : verbose logging to console and system log],
 [ if   test "x$enableval" = "xnone" ; then
      AC_DEFINE(DEBUG_LEVEL, NONE, [no debugging code compiled])
   elif test "x$enableval" = "xlow" ; then
      AC_DEFINE(DEBUG_LEVEL, LOW, [some outputs])
   elif test "x$enableval" = "xmedium" ; then
      AC_DEFINE(DEBUG_LEVEL, MEDIUM, [more outputs])
   elif test "x$enableval" = "xhigh" ; then
      AC_DEFINE(DEBUG_LEVEL, HIGH, [verbose logging to console and system log])
   else
      echo
      echo "Error!"
      echo "Unknown debug level"
      exit -1
   fi
 ])


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memset mkdir sqrt strstr])

AC_CONFIG_FILES([src/libYARP_dev/generated_src/PopulateDrivers.cpp:src/libYARP_dev/src/PopulateDrivers.cpp.in])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libYARP_OS/Makefile
                 src/libYARP_OS/include/Makefile
                 src/libYARP_OS/include/yarp/Makefile
                 src/libYARP_OS/include/yarp/os/Makefile
                 src/libYARP_OS/src/Makefile
                 src/libYARP_OS/yarp/Makefile
                 src/libYARP_OS/harness/Makefile
                 src/libYARP_sig/Makefile
                 src/libYARP_sig/include/Makefile
                 src/libYARP_sig/include/yarp/Makefile
                 src/libYARP_sig/include/yarp/sig/Makefile
                 src/libYARP_sig/src/Makefile
                 src/libYARP_sig/harness/Makefile
                 src/libYARP_dev/Makefile
                 src/libYARP_dev/include/Makefile
                 src/libYARP_dev/include/yarp/Makefile
                 src/libYARP_dev/include/yarp/dev/Makefile
                 src/libYARP_dev/src/Makefile
                 src/libYARP_dev/harness/Makefile
                 src/yarpdev/Makefile
		 ])

AC_OUTPUT

