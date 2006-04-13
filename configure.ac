#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.5)
AC_INIT(yarp, 2.0, [paulfitz@alum.mit.edu])
AC_CONFIG_SRCDIR([src/libYARP_OS/src/Address.cpp])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([src/libYARP_OS/include/yarp/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lYARP_OS':
#AC_CHECK_LIB([YARP_OS], [main])
# FIXME: Replace `main' with a function in `-lYARP_OS2':
#AC_CHECK_LIB([YARP_OS2], [main])
# FIXME: Replace `main' with a function in `-lYARP_OS_compat':
#AC_CHECK_LIB([YARP_OS_compat], [main])
# FIXME: Replace `main' with a function in `-lYARP_sig':
#AC_CHECK_LIB([YARP_sig], [main])
# FIXME: Replace `main' with a function in `-lYARP_sig_compat':
#AC_CHECK_LIB([YARP_sig_compat], [main])

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

AC_CHECK_TYPE([uint32_t],
	      [AC_DEFINE([YARP_HAS_UINT32_T], 1,
			 [Define to 1 if the system has the type `uint32_t'.])],
	      [],
	      [
#ifndef YARP_LACKS_STDINT_H
#include <stdint.h>
#endif
#ifndef YARP_LACKS_INTTYPES_H
#include <inttypes.h>
#endif])


# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memset mkdir sqrt strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libYARP_OS/Makefile
                 src/libYARP_OS/src/Makefile
                 src/libYARP_OS/yarp/Makefile
                 src/libYARP_OS/harness/Makefile
                 src/libYARP_sig/Makefile
                 src/libYARP_sig/src/Makefile
                 src/libYARP_sig/harness/Makefile
                 src/libYARP_OS_compat/Makefile
                 src/libYARP_OS_compat/src/Makefile
                 src/libYARP_OS_compat/harness/Makefile])
AC_OUTPUT
