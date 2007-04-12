// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
      class bazbot;
    }
}

class yarp::dev::bazbot : public DeviceDriver
{
private:
public:
    bazbot() {}
    virtual bool open(yarp::os::Searchable& config);
};

