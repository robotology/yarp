/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_WIRE_REP_UTILS_WIRE_REP_UTILS_API_H
#define YARP_WIRE_REP_UTILS_WIRE_REP_UTILS_API_H

#include <yarp/conf/api.h>

// avoid dll export until libYARP_wire_rep_utils becomes dynamic again
#define YARP_wire_rep_utils_API

#ifndef YARP_wire_rep_utils_API
#  ifdef wire_rep_utils_EXPORTS
#    define YARP_wire_rep_utils_API YARP_EXPORT
#    define YARP_wire_rep_utils_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_wire_rep_utils_API YARP_IMPORT
#    define YARP_wire_rep_utils_EXTERN YARP_IMPORT_EXTERN
#  endif
#  ifdef YARP_NO_DEPRECATED
#    define YARP_wire_rep_utils_DEPRECATED_API YARP_DEPRECATED
#    define YARP_wire_rep_utils_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X)
#  else
#    define YARP_wire_rep_utils_DEPRECATED_API YARP_DEPRECATED YARP_wire_rep_utils_API
#    define YARP_wire_rep_utils_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG(X) YARP_wire_rep_utils_API
#  endif
#endif

#endif // YARP_WIRE_REP_UTILS_API_WIRE_REP_UTILS_H
