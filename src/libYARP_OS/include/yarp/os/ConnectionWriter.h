// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONNECTIONWRITER_
#define _YARP2_CONNECTIONWRITER_

#include <yarp/os/ConstString.h>
#include <yarp/conf/numeric.h>

namespace yarp {
    namespace os {
        class ConnectionWriter;
        class ConnectionReader;
        class PortReader;
        class PortWriter;
        class Portable;
        class SizedWriter;
        class OutputStream;
    }
}

/**
 * An interface for writing to a network connection.
 *
 * @see Port, PortWriter, ConnectionReader
 */
class YARP_OS_API yarp::os::ConnectionWriter {
public:

    /**
     * Destructor.
     */
    virtual ~ConnectionWriter();

    /**
     * Send a block of data to the network connection.
     * Since communication may not happen immediately, or may
     * happen multiple times, a copy of this data is made.
     * If you know the block of data will remain valid,
     * and it is a large block of data,
     * please call ConnectionWriter::appendExternalBlock instead.
     * @param data the start of the data block
     * @param len  the length of the data block
     */
    virtual void appendBlock(const char *data, size_t len) = 0;

    /**
     * Send a representation of an integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt(int data) = 0;

    /**
     * Send a representation of a 64-bit integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt64(const YARP_INT64& data) = 0;

    /**
     * Send a representation of a floating point number to the network
     * connection.
     *
     * @param data the floating point number to send
     */
    virtual void appendDouble(double data) = 0;

    /**
     * Send a character sequence to the network connection.
     * @param str the character sequence to send
     * @param terminate the terminating character to use
     */
    virtual void appendString(const char *str, int terminate = '\n') = 0;

    /**
     * Send a block of data to the network connection, without making a copy.
     * If you are not confident that the block of data will remain valid
     * throughout transmission,
     * then call ConnectionWriter::appendBlock instead.
     * @param data the start of the data block
     * @param len  the length of the data block
     */
    virtual void appendExternalBlock(const char *data, size_t len) = 0;

    /**
     * Check if the connection is text mode.  If it is, you are
     * encouraged (but by no means required) to use a human-readable
     * representation of your data structure.
     * @return true if the connection is text mode (as opposed to binary)
     */
    virtual bool isTextMode() = 0;

    /**
     * Check if the connection is bare mode.  If it is, you are
     * encouraged to omit type information from your serialization.
     * @return true if the connection is bare
     */
    virtual bool isBareMode() = 0;

    /**
     * If you can easily determine how many blocks there are in a message,
     * call this first, before anything else.
     * This may improve efficiency in some situations.
     * @param argc Number of blocks
     * @param argv An array of integers, giving the length of each block
     */
    virtual void declareSizes(int argc, int *argv) = 0;

    /**
     * This sets a handler to deal with replies to the message.  The
     * handler will be called once per connection. There will be
     * problems for connections using carriers that don't support replies.
     * @param reader the object that handles replies.
     */
    virtual void setReplyHandler(PortReader& reader) = 0;

    /**
     * Stores a direct pointer to the object being sent.
     * This is useful for local communication, to bypass
     * serialization.
     */
    virtual void setReference(Portable *obj) = 0;


    /**
     * Converts a standard description in binary into a textual
     * description, if the connection is in text-mode.  Useful if you
     * only operate on the binary description usually, and just
     * want to permit text mode for command-line interaction.
     * If isTextMode would return false, no conversion is done.
     * @return true if the conversion was possible
     */
    virtual bool convertTextMode() = 0;


    /**
     * @return true if the writer is valid.  A writer may be invalid
     * if a connection has closed.
     */
    virtual bool isValid() = 0;

    /**
     * @return true if the writer is active.  Writers may become inactive
     * if the connection they are associated with breaks.
     */
    virtual bool isActive() = 0;

    /**
     * @return true if the writer encountered an error.  Writers can
     * encounter an error if there is some data loss.  For unreliable
     * protocols like UDP/Multicast, where losses are not unexpected,
     * this error flag will be reset for the next incoming message.
     */
    virtual bool isError() = 0;


    /**
     *
     * Tag the connection to be dropped after the current message.
     *
     */
    virtual void requestDrop() = 0;

    /**
     *
     * @return true if writer goes nowhere.
     *
     */
    virtual bool isNull() const;


    /**
     *
     * @return a buffer if one is present.
     *
     */
    virtual SizedWriter *getBuffer() = 0;


    virtual void appendRawString(const ConstString& str) {
        appendInt(str.length());
        appendBlock((char*)str.c_str(),str.length());
    }

    /**
     *
     * Create a connection writer implementation that stores
     * to a buffer which can be read later using getBuffer()
     *
     */
    static ConnectionWriter *createBufferedConnectionWriter();

    static bool writeToStream(PortWriter& portable, OutputStream& os);
};

#endif
