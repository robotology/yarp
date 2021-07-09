/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONNECTIONREADER_H
#define YARP_OS_CONNECTIONREADER_H

#include <yarp/conf/numeric.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Searchable.h>

#include <string>

namespace yarp {
namespace os {
class ConnectionWriter;
class PortReader;
class Portable;
class InputStream;
} // namespace os
} // namespace yarp


namespace yarp {
namespace os {

/**
 * An interface for reading from a network connection.
 *
 * @see Port, PortReader, ConnectionWriter
 */
class YARP_os_API ConnectionReader
{
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
    virtual bool expectBlock(char* data, size_t len) = 0;

    /**
     * Read some text from the network connection.
     * @param terminatingChar The marker for the end of the text
     * @return the text read from the connection
     */
    virtual std::string expectText(const char terminatingChar = '\n') = 0;

    /**
     * Read a string from the network connection.
     * The string should be serialized as "length" + "block".
     * @return the string read from the connection
     */
    virtual std::string expectString()
    {
        std::string ret;
        std::int32_t len = expectInt32();
        if (!isError()) {
            ret.resize(static_cast<size_t>(len));
            expectBlock(const_cast<char*>(ret.data()), len);
        }
        return ret;
    }

    /**
     * Read an integer from the network connection.
     * @return the integer read from the connection
     * @warning Unsafe, sizeof(int) is platform dependent. Use expectInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use expectInt32 instead") // Since YARP 3.5.0
    virtual int expectInt() final
    {
        return static_cast<int>(expectInt32());
    }

    /**
     * Read a 8-bit integer from the network connection.
     * @return the integer read from the connection
     */
    virtual std::int8_t expectInt8() = 0;

    /**
     * Read a 16-bit integer from the network connection.
     * @return the integer read from the connection
     */
    virtual std::int16_t expectInt16() = 0;

    /**
     * Read a 32-bit integer from the network connection.
     * @return the integer read from the connection
     */
    virtual std::int32_t expectInt32() = 0;

    /**
     * Read a 64-bit integer from the network connection.
     * @return the integer read from the connection
     */
    virtual std::int64_t expectInt64() = 0;

    /**
     * Read a floating point number from the network connection.
     * @return the floating point number read from the connection
     * @warning Unsafe, sizeof(double) is platform dependent. Use expectFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use expectFloat64 instead") // Since YARP 3.5.0
    virtual double expectDouble()
    {
        return static_cast<double>(expectFloat64());
    }

    /**
     * Read a 32-bit floating point number from the network connection.
     * @return the floating point number read from the connection
     */
    virtual yarp::conf::float32_t expectFloat32() = 0;

    /**
     * Read a 64-bit floating point number from the network connection.
     * @return the floating point number read from the connection
     */
    virtual yarp::conf::float64_t expectFloat64() = 0;

    /**
     * Check if the connection is text mode.  If it is, you are
     * encouraged (but by no means required) to use a human-readable
     * representation of your data structure.
     * @return true if the connection is text mode (as opposed to binary)
     */
    virtual bool isTextMode() const = 0;

    /**
     * Check if the connection is bare mode.  If it is, you are
     * encouraged to omit type information from your serialization.
     * @return true if the connection is bare
     */
    virtual bool isBareMode() const = 0;

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
    virtual size_t getSize() const = 0;


    /**
     * Gets a way to reply to the message, if possible.
     * @return An object that permits replies, or nullptr if this cannot be
     *         done.
     */
    virtual ConnectionWriter* getWriter() = 0;

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
     * this returns nullptr.
     * @return The message object, or nullptr if not available
     */
    virtual Portable* getReference() const = 0;

    /**
     * Gets information about who is supplying the data being read, if
     * that information is available.
     *
     * @return contact information about sender (Contact::invalid if not
     * available)
     */
    virtual Contact getRemoteContact() const = 0;

    /**
     * Gets information about who is receiving the data, if that
     * information is available.
     *
     * @return contact information about sender (Contact::invalid if not
     * available)
     */
    virtual Contact getLocalContact() const = 0;

    /**
     * @return true if the reader is valid.  Invalid readers may signal
     * a shutdown.
     */
    virtual bool isValid() const = 0;

    /**
     * @return true if the reader is active.  Readers become inactive
     * if the connection they are associated with breaks.
     */
    virtual bool isActive() const = 0;

    /**
     * @return true if the reader encountered an error.  Readers can
     * encounter an error if there is some data loss.  For unreliable
     * protocols like UDP/Multicast, where losses are not unexpected,
     * this error flag will be reset for the next incoming message.
     */
    virtual bool isError() const = 0;

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
    virtual const Searchable& getConnectionModifiers() const = 0;

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
    static ConnectionReader* createConnectionReader(InputStream& is);

    static bool readFromStream(PortReader& portable, InputStream& is);

    /**
     * Set ConnectionReader to be used for reading the envelope.
     *
     * Used by PortCoreInputUnit
     */
    virtual void setParentConnectionReader(ConnectionReader* parentConnectionReader);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CONNECTIONREADER_H
