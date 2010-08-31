// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_LOG_
#define _YARP2_LOG_

#include <stdio.h>   // for printf
#include <stdlib.h>  // for exit

#include <yarp/os/api.h>

/**
 *
 * Low level function for printing a stack trace, if implemented (gcc/Linux).
 *
 */
YARP_OS_API void yarp_print_trace(FILE *out, const char *file, int line);

/**
 *
 * Very basic assertion macro.
 *
 */
#define YARP_ASSERT(x) if (!(x)) { printf("Assertion failure %s:%d  !(%s)\n",__FILE__,__LINE__, #x ); yarp_print_trace(stdout,__FILE__,__LINE__); exit(1); }

YARP_OS_API void __yarp_error(const char *str);
YARP_OS_API void __yarp_warn(const char *str);
YARP_OS_API void __yarp_info(const char *str);
YARP_OS_API void __yarp_debug(const char *str);

#define YARP_LOG_ERROR(x) __yarp_error(x)
#define YARP_LOG_WARN(x) __yarp_warn(x)
#define YARP_LOG_INFO(x) __yarp_info(x)
#define YARP_LOG_DEBUG(x) __yarp_debug(x)

#endif
