/*
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#  ifdef YARP_NO_DEPRECATED
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X)
#  else
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED YARP_gsl_API
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X) YARP_gsl_API
#  endif
#endif

#endif // YARP_GSL_API_H
