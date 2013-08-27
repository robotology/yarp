// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

using namespace yarp::os;
using namespace yarp::os::impl;

ConnectionWriter::~ConnectionWriter() {
}

bool ConnectionWriter::isNull() const {
    return false;
}


ConnectionWriter *ConnectionWriter::createBufferedConnectionWriter() {
    return new BufferedConnectionWriter;
}
