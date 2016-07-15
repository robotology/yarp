/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/NameSpace.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Carriers.h>

using namespace yarp::os;
using namespace yarp::os::impl;


NameSpace::~NameSpace() {
}

bool NameSpace::checkNetwork() {
    if (localOnly()) return true;

    Contact c = queryName(getNameServerName());
    if (!c.isValid()) return false;

    OutputProtocol *out = Carriers::connect(c);
    if (out==YARP_NULLPTR) {
        return false;
    }

    out->close();
    delete out;
    out = YARP_NULLPTR;

    return true;
}

bool NameSpace::checkNetwork(double timeout) {
    Contact c = queryName(getNameServerName());
    if (!c.isValid()) return false;

    c.setTimeout((float)timeout);
    OutputProtocol *out = Carriers::connect(c);
    if (out==YARP_NULLPTR) {
        return false;
    }

    out->close();
    delete out;
    out = YARP_NULLPTR;

    return true;
}

ConstString NameSpace::getNameServerName() const {
    return getNameServerContact().getName();
}
