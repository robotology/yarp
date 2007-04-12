// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


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

