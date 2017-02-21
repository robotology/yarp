/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_GLDEBUG_H
#define YARP_OVRHEADSET_GLDEBUG_H

#include <yarp/os/Log.h>

namespace yarp {
namespace dev {

void checkGlError(const char* file, int line, const char* func);

} // namespace dev
} // namespace yarp

#define checkGlErrorMacro yarp::dev::checkGlError(__FILE__, __LINE__, __YFUNCTION__)

#endif // YARP_OVRHEADSET_GLDEBUG_H
