/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/PortablePair.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>


bool yarp::os::PortablePairBase::readPair(ConnectionReader& connection,
                                          Portable& head,
                                          Portable& body)
{
    // if someone connects in text mode, use standard
    // text-to-binary mapping
    connection.convertTextMode();

    std::int32_t header = connection.expectInt32();
    if (header != BOTTLE_TAG_LIST) {
        return false;
    }
    std::int32_t len = connection.expectInt32();
    if (len != 2) {
        return false;
    }

    bool ok = head.read(connection);
    if (ok) {
        ok = body.read(connection);
    }
    return ok;
}

bool yarp::os::PortablePairBase::writePair(ConnectionWriter& connection,
                                           const Portable& head,
                                           const Portable& body)
{
    connection.appendInt32(BOTTLE_TAG_LIST); // nested structure
    connection.appendInt32(2);               // with two elements
    bool ok = head.write(connection);
    if (ok) {
        ok = body.write(connection);
    }

    if (ok) {
        // if someone connects in text mode,
        // let them see something readable.
        connection.convertTextMode();
    }

    return ok;
}
