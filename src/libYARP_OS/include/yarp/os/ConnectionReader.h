// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONNECTIONREADER_
#define _YARP2_CONNECTIONREADER_

#include <yarp/os/ConstString.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/Searchable.h>
#include <yarp/conf/numeric.h>

namespace yarp {
    namespace os {
        class ConnectionReader;
        class ConnectionWriter;
        class PortReader;
        class Portable;
        class InputStream;
    }

    // place in both namespaces
    using os::ConnectionReader;
    using os::ConnectionWriter;
}

/**
 * An interface for reading from a network connection.
 *
 * @see Port, PortReader, ConnectionWriter
 */
class YARP_OS_API yarp::os::ConnectionReader {
public:

    /**
     * Destructor.
     */
    virtual ~ConnectionReader();

    /**
     * Read a block of data from the network connection.
     * @param data Start of the block of data
     * @param len Length of the block of data
     *
     * @return true on success
     */
    virtual bool expectBlock(const char *data, size_t len) = 0;

    /**
     * Read some text from the network connection.
     * @param terminatingChar The marker for the end of the text
     * @return the text read from the connection
     */
    virtual ConstString expectText(int terminatingChar = '\n') = 0;

    /**
     * Read an integer from the network connection.
     * @return the integer read from the connection
     */
    virtual int expectInt() = 0;

    /**
     * Read a 64 bit integer from the network connection.
     * @return the integer read from the connection
     */
    virtual YARP_INT64 expectInt64() = 0;

    /**
     * Read a floating point number from the network connection.
     * @return the floating point number read from the connection
     */
    virtual double expectDouble() = 0;

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
     * Reads in a standard description in text mode, and converts
     * it to a standard description in binary.  Useful if you
     * only operate on the binary description usually, and just
     * want to permit text mode for command-line interaction.
     * If isTextMode would return false, no conversion is done.
     * @return true if the conversion was possible
     */
    virtual bool convertTextMode() = 0;

    /**
     * Checks how much data is available.
     * @return the number of bytes left on the connection.
     */
    virtual size_t getSize() = 0;


    /**
     * Gets a way to reply to the message, if possible.
     * @return An object that permits replies, or NULL if this cannot be done.
     */
    virtual ConnectionWriter *getWriter() = 0;

    /**
     *
     * Read a message envelope, if available.
     *
     */
    virtual Bytes readEnvelope();

    /**
     * Get a direct pointer to the object being sent, if possible.
     * This only makes sense in local operation, when sender and
     * receiver are in the same process; in all other situations
     * this returns NULL
     * @return The message object, or NULL if not available
     */
    virtual Portable *getReference() = 0;

    /**
     * Gets information about who is supplying the data being read, if
     * that information is available.
     *
     * @return contact information about sender (Contact::invalid if not
     * available)
     */
    virtual Contact getRemoteContact() = 0;

    /**
     * Gets information about who is receiving the data, if that
     * information is available.
     *
     * @return contact information about sender (Contact::invalid if not
     * available)
     */
    virtual Contact getLocalContact() = 0;

    /**
     * @return true if the reader is valid.  Invalid readers may signal
     * a shutdown.
     */
    virtual bool isValid() = 0;

    /**
     * @return true if the reader is active.  Readers become inactive
     * if the connection they are associated with breaks.
     */
    virtual bool isActive() = 0;

    /**
     * @return true if the reader encountered an error.  Readers can
     * encounter an error if there is some data loss.  For unreliable
     * protocols like UDP/Multicast, where losses are not unexpected,
     * this error flag will be reset for the next incoming message.
     */
    virtual bool isError() = 0;

    /**
     * Tag the connection to be dropped after the current message.
     */
    virtual void requestDrop() = 0;

    /**
     * Access modifiers associated with the connection, if any.
     *
     * @return connection configuration object
     *
     */
    virtual Searchable& getConnectionModifiers() = 0;

    /**
     * Store an integer to return on the next call to expectInt()
     * @param x the integer to store
     * @return true on success
     */
    virtual bool pushInt(int x) = 0;

    virtual bool setSize(size_t len) = 0;

    /**
     *
     * Create an instance of YARP's standard connection reader 
     * implementation.
     *
     */
    static ConnectionReader *createConnectionReader(InputStream& is);

    static bool readFromStream(PortReader& portable, InputStream& is);
};

#endif
