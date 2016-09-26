/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// This is just an example for an empty device.

#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
      class foobot;
    }
}

class yarp::dev::foobot : public DeviceDriver
{
private:
public:
    foobot() {}
    virtual bool open(yarp::os::Searchable& config);
};

