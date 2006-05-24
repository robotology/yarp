// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_DRIVERS__
#define __YARP2_DRIVERS__

#include <yarp/os/Property.h>
#include <yarp/dev/DeviceDriver.h>

namespace yarp {
  namespace dev {
      class DriverFactory;
      class Drivers;
  }
}


class yarp::dev::DriverFactory {
public:
    virtual ~DriverFactory() {}

    // calls create with the given name property
    DeviceDriver *create(const char *name) {
        yarp::os::Property p;
        p.put("name",name);
        return create(p);
    }

    // returns a simple description of devices the factory can make
    virtual yarp::os::ConstString toString() = 0;

    // returns null if name is not consistent
    virtual DeviceDriver *create(const yarp::os::Property& prop) = 0;
};



class yarp::dev::Drivers : public DriverFactory {
public:
    static DriverFactory& factory() {
        return instance;
    }

    virtual DeviceDriver *create(const yarp::os::Property& prop);

    virtual yarp::os::ConstString toString();

    virtual ~Drivers();

private:

    void *implementation;

    Drivers();
    
    static Drivers instance;
};

#endif
