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


/**
 * A factory for creating driver objects.
 */
class yarp::dev::DriverCreator {
public:
    virtual ~DriverCreator() {}

    // returns a simple description of devices the factory can make
    virtual yarp::os::ConstString toString() = 0;

    virtual DeviceDriver *create() = 0;
    
    virtual yarp::os::ConstString getName() = 0;
    virtual yarp::os::ConstString getWrapper() = 0;
    virtual yarp::os::ConstString getCode() = 0;
};


/**
 * A factory for creating driver objects of a particular type.
 */
template <class T>
class yarp::dev::DriverCreatorOf : public DriverCreator {
private:
    yarp::os::ConstString desc, wrap, code;
public:
    DriverCreatorOf() : desc("unnamed"), wrap("unnamed"), code("unnamed") {
    }

    /**
     * Constructor.  Sets up the name by which the device will be known.
     * @param name The "common name" of the device.  This is the name 
     * that will be used when naming the device externally, for example
     * from the command line.
     * @param wrap The "common name" of another device which can wrap
     * this device up for network access.  If there is no such device,
     * use an empty name: ""
     * @param code The name of a class associated with this device.
     * This is to give the user an entry point into the relevant
     * code documentation.
     *
     */
    DriverCreatorOf(const char *name, const char *wrap, const char *code) : 
        desc(name), wrap(wrap), code(code)
    {
    }

    virtual yarp::os::ConstString toString() {
        return desc;
    }

    virtual yarp::os::ConstString getName() {
        return desc;
    }

    virtual yarp::os::ConstString getWrapper() {
        return wrap;
    }

    virtual yarp::os::ConstString getCode() {
        return code;
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
