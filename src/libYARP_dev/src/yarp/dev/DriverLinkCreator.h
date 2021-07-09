/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_DRIVERLINKCREATOR_H
#define YARP_DEV_DRIVERLINKCREATOR_H

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

namespace yarp {
namespace dev {

/**
 * A factory for creating links to a driver that has already been
 * created.
 */
class YARP_dev_API DriverLinkCreator :
        public DriverCreator
{
private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
    PolyDriver holding;

public:
    DriverLinkCreator(const std::string& name, PolyDriver& source);
    virtual ~DriverLinkCreator() override;

    std::string toString() const override
    {
        return name;
    }

    DeviceDriver *create() const override
    {
        DeviceDriver *internal;
        const_cast<PolyDriver&>(holding).view(internal);
        return internal;
    }

    std::string getName() const override
    {
        return name;
    }

    std::string getWrapper() const override
    {
        return "(link)";
    }

    std::string getCode() const override
    {
        return "DriverLinkCreator";
    }

    PolyDriver *owner() override
    {
        return &holding;
    }

    void close()
    {
        holding.close();
    }
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_DRIVERLINKCREATOR_H
