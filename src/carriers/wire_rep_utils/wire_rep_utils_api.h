// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_wire_rep_utils_api_API
#define YARP2_wire_rep_utils_api_API

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
#  else
#    define YARP_wire_rep_utils_DEPRECATED_API YARP_DEPRECATED YARP_wire_rep_utils_API
#  endif
#endif

#endif
