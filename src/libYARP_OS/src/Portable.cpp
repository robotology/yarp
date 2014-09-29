// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Portable.h>
#include <yarp/os/DummyConnector.h>

bool yarp::os::Portable::copyPortable(yarp::os::PortWriter& writer, yarp::os::PortReader& reader) {
    yarp::os::DummyConnector con;
    if (!writer.write(con.getWriter())) return false;
    return reader.read(con.getReader());
}
