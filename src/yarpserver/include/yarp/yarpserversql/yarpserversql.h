// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_INC_YARPSERVERSQL
#define YARP_INC_YARPSERVERSQL

#include <yarp/conf/api.h>
#ifndef YARP_yarpserversql_API
#  ifdef YARP_yarpserversql_EXPORTS
#    define YARP_yarpserversql_API YARP_EXPORT
#  else
#     define YARP_yarpserversql_API YARP_IMPORT
#  endif
#endif

YARP_yarpserversql_API int yarpserver3_main(int argc, char *argv[]);

#endif
