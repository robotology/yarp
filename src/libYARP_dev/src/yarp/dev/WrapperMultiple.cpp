/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/WrapperMultiple.h>

#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(WRAPPERSINGLE, "yarp.dev.WrapperMultiple")
} // namespace


yarp::dev::WrapperMultiple::~WrapperMultiple() = default;


bool yarp::dev::WrapperMultiple::attach(PolyDriver *driver)
{
    if (!driver || !driver->isValid()) {
        yCError(WRAPPERSINGLE, "Could not attach an invalid device");
        return false;
    }

    yarp::dev::PolyDriverList drivers;
    drivers.push(driver, "...");

    return attachAll(drivers);
}


bool yarp::dev::WrapperMultiple::detach()
{
    return detachAll();
}
