/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual yarp::os::ConstString toString() override {
        return name;
    }

    virtual DeviceDriver *create() override {
        DeviceDriver *internal;
        holding.view(internal);
        return internal;
    }

    virtual yarp::os::ConstString getName() override {
        return name;
    }

    virtual yarp::os::ConstString getWrapper() override {
        return "(link)";
    }

    virtual yarp::os::ConstString getCode() override {
        return "DriverLinkCreator";
    }

    virtual PolyDriver *owner() override {
        return &holding;
    }

    void close() {
        holding.close();
    }
};

#endif // YARP_DEV_DRIVERLINKCREATOR_H
