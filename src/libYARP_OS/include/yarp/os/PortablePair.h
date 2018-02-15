/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTABLEPAIR_H
#define YARP_OS_PORTABLEPAIR_H

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
    virtual bool read(ConnectionReader& connection) override {
        return readPair(connection, head, body);
    }

    /**
     * Writes this object pair to a network connection.
     * @param connection an interface to the network connection for writing
     * @return true iff the object pair was successfully written
     */
    virtual bool write(ConnectionWriter& connection) override {
        return writePair(connection, head, body);
    }

    /**
     * This is called when the port has finished all writing operations.
     * Passes call on to head and body.
     */
    virtual void onCompletion() override {
        head.onCompletion();
        body.onCompletion();
    }
};

#endif // YARP_OS_PORTABLEPAIR_H
