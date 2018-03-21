/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_PRECISELYTIMED_H
#define YARP_DEV_PRECISELYTIMED_H

#include <yarp/os/Stamp.h>

#include <yarp/dev/api.h>

namespace yarp{
    namespace dev{
        class IPreciselyTimed;
    }
}

class YARP_dev_API yarp::dev::IPreciselyTimed {
public:
    /**
    * Return the time stamp relative to the last acquisition.
    */
    virtual yarp::os::Stamp getLastInputStamp() = 0;
    virtual ~IPreciselyTimed(){}
};

#endif // YARP_DEV_PRECISELYTIMED_H
