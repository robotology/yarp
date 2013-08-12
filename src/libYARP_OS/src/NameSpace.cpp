// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/NameSpace.h>
#include <yarp/os/impl/OutputProtocol.h>
#include <yarp/os/impl/Carriers.h>

using namespace yarp::os;
using namespace yarp::os::impl;


NameSpace::~NameSpace() {
}

bool NameSpace::checkNetwork() {
    Contact c = queryName(getNameServerName());
    if (!c.isValid()) return false;

    OutputProtocol *out = Carriers::connect(c);
    if (out==NULL) {
        return false;
    }

    out->close();
    delete out;
    out = NULL;

    return true;
}

bool NameSpace::checkNetwork(double timeout) {
    Contact c = queryName(getNameServerName());
    if (!c.isValid()) return false;

    c.setTimeout((float)timeout);
    OutputProtocol *out = Carriers::connect(c);
    if (out==NULL) {
        return false;
    }

    out->close();
    delete out;
    out = NULL;

    return true;
}

ConstString NameSpace::getNameServerName() const {
    return getNameServerContact().getName();
}
