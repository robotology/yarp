/*
 * Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GPLv2 or later
 *
 */

#ifndef YARP_GSL_API_H
#define YARP_GSL_API_H

#include <yarp/conf/api.h>
#ifndef YARP_gsl_API
#  ifdef YARP_gsl_EXPORTS
#    define YARP_gsl_API YARP_EXPORT
#    define YARP_gsl_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_gsl_API YARP_IMPORT
#    define YARP_gsl_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_gsl_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_GSL_API_H
