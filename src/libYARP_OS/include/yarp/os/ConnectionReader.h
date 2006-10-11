// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_CONNECTIONREADER_
#define _YARP2_CONNECTIONREADER_

#include <yarp/os/ConstString.h>
#include <yarp/os/Contact.h>

namespace yarp {
    namespace os {
        class ConnectionReader;
        class ConnectionWriter;
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
class yarp::os::ConnectionReader {
public:

    /**
     * Destructor.
     */
    virtual ~ConnectionReader() {
    }

    /**
     * Read a block of data from the network connection.
     * @param data Start of the block of data
     * @param len Length of the block of data
     */
    virtual void expectBlock(const char *data, int len) = 0;

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
    virtual int getSize() = 0;


    /**
     * Gets a way to reply to the message, if possible.
     * @return An object that permits replies, or NULL if this cannot be done.
     */
    virtual ConnectionWriter *getWriter() = 0;

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
     * @return true if the reader is valid.  A reader may be invalid
     * if a connection has closed.
     */
    virtual bool isValid() = 0;
};

#endif
