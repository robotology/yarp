/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SERVERSQL_API_H
#define YARP_SERVERSQL_API_H

#include <yarp/conf/api.h>
#ifndef YARP_serversql_API
#  ifdef YARP_serversql_EXPORTS
#    define YARP_serversql_API YARP_EXPORT
#    define YARP_serversql_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_serversql_API YARP_IMPORT
#    define YARP_serversql_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_serversql_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_serversql_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_SERVERSQL_API_H
