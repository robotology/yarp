// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_dev_API_
#define _YARP2_dev_API_

#include <yarp/conf/api.h>
#ifndef YARP_dev_API
#  ifdef YARP_dev_EXPORTS
#    define YARP_dev_API YARP_EXPORT
#  else
#    define YARP_dev_API YARP_IMPORT
#  endif
#endif

#endif
