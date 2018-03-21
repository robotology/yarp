/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Portable.h>
#include <yarp/os/DummyConnector.h>

bool yarp::os::Portable::copyPortable(yarp::os::PortWriter& writer, yarp::os::PortReader& reader) {
    yarp::os::DummyConnector con;
    if (!writer.write(con.getWriter())) return false;
    return reader.read(con.getReader());
}
