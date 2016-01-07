// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Author Alessandro Scalzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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

#endif
