// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Alex Bernardino & Carlos Beltran-Gonzalez
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
     * Gets one line (a sequence of chars with a ending '\\n' or '\\r') from the receive queue. The ending '\\n''\\r' chars are not removed in the returned line.
     * \param line - a previously allocated buffer where the received line is stored.
     * \param MaxLineLength - the size of the 'line' parameter.
     * \return - the number of received characters (including the '\n''\r' chars, plus the buffer terminator '\\0'). The function returns 0 if no chars are received.
     */
    virtual int  receiveLine(char* line, const int MaxLineLength) = 0;
    /**
     * Flushes the internal buffer.
     * \return - the number of flushed characters.
     */
    virtual int  flush() = 0;
};

#endif
