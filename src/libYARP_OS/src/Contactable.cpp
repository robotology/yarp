// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Contactable.h>


yarp::os::Contactable::~Contactable() {
}

bool yarp::os::Contactable::open() {
    return open("...");
}

yarp::os::ConstString yarp::os::Contactable::getName() const {
    return where().getName();
}
