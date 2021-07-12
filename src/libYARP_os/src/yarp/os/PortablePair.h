/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTABLEPAIR_H
#define YARP_OS_PORTABLEPAIR_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Portable.h>

namespace yarp {
namespace os {

class YARP_os_API PortablePairBase : public Portable
{
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
                          const Portable& head,
                          const Portable& body);
};

/**
 * Group a pair of objects to be sent and received together.
 * Handy for adding general-purpose headers, for example.
 */
template <class HEAD, class BODY>
class PortablePair : public PortablePairBase
{
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
    bool read(ConnectionReader& connection) override
    {
        return readPair(connection, head, body);
    }

    /**
     * Writes this object pair to a network connection.
     * @param connection an interface to the network connection for writing
     * @return true iff the object pair was successfully written
     */
    bool write(ConnectionWriter& connection) const override
    {
        return writePair(connection, head, body);
    }

    /**
     * This is called when the port has finished all writing operations.
     * Passes call on to head and body.
     */
    void onCompletion() const override
    {
        head.onCompletion();
        body.onCompletion();
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTABLEPAIR_H
