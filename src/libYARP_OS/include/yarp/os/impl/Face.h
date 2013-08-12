// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_FACE_
#define _YARP2_FACE_

#include <yarp/os/Contact.h>
#include <yarp/os/impl/InputProtocol.h>
#include <yarp/os/impl/OutputProtocol.h>

#include <yarp/os/impl/AuthHMAC.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Face;
        }
    }
}

/**
 * The initial point-of-contact with a port.
 */
class YARP_OS_impl_API yarp::os::impl::Face {
public:
    /**
     * Constructor.
     */
    Face() : auth() {};

    /**
     * Destructor.
     */
    virtual ~Face() {}

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
     * @return a protocol object for talking to someone, or NULL on failure.
     *
     */
    virtual InputProtocol *read() = 0;

    /**
     *
     * Try to reach out and talk to someone.
     *
     * @param address parameters describing destination.  For example, for
     * TCP this would contain a machine name/IP address, and a socket number.
     *
     * @return a protocol object to talk with, or NULL on failure.
     *
     */
    virtual OutputProtocol *write(const Contact& address) = 0;


    /**
     *
     * Get address after open(), if more specific that the address provided
     * to open() - otherwise an invalid address is returned.
     *
     * @return an address
     *
     */
    virtual Contact getLocalAddress() {
        return Contact();
    }

protected:

    yarp::os::impl::AuthHMAC auth;
};

#endif

