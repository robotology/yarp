/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_TCPROS_CARRIER_API_H
#define YARP_TCPROS_CARRIER_API_H

#include <yarp/conf/api.h>

// avoid dll export until ros becomes a library
#define YARP_tcpros_carrier_API

#ifndef YARP_tcpros_carrier_API
#  ifdef yarp_tcpros_EXPORTS
#    define YARP_tcpros_carrier_API YARP_EXPORT
#    define YARP_tcpros_carrier_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_tcpros_carrier_API YARP_IMPORT
#    define YARP_tcpros_carrier_EXTERN YARP_IMPORT_EXTERN
#  endif
#  define YARP_tcpros_carrier_DEPRECATED_API YARP_DEPRECATED_API
#  define YARP_tcpros_carrier_DEPRECATED_API_MSG(X) YARP_DEPRECATED_API_MSG(X)
#endif

#endif // YARP_TCPROS_CARRIER_API_H
