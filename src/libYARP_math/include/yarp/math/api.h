/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
