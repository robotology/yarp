/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONNECTIONWRITER_H
#define YARP_OS_CONNECTIONWRITER_H

#include <yarp/conf/numeric.h>

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {
class ConnectionReader;
class PortReader;
class PortWriter;
class Portable;
class SizedWriter;
class OutputStream;
} // namespace os
} // namespace yarp


namespace yarp {
namespace os {

/**
 * An interface for writing to a network connection.
 *
 * @see Port, PortWriter, ConnectionReader
 */
class YARP_os_API ConnectionWriter
{
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
    virtual void appendBlock(const char* data, size_t len) = 0;

    /**
     * Send a representation of an integer to the network connection.
     * @param data the integer to send
     * @warning Unsafe, sizeof(int) is platform dependent. Use appendInt32 instead.
     */
    YARP_DEPRECATED_MSG("Use appendInt32 instead") // Since YARP 3.5.0
    virtual void appendInt(int data) final
    {
        appendInt32(static_cast<std::int32_t>(data));
    }

    /**
     * Send a representation of a 8-bit integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt8(std::int8_t data) = 0;

    /**
     * Send a representation of a 16-bit integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt16(std::int16_t data) = 0;

    /**
     * Send a representation of a 32-bit integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt32(std::int32_t data) = 0;

    /**
     * Send a representation of a 64-bit integer to the network connection.
     * @param data the integer to send
     */
    virtual void appendInt64(std::int64_t data) = 0;

    /**
     * Send a representation of a floating point number to the network
     * connection.
     *
     * @param data the floating point number to send
     * @warning Unsafe, sizeof(double) is platform dependent. Use appendFloat64 instead.
     */
    YARP_DEPRECATED_MSG("Use appendFloat64 instead") // Since YARP 3.5.0
    virtual void appendDouble(double data)
    {
        appendFloat64(static_cast<yarp::conf::float64_t>(data));
    }

    /**
     * Send a representation of a 32-bit floating point number to the network
     * connection.
     *
     * @param data the floating point number to send
     */
    virtual void appendFloat32(yarp::conf::float32_t data) = 0;

    /**
     * Send a representation of a 64-bit floating point number to the network
     * connection.
     *
     * @param data the floating point number to send
     */
    virtual void appendFloat64(yarp::conf::float64_t data) = 0;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
    /**
     * Send a character sequence to the network connection.
     * @param str the character sequence to send
     * @param terminate the terminating character to use
     * @deprecated since YARP 3.2
     */
    YARP_DEPRECATED_MSG("Use appendText() instead")
    virtual void appendString(const char* str, const char terminate = '\n') final
    {
        appendText({str}, terminate);
    }
#endif

    /**
     * Send a terminated string to the network connection.
     *
     * The lenght of string is not specified in advance, therefore the
     * reader should read until the terminating character is found.
     *
     * @param str the string to send
     * @param terminate the terminating character to use
     */
    virtual void appendText(const std::string& str, const char terminate = '\n') = 0;

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2
    YARP_DEPRECATED_MSG("Use appendString() instead")
    virtual void appendRawString(const std::string& str) final
    {
        appendString(str);
    }
#endif

    /**
     * Send a string to the network connection.
     *
     * The lenght of string is not specified in advance, therefore the
     * reader should read the number of bytes specified by the first integer.
     * The string terminating character (normally '\0') is not transmitted.
     *
     * @param str the string to send
     */
    void appendString(const std::string& str)
    {
        appendInt32(static_cast<std::int32_t>(str.length()));
        appendBlock((char*)str.c_str(), str.length());
    }

    /**
     * Send a block of data to the network connection, without making a copy.
     * If you are not confident that the block of data will remain valid
     * throughout transmission, call ConnectionWriter::appendBlock instead.
     * @param data the start of the data block
     * @param len  the length of the data block
     */
    virtual void appendExternalBlock(const char* data, size_t len) = 0;

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
     * If you can easily determine how many blocks there are in a message,
     * call this first, before anything else.
     * This may improve efficiency in some situations.
     * @param argc Number of blocks
     * @param argv An array of integers, giving the length of each block
     */
    virtual void declareSizes(int argc, int* argv) = 0;

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
    virtual void setReference(Portable* obj) = 0;


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
    virtual bool isValid() const = 0;

    /**
     * @return true if the writer is active.  Writers may become inactive
     * if the connection they are associated with breaks.
     */
    virtual bool isActive() const = 0;

    /**
     * @return true if the writer encountered an error.  Writers can
     * encounter an error if there is some data loss.  For unreliable
     * protocols like UDP/Multicast, where losses are not unexpected,
     * this error flag will be reset for the next incoming message.
     */
    virtual bool isError() const = 0;


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
    virtual SizedWriter* getBuffer() const = 0;

    /**
     *
     * Create a connection writer implementation that stores
     * to a buffer which can be read later using getBuffer()
     *
     */
    static ConnectionWriter* createBufferedConnectionWriter();

    static bool writeToStream(PortWriter& portable, OutputStream& os);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CONNECTIONWRITER_H
