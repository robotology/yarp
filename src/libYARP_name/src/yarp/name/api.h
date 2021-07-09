/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_NAME_API_H
#define YARP_NAME_API_H

#include <yarp/conf/api.h>
#ifndef YARP_name_API
#  ifdef YARP_name_EXPORTS
#    define YARP_name_API YARP_EXPORT
#    define YARP_name_EXTERN YARP_EXPORT_EXTERN
#    define YARP_name_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_name_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_name_API YARP_IMPORT
#    define YARP_name_EXTERN YARP_IMPORT_EXTERN
#    define YARP_name_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_name_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_NAME_API_H
