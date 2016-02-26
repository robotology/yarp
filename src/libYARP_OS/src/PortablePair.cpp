/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/PortablePair.h>


bool yarp::os::PortablePairBase::readPair(ConnectionReader& connection,
                                      Portable& head,
                                      Portable& body) {
    // if someone connects in text mode, use standard
    // text-to-binary mapping
    connection.convertTextMode();

    int header = connection.expectInt();
    if (header!=BOTTLE_TAG_LIST) { return false; }
    int len = connection.expectInt();
    if (len!=2) { return false; }

    bool ok = head.read(connection);
    if (ok) {
        ok = body.read(connection);
    }
    return ok;
}

bool yarp::os::PortablePairBase::writePair(ConnectionWriter& connection,
                                           Portable& head,
                                           Portable& body) {
    connection.appendInt(BOTTLE_TAG_LIST); // nested structure
    connection.appendInt(2);               // with two elements
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
