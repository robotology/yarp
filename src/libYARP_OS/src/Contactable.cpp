/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Contactable.h>
#include <yarp/os/AbstractContactable.h>


yarp::os::Contactable::~Contactable() {
}

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
bool yarp::os::Contactable::open() {
    return open("...");
}
#endif // YARP_NO_DEPRECATED

yarp::os::ConstString yarp::os::Contactable::getName() const {
    return where().getName();
}
