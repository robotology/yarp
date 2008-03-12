// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Alex Bernardino & Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARPSERIALINTERFACES__
#define __YARPSERIALINTERFACES__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/sig/Vector.h>

/*! \file SerialInterfaces.h define interfaces for a generic serial device (I/O)*/

namespace yarp {
    namespace dev {
        class ISerialDevice;
    }
}

/**
 * @ingroup dev_iface_other
 *
 * A generic interface to serial port devices.
 */
class yarp::dev::ISerialDevice
{
public:
    virtual ~ISerialDevice(){}

    /**
     * Sends a string of chars to the serial communications channel.
     * @param msg the string to send
     * @return true on success
     */
    virtual bool send(const yarp::os::Bottle& msg) = 0;
    virtual bool send(char *msg, size_t size) = 0;
    //bool putMessage(Bottle& msg, bool waitreply, double replytimeout, Bottle& reply, char *replydelimiter, int replysize );
    /**
     * Gets the existing chars in the receive queue.
     * @param msg - the received string
     * @return - true on success; false if no messages available
     */
    virtual bool receive(yarp::os::Bottle& msg) = 0;
};

#endif
//
