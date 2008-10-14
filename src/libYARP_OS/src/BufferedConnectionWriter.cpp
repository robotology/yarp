// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;


bool BufferedConnectionWriter::convertTextMode() {
    if (isTextMode()) {
        String s = toString();
        Bottle b;
        b.fromBinary(s.c_str(),s.length());
        clear();
        b.write(*this);
    }
    return true;
}


