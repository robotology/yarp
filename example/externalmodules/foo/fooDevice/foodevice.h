// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
      class foodevice;
    } 
}

class yarp::dev::foodevice : public DeviceDriver
{
private:
public:
    foodevice() {}
    virtual bool open(yarp::os::Searchable& config);
};

