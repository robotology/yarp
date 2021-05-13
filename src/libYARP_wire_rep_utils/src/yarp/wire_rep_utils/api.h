/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_WIRE_REP_UTILS_WIRE_REP_UTILS_API_H
#define YARP_WIRE_REP_UTILS_WIRE_REP_UTILS_API_H

#include <yarp/conf/api.h>

// avoid dll export until YARP_wire_rep_utils becomes dynamic again
#define YARP_wire_rep_utils_API

#ifndef YARP_wire_rep_utils_API
#  ifdef wire_rep_utils_EXPORTS
#    define YARP_wire_rep_utils_API YARP_EXPORT
#    define YARP_wire_rep_utils_EXTERN YARP_EXPORT_EXTERN
#    define YARP_wire_rep_utils_DEPRECATED_API YARP_DEPRECATED_EXPORT
#    define YARP_wire_rep_utils_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_EXPORT(X)
#  else
#    define YARP_wire_rep_utils_API YARP_IMPORT
#    define YARP_wire_rep_utils_EXTERN YARP_IMPORT_EXTERN
#    define YARP_wire_rep_utils_DEPRECATED_API YARP_DEPRECATED_IMPORT
#    define YARP_wire_rep_utils_DEPRECATED_API_MSG(X) YARP_DEPRECATED_MSG_IMPORT(X)
#  endif
#endif

#endif // YARP_WIRE_REP_UTILS_API_WIRE_REP_UTILS_H
