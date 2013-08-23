// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/StreamConnectionReader.h>

using namespace yarp::os;
using namespace yarp::os::impl;

ConnectionReader::~ConnectionReader() {
}

Bytes ConnectionReader::readEnvelope() {
    return Bytes(0,0);
}

ConnectionReader *ConnectionReader::createConnectionReader(InputStream& is) {
    StreamConnectionReader *reader = new StreamConnectionReader();
    Route r;
    reader->reset(is,NULL,r,0,false);
    return reader;
}
