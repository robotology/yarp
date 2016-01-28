// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_NAME_API_H
#define YARP_NAME_API_H

#include <yarp/conf/api.h>
#ifndef YARP_name_API
#  ifdef YARP_name_EXPORTS
#    define YARP_name_API YARP_EXPORT
#    define YARP_name_EXTERN YARP_EXPORT_EXTERN
#  else
#    define YARP_name_API YARP_IMPORT
#    define YARP_name_EXTERN YARP_IMPORT_EXTERN
#  endif
#  ifdef YARP_NO_DEPRECATED
#    define YARP_name_DEPRECATED_API YARP_DEPRECATED
#  else
#    define YARP_name_DEPRECATED_API YARP_DEPRECATED YARP_name_API
#  endif
#endif

#endif // YARP_NAME_API_H
