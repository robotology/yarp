// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTABLEPAIR_
#define _YARP2_PORTABLEPAIR_

#include <yarp/os/Portable.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        class PortablePairBase;
        template <class HEAD, class BODY> class PortablePair;
    }
}

class YARP_OS_API yarp::os::PortablePairBase : public Portable {
public:
    /**
     * Reads an object pair from a network connection.
     * @param connection an interface to the network connection for reading
     * @param head the first object
     * @param body the second object
     * @return true iff the object pair was successfully read
     */
    static bool readPair(ConnectionReader& connection,
                         Portable& head,
                         Portable& body);

    /**
     * Writes an object pair to a network connection.
     * @param connection an interface to the network connection for writing
     * @param head the first object
     * @param body the second object
     * @return true iff the object pair was successfully written
     */
    static bool writePair(ConnectionWriter& connection,
                          Portable& head,
                          Portable& body);
};

/**
 * Group a pair of objects to be sent and received together.
 * Handy for adding general-purpose headers, for example.
 */
template <class HEAD, class BODY>
class yarp::os::PortablePair : public PortablePairBase {
public:
    /**
     * An object of the first type (HEAD).
     */
    HEAD head;

    /**
     * An object of the second type (BODY).
     */
    BODY body;

    /**
     * Reads this object pair from a network connection.
     * @param connection an interface to the network connection for reading
     * @return true iff the object pair was successfully read
     */
    virtual bool read(ConnectionReader& connection) {
        return readPair(connection,head,body);
    }

    /**
     * Writes this object pair to a network connection.
     * @param connection an interface to the network connection for writing
     * @return true iff the object pair was successfully written
     */
    virtual bool write(ConnectionWriter& connection) {
        return writePair(connection,head,body);
    }

    /**
     * This is called when the port has finished all writing operations.
     * Passes call on to head and body.
     */
    virtual void onCompletion() {
        head.onCompletion();
        body.onCompletion();
    }
};

#endif
