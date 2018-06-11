/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is just an example for an empty device.

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
      class bazdevice;
    }
}

class yarp::dev::bazdevice : public DeviceDriver
{
private:
public:
    bazdevice() {}
    virtual bool open(yarp::os::Searchable& config);
};

