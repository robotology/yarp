/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_FACE_H
#define YARP_OS_FACE_H

#include <yarp/os/Contact.h>
#include <yarp/os/InputProtocol.h>
#include <yarp/os/OutputProtocol.h>

namespace yarp {
namespace os {

/**
 * The initial point-of-contact with a port.
 */
class YARP_os_API Face
{
public:
    /**
     * Constructor.
     */
    Face() = default;

    /**
     * Destructor.
     */
    virtual ~Face() = default;

    /**
     *
     * Start listening to the given address.
     *
     * @param address the parameters needed for listening.  E.g. for TCP,
     * this would contain a socket number.
     *
     * @return true on success.
     *
     */
    virtual bool open(const Contact& address) = 0;

    /**
     *
     * Stop listening.
     *
     */
    virtual void close() = 0;

    /**
     *
     * Block and wait for someone to talk to us.
     *
     * @return a protocol object for talking to someone, or nullptr on
     *         failure.
     *
     */
    virtual InputProtocol* read() = 0;

    /**
     *
     * Try to reach out and talk to someone.
     *
     * @param address parameters describing destination.  For example, for
     * TCP this would contain a machine name/IP address, and a socket number.
     *
     * @return a protocol object to talk with, or nullptr on failure.
     *
     */
    virtual OutputProtocol* write(const Contact& address) = 0;


    /**
     *
     * Get address after open(), if more specific that the address provided
     * to open() - otherwise an invalid address is returned.
     *
     * @return an address
     *
     */
    virtual Contact getLocalAddress() const
    {
        return Contact();
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_FACE_H
