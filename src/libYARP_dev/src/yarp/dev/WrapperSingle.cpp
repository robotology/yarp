/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/WrapperSingle.h>

#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(WRAPPERSINGLE, "yarp.dev.WrapperSingle")
} // namespace


yarp::dev::WrapperSingle::~WrapperSingle() = default;


bool yarp::dev::WrapperSingle::attachAll(const yarp::dev::PolyDriverList& drivers)
{
    if (drivers.size() != 1) {
        yCError(WRAPPERSINGLE, "Expected only one device to be attached");
        return false;
    }

    if (!drivers[0]->poly || !drivers[0]->poly->isValid()) {
        yCError(WRAPPERSINGLE, "Could not attach an invalid device");
        return false;
    }

    return attach(drivers[0]->poly);
}


bool yarp::dev::WrapperSingle::detachAll()
{
    return detach();
}
