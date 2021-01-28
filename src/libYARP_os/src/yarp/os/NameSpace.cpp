/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NameSpace.h>

#include <yarp/os/Carriers.h>
#include <yarp/os/OutputProtocol.h>

using namespace yarp::os;
using namespace yarp::os::impl;


NameSpace::~NameSpace() = default;

bool NameSpace::checkNetwork()
{
    if (localOnly()) {
        return true;
    }

    Contact c = queryName(getNameServerName());
    if (!c.isValid()) {
        return false;
    }

    OutputProtocol* out = Carriers::connect(c);
    if (out == nullptr) {
        return false;
    }

    out->close();
    delete out;
    out = nullptr;

    return true;
}

bool NameSpace::checkNetwork(double timeout)
{
    if (localOnly()) {
        return true;
    }

    Contact c = queryName(getNameServerName());
    if (!c.isValid()) {
        return false;
    }

    c.setTimeout((float)timeout);
    OutputProtocol* out = Carriers::connect(c);
    if (out == nullptr) {
        return false;
    }

    out->close();
    delete out;
    out = nullptr;

    return true;
}

std::string NameSpace::getNameServerName() const
{
    return getNameServerContact().getName();
}
