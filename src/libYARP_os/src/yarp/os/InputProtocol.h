/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_INPUTPROTOCOL_H
#define YARP_OS_INPUTPROTOCOL_H

#include <yarp/os/Connection.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>

#include <string>

namespace yarp {
namespace os {
class OutputProtocol;
class Contactable;
} // namespace os
} // namespace yarp


namespace yarp {
namespace os {

/**
 * The input side of an active connection between two ports.
 */
class YARP_os_API InputProtocol
{
public:
    /**
     * Destructor.
     */
    virtual ~InputProtocol() = default;

    /**
     * Start negotiating a carrier, using the given name as our own
     * if a name is needed (this should generally match the name of
     * the associated port).
     */
    virtual bool open(const std::string& name) = 0;

    /**
     * Negotiate an end to operations.
     */
    virtual void close() = 0;

    /**
     * Try to get operations interrupted.
     */
    virtual void interrupt() = 0;

    /**
     * Get the route associated with this connection. A route is
     * a triplet of the source port, destination port, and carrier.
     */
    virtual const Route& getRoute() const = 0;

    /**
     * Begin a read operation, with bytes read via the returned
     * yarp::os::ConnectionReader object.  Be sure to call
     * endRead() when done.
     */
    virtual ConnectionReader& beginRead() = 0;

    /**
     * End the current read operation, begin by beginRead().
     */
    virtual void endRead() = 0;

    /**
     * Reply to a message we have just read.
     */
    virtual void reply(SizedWriter& writer) = 0;

    /**
     * Get the basic connection through which we are communicating.
     */
    virtual Connection& getConnection() = 0;

    /**
     * It is possible to chain a basic connection with a modifier.
     * If there is a modifier, this call returns it, otherwise it
     * will return a connection for which yarp::os::Connection::isValid()
     * returns false.
     */
    virtual Connection& getReceiver() = 0;

    /**
     * Check if the connection is valid and can be used.
     */
    virtual bool isOk() const = 0;

    /**
     * Get an interface for doing write operations on the connection.
     * Not all types of connections will support this, check
     * yarp::os::OutputProtocol::isOk() on the result.
     */
    virtual OutputProtocol& getOutput() = 0;

    /**
     * Make sure that any attempt to send a reply to input will be denied.
     * This is a good thing to do if we know the sender is not expecting
     * a reply.
     */
    virtual void suppressReply() = 0;

    /**
     * Access the output stream associated with the connection.
     */
    virtual OutputStream& getOutputStream() = 0;

    /**
     * Access the input stream associated with the connection.
     */
    virtual InputStream& getInputStream() = 0;

    /**
     * Set the envelope that will be attached to the next message.
     */
    virtual void setEnvelope(const std::string& str) = 0;

    /**
     * Set the timeout to be used for network operations.
     */
    virtual bool setTimeout(double timeout) = 0;

    /**
     * Set the port to be associated with the connection.
     */
    virtual void attachPort(Contactable* port) = 0;

    /**
     * @return true if a reply will be made to a message.
     */
    virtual bool isReplying() const = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_INPUTPROTOCOL_H
