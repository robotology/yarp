/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MATH_API_H
#define YARP_MATH_API_H

#include <yarp/conf/api.h>
#ifndef YARP_math_API
#  ifdef YARP_math_EXPORTS
#    define YARP_math_API YARP_EXPORT
#    define YARP_math_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_math_API YARP_IMPORT
#    define YARP_math_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_math_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_math_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_MATH_API_H
