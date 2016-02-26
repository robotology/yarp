/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#  ifdef YARP_NO_DEPRECATED
#    define YARP_math_DEPRECATED_API YARP_DEPRECATED
#  else
#    define YARP_math_DEPRECATED_API YARP_DEPRECATED YARP_math_API
#  endif
#endif

#endif // YARP_MATH_API_H
