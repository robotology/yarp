/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
#  ifdef YARP_NO_DEPRECATED
#    define YARP_tcpros_carrier_DEPRECATED_API YARP_DEPRECATED
#    define YARP_tcpros_carrier_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X)
#  else
#    define YARP_tcpros_carrier_DEPRECATED_API YARP_DEPRECATED YARP_wire_rep_utils_API
#    define YARP_tcpros_carrier_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X) YARP_wire_rep_utils_API
#  endif
#endif

#endif // YARP_TCPROS_CARRIER_API_H
