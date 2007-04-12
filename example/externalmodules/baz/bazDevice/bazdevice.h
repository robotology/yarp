// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


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

