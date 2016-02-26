/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Runnable.h>


yarp::os::Runnable::~Runnable() {
}

void yarp::os::Runnable::run() {
}

void yarp::os::Runnable::close() {
}

void yarp::os::Runnable::beforeStart() {
}

void yarp::os::Runnable::afterStart(bool success) {
}

bool yarp::os::Runnable::threadInit() {
    return true;
}

void yarp::os::Runnable::threadRelease() {
}
