/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SERVERSQL_API_H
#define YARP_SERVERSQL_API_H

#include <yarp/conf/api.h>
#ifndef YARP_serversql_API
#  ifdef YARP_serversql_EXPORTS
#    define YARP_serversql_API YARP_EXPORT
#    define YARP_serversql_EXTERN YARP_EXPORT_EXTERN
#    define YARP_serversql_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_serversql_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_serversql_API YARP_IMPORT
#    define YARP_serversql_EXTERN YARP_IMPORT_EXTERN
#    define YARP_serversql_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_serversql_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_SERVERSQL_API_H
