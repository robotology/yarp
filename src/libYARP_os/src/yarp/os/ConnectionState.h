/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONNECTIONSTATE_H
#define YARP_OS_CONNECTIONSTATE_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

class Connection;
class InputStream;
class OutputStream;
class Portable;
class Route;
class TwoWayStream;
class Contactable;

/**
 * The basic state of a connection - route, streams in use, etc.
 */
class YARP_os_API ConnectionState
{
public:
    /**
     * Destructor.
     */
    virtual ~ConnectionState();

    /**
     * Get the route associated with this connection. A route is
     * a triplet of the source port, destination port, and carrier.
     */
    virtual const Route& getRoute() const = 0;

    /**
     * Set the route associated with this connection.
     */
    virtual void setRoute(const Route& route) = 0;

    /**
     * Access the output stream associated with this connection.
     */
    virtual OutputStream& getOutputStream() = 0;

    /**
     * Access the input stream associated with this connection.
     */
    virtual InputStream& getInputStream() = 0;

    /**
     * Access the controller for this connection.
     */
    virtual Connection& getConnection() = 0;

    /**
     * Tell the connection that the given number of
     * bytes are left to be read.  This is useful
     * when there is no low-level way to know this.
     */
    virtual void setRemainingLength(int len) = 0;

    /**
     * Extract a name for the sender, if the connection
     * type supports that.
     */
    virtual std::string getSenderSpecifier() const = 0;

    /**
     * Access the streams associated with the connection.
     * The connection remains the owner of those streams.
     */
    virtual TwoWayStream& getStreams() = 0;

    /**
     * Provide streams to be used with the connection.
     * The connection becomes the owner of these streams.
     * Any streams already in use are closed and destroyed.
     */
    virtual void takeStreams(TwoWayStream* streams) = 0;

    /**
     * Take ownership of the streams associated with
     * the connection.  The connection will never touch
     * them again after this call.
     */
    virtual TwoWayStream* giveStreams() = 0;

    /**
     * Give a direct pointer to an object being sent
     * on the connection.  This allows serialization
     * to be bypassed for local connections.
     */
    virtual void setReference(yarp::os::Portable* ref) = 0;

    /**
     * Check whether streams are in a good state.
     */
    virtual bool checkStreams() const = 0;

    /**
     * Get the port associated with the connection.
     */
    virtual Contactable* getContactable() const = 0;

    /**
     * Shorthand for getOutputStream()
     */
    OutputStream& os()
    {
        return getOutputStream();
    }

    /**
     * Shorthand for getInputStream()
     */
    InputStream& is()
    {
        return getInputStream();
    }

    /**
     * Read the envelope associated with the current message.
     */
    virtual const std::string& getEnvelope() const = 0;
};


} // namespace os
} // namespace yarp

#endif // YARP_OS_CONNECTIONSTATE_H
