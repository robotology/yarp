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
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
    PolyDriver holding;
public:
    DriverLinkCreator(const std::string& name, PolyDriver& source) {
        this->name = name;
        holding.link(source);
    }

    virtual ~DriverLinkCreator() {
        holding.close();
    }

    virtual std::string toString() const override {
        return name;
    }

    virtual DeviceDriver *create() override {
        DeviceDriver *internal;
        holding.view(internal);
        return internal;
    }

    virtual std::string getName() const override {
        return name;
    }

    virtual std::string getWrapper() const override {
        return "(link)";
    }

    virtual std::string getCode() const override {
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
