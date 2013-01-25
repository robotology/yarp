// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_tcpros_carrier_api_API_
#define _YARP2_tcpros_carrier_api_API_

#include <yarp/conf/api.h>

// avoid dll export until ros becomes a library
#define YARP_tcpros_carrier_API

#ifndef YARP_tcpros_carrier_API
#  ifdef yarp_tcpros_EXPORTS
#    define YARP_tcpros_carrier_API YARP_EXPORT
#  else
#     define YARP_tcpros_carrier_API YARP_IMPORT
#  endif
#endif

#endif
