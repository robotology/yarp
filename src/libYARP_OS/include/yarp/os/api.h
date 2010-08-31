// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_OS_API_
#define _YARP2_OS_API_

#include <yarp/conf/api.h>
#ifndef YARP_OS_API
#  ifdef YARP_OS_EXPORTS
#    define YARP_OS_API YARP_EXPORT
#  else
#     define YARP_OS_API YARP_IMPORT
#  endif
#endif

#ifndef YARP_init_API
#  ifdef YARP_init_EXPORTS
#    define YARP_init_API YARP_EXPORT
#  else
#     define YARP_init_API YARP_IMPORT
#  endif
#endif

#endif
