// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP2_DRIVERS__
#define __YARP2_DRIVERS__

#include <yarp/os/Property.h>
#include <yarp/dev/DeviceDriver.h>

namespace yarp {
    namespace dev {
        template <class T> class DriverCreatorOf;
        class DriverCreator;
        class Drivers;
    }
}

class yarp::dev::DriverCreator {
public:
    virtual ~DriverCreator() {}

    // returns a simple description of devices the factory can make
    virtual yarp::os::ConstString toString() = 0;

    virtual DeviceDriver *create() = 0;
};


template <class T>
class yarp::dev::DriverCreatorOf : public DriverCreator {
private:
    yarp::os::ConstString desc;
public:
    DriverCreatorOf() : desc("unnamed") {
    }

    DriverCreatorOf(const char *str) : desc(str) {
    }

    virtual yarp::os::ConstString toString() {
        return desc;
    }

    virtual DeviceDriver *create() {
        return new T;
    }
};



class yarp::dev::Drivers {
public:
    static Drivers& factory() {
        return instance;
    }

    // calls create with the given name property
    DeviceDriver *open(const char *device) {
        yarp::os::Property p;
        p.put("device",device);
        return open(p);
    }

    virtual DeviceDriver *open(yarp::os::Searchable& prop);

    virtual yarp::os::ConstString toString();

    virtual ~Drivers();

    void add(DriverCreator *creator);

private:

    DriverCreator *find(const char *name);

    void init();

    void *implementation;

    Drivers();
    
    static Drivers instance;
};

#endif
