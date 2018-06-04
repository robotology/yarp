/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_CONNECTION_H
#define YARP_OS_CONNECTION_H

#include <yarp/os/api.h>
#include <yarp/os/Bytes.h>

namespace yarp {
    namespace os {
        class Connection;
        class NullConnection;
    }
}

/**
 *
 * A controller for an individual connection.  A bundle of callbacks
 * that defines how the connection behaves.
 *
 */
class YARP_OS_API yarp::os::Connection
{
public:

    /**
     * Destructor.
     */
    virtual ~Connection() {}

    /**
     *
     * Check if this object is really a connection, or just
     * an empty placeholder
     *
     * @return true if a valid connection
     *
     */
    virtual bool isValid() const { return true; }

    /**
     * Check if carrier is textual in nature
     *
     * @return true if carrier is text-based
     */
    virtual bool isTextMode() const = 0;

    /**
     * Check if carrier excludes type information from payload
     *
     * @return true if carrier is bare
     */
    virtual bool isBareMode() const { return false; }

    /**
     * Carriers that do not distinguish data from administrative headers
     * (i.e. canEscape returns false), can overload this method to
     * handle the envelope inside the stream.
     * On the receiving side, the InputStream will have to overload the
     * yarp::os::InputStream::setReadEnvelopeCallback method, and
     * execute the callback as soon as the envelope is ready.
     *
     * @param envelope the envelope to transmit bundled with data.
     */
    virtual void handleEnvelope(const std::string& envelope) = 0;


    /**
     * Check if carrier can encode administrative messages, as opposed
     * to just user data.  The word escape is used in the sense of
     * escape character or escape sequence here.
     *
     * @return true if carrier can encode administrative messages
     */
    virtual bool canEscape() const = 0;

    /**
     * Check if carrier has flow control, requiring sent messages
     * to be acknowledged by recipient.
     *
     * @return true if carrier requires acknowledgement.
     */
    virtual bool requireAck() const = 0;

    /**
     * This flag is used by YARP to determine whether the connection
     * can carry RPC traffic, that is, messages with replies.
     *
     * @return true if carrier supports replies
     */
    virtual bool supportReply() const = 0;

    /**
     * Check if carrier operates within a single process.
     *
     * In such a case, YARP connections may get completely reorganized
     * in order to optimize them.
     *
     * @return true if carrier will only operate within a single process
     */
    virtual bool isLocal() const = 0;


    /**
     * Check if carrier is "push" or "pull" style.
     *
     * Push means that the side that initiates a connection is also the
     * one that will sending of data or commands.  All native YARP
     * connections are like this.
     * A "pull" style is equivalent to automatically sending a "reverse
     * connection" port command at the start of the connection.
     *
     * @return true if carrier is "push" style, false if "pull" style
     */
    virtual bool isPush() const = 0;

   /**
     * Check if this carrier is connectionless (like udp, mcast) or
     * connection based (like tcp).
     *
     * This flag is used by YARP to determine whether the connection can
     * carry RPC traffic, that is, messages with replies.  Replies are
     * not supported by YARP on connectionless carriers.
     *
     * @return true if carrier is connectionless
     */
    virtual bool isConnectionless() const = 0;


    /**
     * Check if this carrier uses a broadcast mechanism.
     *
     * This flag is used to determine whether it is appropriate to send
     * "kill" messages using a carrier or whether they should be sent
     * "out-of-band"
     *
     * @return true if carrier uses a broadcast mechanism.
     */
    virtual bool isBroadcast() = 0;


    /**
     * Check if carrier is alive and error free.
     *
     * @return true if carrier is active.
     */
    virtual bool isActive() const = 0;


    /**
     * Check if this carrier modifies incoming data through the
     * Carrier::modifyIncomingData method.
     *
     * @return true if carrier wants Carrier::modifyIncomingData called.
     */
    virtual bool modifiesIncomingData() = 0;

    /**
     * Modify incoming payload data, if appropriate.
     *
     * Doesn't need to be done immediately, it is fine to hold onto a
     * reference to the incoming data reader and use it on demand.
     * This can be handy in order to avoid unnecessary copies.
     *
     * @param reader for incoming data.
     * @return reader for modified version of incoming data.
     */
    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) = 0;

    /**
     * Determine whether incoming data should be accepted.
     *
     * @param reader for incoming data.
     * @return true if data should be accepted, false if it should be
     *         discarded.
     */
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) = 0;

    /**
     * Check if this carrier modifies outgoing data through the
     * Carrier::modifyOutgoingData method.
     *
     * @return true if carrier wants Carrier::modifyOutgoingData called.
     */
    virtual bool modifiesOutgoingData() = 0;

    /**
     * Modify outgoing payload data, if appropriate.
     *
     * Doesn't need to be done immediately, it is fine to hold onto a
     * reference to the outgoing data reader and use it on demand.
     * This can be handy in order to avoid unnecessary copies.
     *
     * @param writer for outgoing data.
     * @return writer for modified version of outgoing data.
     */
    virtual PortWriter& modifyOutgoingData(PortWriter& writer) = 0;

    /**
     * Determine whether outgoing data should be accepted.
     *
     * @param writer for outgoing data.
     * @return true if data should be accepted, false if it should be
     *         discarded.
     */
    virtual bool acceptOutgoingData(PortWriter& writer) = 0;

    /**
     * Check if this carrier modifies outgoing data through the
     * Carrier::modifyReply method.
     *
     * @return true if carrier wants Carrier::modifyReply called.
     */
    virtual bool modifiesReply() = 0;

    /**
     * Modify reply payload data, if appropriate.
     *
     * @param reader for the replied message.
     * @return reader for modified version of the replied message.
     */
    virtual PortReader& modifyReply(PortReader& reader) = 0;

    /**
     * Configure carrier from port administrative commands.
     *
     * @param params carrier properties
     */
    virtual void setCarrierParams(const yarp::os::Property& params) = 0;

    /**
     * Get carrier configuration and deliver it by port administrative
     * commands.
     *
     * @param params output carrier properties
     */
    virtual void getCarrierParams(yarp::os::Property& params) = 0;

    /**
     * Provide 8 bytes describing this connection sufficiently to
     * allow the other side of a connection to select it.
     *
     * @param header a buffer to hold the first 8 bytes to send on a
     *               connection
     */
    virtual void getHeader(const yarp::os::Bytes& header) = 0;

    /**
     * Do cleanup and preparation for the coming disconnect, if
     * necessary.
     */
    virtual void prepareDisconnect() = 0;

    /**
     * Get the name of this connection type ("tcp", "mcast", "shmem", ...)
     *
     * @return the name of this connection type
     */
    virtual std::string getName() const = 0;
};


class YARP_OS_API yarp::os::NullConnection : public Connection
{
public:
    virtual bool isValid() const override { return false; }
    virtual bool isTextMode() const override { return true; }
    virtual bool canEscape() const override { return true; }
    virtual void handleEnvelope(const std::string& envelope) override { YARP_UNUSED(envelope); }
    virtual bool requireAck() const override { return false; }
    virtual bool supportReply() const override { return false; }
    virtual bool isLocal() const override { return false; }
    virtual bool isPush() const override { return true; }
    virtual bool isConnectionless() const override { return false; }
    virtual bool isBroadcast() override { return false; }
    virtual bool isActive() const override { return false; }

    virtual bool modifiesIncomingData() override { return false; }

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override
    {
        return reader;
    }

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) override
    {
        YARP_UNUSED(reader);
        return true;
    }

    virtual bool modifiesOutgoingData() override
    {
        return false;
    }

    virtual PortWriter& modifyOutgoingData(PortWriter& writer) override
    {
        return writer;
    }

    virtual bool acceptOutgoingData(PortWriter& writer) override
    {
        YARP_UNUSED(writer);
        return true;
    }

    virtual bool modifiesReply() override
    {
        return false;
    }

    virtual PortReader& modifyReply(PortReader& reader) override
    {
        return reader;
    }

    virtual void setCarrierParams(const yarp::os::Property& params) override
    {
        YARP_UNUSED(params);
    }

    virtual void getCarrierParams(yarp::os::Property& params) override
    {
        YARP_UNUSED(params);
    }

    virtual void getHeader(const yarp::os::Bytes& header) override
    {
        for (size_t i=0; i<header.length(); i++) {
            header.get()[i] = '\0';
        }
    }

    virtual void prepareDisconnect() override {}

    virtual std::string getName() const override
    {
        return "null";
    }
};

#endif // YARP_OS_CONNECTION_H
