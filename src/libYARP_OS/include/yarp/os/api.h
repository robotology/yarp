// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
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

#ifndef YARP_OS_impl_API
// FIXME YARP should not export symbols from os/impl.
//       Unfortunately at the moment, methods from impl are used in other
//       internal libraries, forcing us to export them in the library
//#  ifdef YARP_FILTER_API
//#    define YARP_OS_impl_API
//#  else
#    define YARP_OS_impl_API YARP_OS_API
//#  endif
#endif

#endif
