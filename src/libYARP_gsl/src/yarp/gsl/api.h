/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef YARP_GSL_API_H
#define YARP_GSL_API_H

#include <yarp/conf/api.h>
#ifndef YARP_gsl_API
#  ifdef YARP_gsl_EXPORTS
#    define YARP_gsl_API YARP_EXPORT
#    define YARP_gsl_EXTERN YARP_EXPORT_EXTERN
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_gsl_API YARP_IMPORT
#    define YARP_gsl_EXTERN YARP_IMPORT_EXTERN
#    define YARP_gsl_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_gsl_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_GSL_API_H
