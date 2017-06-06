/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_DRIVERLINKCREATOR_H
#define YARP_DEV_DRIVERLINKCREATOR_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

namespace yarp {
    namespace dev {
        class DriverLinkCreator;
    }
}

/**
 * A factory for creating links to a driver that has already been
 * created.
 */
class YARP_dev_API yarp::dev::DriverLinkCreator : public DriverCreator {
private:
    yarp::os::ConstString name;
    PolyDriver holding;
public:
    DriverLinkCreator(const yarp::os::ConstString& name, PolyDriver& source) {
        this->name = name;
        holding.link(source);
    }

    virtual ~DriverLinkCreator() {
        holding.close();
    }

    virtual yarp::os::ConstString toString() YARP_OVERRIDE {
        return name;
    }

    virtual DeviceDriver *create() YARP_OVERRIDE {
        DeviceDriver *internal;
        holding.view(internal);
        return internal;
    }

    virtual yarp::os::ConstString getName() YARP_OVERRIDE {
        return name;
    }

    virtual yarp::os::ConstString getWrapper() YARP_OVERRIDE {
        return "(link)";
    }

    virtual yarp::os::ConstString getCode() YARP_OVERRIDE {
        return "DriverLinkCreator";
    }

    virtual PolyDriver *owner() YARP_OVERRIDE {
        return &holding;
    }

    void close() {
        holding.close();
    }
};

#endif // YARP_DEV_DRIVERLINKCREATOR_H
