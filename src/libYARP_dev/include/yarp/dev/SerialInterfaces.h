/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006 Alexandre Bernardino
 * Copyright (C) 2006 Carlos Beltran-Gonzalez
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_SERIALINTERFACES_H
#define YARP_DEV_SERIALINTERFACES_H

#include <yarp/os/Bottle.h>
#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>

/*! \file SerialInterfaces.h define interfaces for a generic serial device (I/O)*/

namespace yarp {
    namespace dev {
        class ISerialDevice;
    }
}

/**
 * \ingroup dev_iface_other
 *
 * \brief A generic interface to serial port devices.
 */
class YARP_dev_API yarp::dev::ISerialDevice
{
public:
    virtual ~ISerialDevice(){}

    /**
     * Sends a string of chars to the serial communications channel.
     * \param msg the string to send
     * \return true on success
     */
    virtual bool send(const yarp::os::Bottle& msg) = 0;
    virtual bool send(char *msg, size_t size) = 0;

    //bool putMessage(Bottle& msg, bool waitreply, double replytimeout, Bottle& reply, char *replydelimiter, int replysize );
    /**
     * Gets the existing chars in the receive queue.
     * \param msg - the received string
     * \return - true on success; false if no messages available
     */
    virtual bool receive(yarp::os::Bottle& msg) = 0;

    /**
     * Gets one single char from the receive queue.
     * \param chr - the received char.
     * \return - 0 if no chars are received; 1 if one char is received.
     */
    virtual int  receiveChar(char& chr) = 0;

    /**
    * Gets an array of bytes (unsigned char) with size <= 'size' parameter. The array is NOT null terminated.
    * @param bytes - a previously allocated buffer where the received data is stored.
    * @param size - the size of the 'bytes' parameter.
    * @return - the number of received bytes. The function returns 0 if no bytes are received.
    */
    virtual int  receiveBytes(unsigned char* bytes, const int size) = 0;

    /**
    * Gets one line (a sequence of chars with a ending '\\n' or '\\r') from the receive queue. The ending '\\n''\\r' chars are not removed in the returned line.
    * \param line - a previously allocated buffer where the received line is stored.
    * \param MaxLineLength - the size of the 'line' parameter.
    * \return - the number of received characters (including the '\n''\r' chars, plus the buffer terminator '\\0'). The function returns 0 if no chars are received.
    */

    virtual int  receiveLine(char* line, const int MaxLineLength) = 0;
    /**
    * Enable/Disable DTR protocol
    * @param enable Enable/Disable DTR protocol
    * @return true on success
    */
    virtual bool setDTR(bool enable) = 0;
    /**
     * Flushes the internal buffer.
     * \return - the number of flushed characters.
     */
    virtual int  flush() = 0;
};

#endif // YARP_DEV_SERIALINTERFACES_H
