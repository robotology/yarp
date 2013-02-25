// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/ConnectionReader.h>

yarp::os::ConnectionReader::~ConnectionReader() {
}

yarp::os::Bytes yarp::os::ConnectionReader::readEnvelope() {
    return yarp::os::Bytes(0,0);
}
