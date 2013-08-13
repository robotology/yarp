// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CARRIER_
#define _YARP2_CARRIER_

#include <yarp/os/ShiftStream.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Connection.h>

#define YARP_ENACT_CONNECT 1
#define YARP_ENACT_DISCONNECT 2
#define YARP_ENACT_EXISTS 3


namespace yarp {
    namespace os {
        namespace impl {
            class Carrier;
            class Protocol;
        }
    }
}

/**
 * A base class for connection types (tcp, mcast, shmem, ...) which are
 * called carriers in YARP.
 *
 * The methods prepareSend, sendHeader, expectReplyToHeader,
 * sendIndex, expectAck, and write are important on the originating
 * side of a connection.
 *
 * The methods expectExtraHeader, respondToHeader, expectIndex,
 * expectSenderSpecifier, and sendAck are important on the
 * receiving side of a connection.
 *
 * To understand the protocol phases involved, see see
 * \ref yarp_protocol.
 */
class YARP_OS_impl_API yarp::os::impl::Carrier : public yarp::os::Connection {
public:


    /**
     * Factory method.  Get a new object of the same type as this one.
     *
     * @return a new object of the same type as this one.
     */
    virtual Carrier *create() = 0;

    /**
     * Get the name of this carrier ("tcp", "mcast", "shmem", ...)
     *
     * @return the name of this carrier
     */
    virtual String getName() = 0;

    /**
     * Given the first 8 bytes received on a connection, decide if
     * this is the right carrier type to use for the rest of the
     * connection.
     *
     * For example the "text" carrier looks for the 8-byte sequence
     * "CONNECT ".  See the YARP protocol documentation for all the
     * sequences in use here.  In general you are free to add your own.
     *
     * @param header a buffer holding the first 8 bytes received on the
     *               connection
     * @return true if this is the carrier to use.
     */
    virtual bool checkHeader(const yarp::os::Bytes& header) = 0;

    /**
     * Configure this carrier based on the first 8 bytes of the
     * connection.
     *
     * This will only be called if checkHeader passed.
     *
     * @param header a buffer holding the first 8 bytes received on the
     *               connection
     */
    virtual void setParameters(const yarp::os::Bytes& header) = 0;

    /**
     * Provide 8 bytes describing this connection sufficiently to
     * allow the other side of a connection to select it.
     *
     * @param header a buffer to hold the first 8 bytes to send on a
     *               connection
     */
    virtual void getHeader(const yarp::os::Bytes& header) = 0;


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
    virtual bool isConnectionless() = 0;


    /**
     * Check if this carrier uses a broadcast mechanism.
     *
     * This flag is used to determine whether it is appropriate to send
     * "kill" messages using a carrier or whether they should be sent
     * "out-of-band"
     *
     * @return true if carrier uses a broadcast mechanism.
     */
    virtual bool isBroadcast() {
        return false;
    }

    /**
     * Check if reading is implemented for this carrier.
     *
     * @return true if carrier can read messages
     */
    virtual bool canAccept() = 0;

    /**
     * Check if writing is implemented for this carrier.
     *
     * @return true if carrier can write messages
     */
    virtual bool canOffer() = 0;

    /**
     * Check if carrier is textual in nature
     *
     * @return true if carrier is text-based
     */
    virtual bool isTextMode() = 0;

    /**
     * Check if carrier can encode administrative messages, as opposed
     * to just user data.  The word escape is used in the sense of
     * escape character or escape sequence here.
     *
     * @return true if carrier can encode administrative messages
     */
    virtual bool canEscape() = 0;

    /**
     * Check if carrier has flow control, requiring sent messages
     * to be acknowledged by recipient.
     *
     * @return true if carrier requires acknowledgement.
     */
    virtual bool requireAck() = 0;

    /**
     * This flag is used by YARP to determine whether the connection
     * can carry RPC traffic, that is, messages with replies.
     *
     * @return true if carrier supports replies
     */
    virtual bool supportReply() = 0;

    /**
     * Check if carrier operates within a single process.
     *
     * In such a case, YARP connections may get completely reorganized
     * in order to optimize them.
     *
     * @return true if carrier will only operate within a single process
     */
    virtual bool isLocal() = 0;


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
    virtual bool isPush() {
        return true;
    }

    /**
     * Perform any initialization needed before writing on a connection.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool prepareSend(Protocol& proto) = 0;

    /**
     * Write a header appropriate to the carrier to the connection,
     * followed by any carrier-specific data.
     *
     * Must communicate at least the name of the originating port, if
     * there is one.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool sendHeader(Protocol& proto) = 0;

    /**
     * Process reply to header, if one is expected for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectReplyToHeader(Protocol& proto) = 0;

    /**
     * Write a message.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool write(Protocol& proto, SizedWriter& writer) = 0;


    virtual bool reply(Protocol& proto, SizedWriter& writer);

    /**
     * Receive any carrier-specific header.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectExtraHeader(Protocol& proto) = 0;

    /**
     * Respond to the header.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool respondToHeader(Protocol& proto) = 0;

    /**
     * Expect a message header, if there is one for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectIndex(Protocol& proto) = 0;

    /**
     * Expect the name of the sending port.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectSenderSpecifier(Protocol& proto) = 0;

    /**
     * Send an acknowledgement, if needed for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool sendAck(Protocol& proto) = 0;

    /**
     * Receive an acknowledgement, if expected for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectAck(Protocol& proto) = 0;

    /**
     * Check if carrier is alive and error free.
     *
     * @return true if carrier is active.
     */
    virtual bool isActive() = 0;

    /**
     * Do cleanup and preparation for the coming disconnect, if
     * necessary.
     */
    virtual void prepareDisconnect() {}


    /**
     * Get name of carrier.
     *
     * @return name of carrier.
     */
    virtual String toString() = 0;

    /**
     * Close the carrier.
     */
    virtual void close() {
    }

    /**
     * Destructor.
     */
    virtual ~Carrier() {
    }

    /**
     * Get the name of the carrier that should be used prior to
     * handshaking, if a port is registered with this carrier as its
     * default carrier.
     *
     * Normally, this should be "tcp".
     *
     * For carriers intended to interoperate with foreign ports not
     * capable of initiating streams, the bootstrap carrier name
     * should be left blank.  YARP may use that fact to arrange for
     * alternate methods of initiating a stream.
     *
     * @return the name of the bootstrap carrier.
     */
    virtual String getBootstrapCarrierName() { return "tcp"; }

    /**
     * Some carrier types may require special connection logic.
     * This can be implemented here.  Normally this is not necessary.
     *
     * @param src Initiator of the connection.
     * @param dest Destination of the connection.
     * @param style Options for connection.
     * @param mode YARP_ENACT_CONNECT / YARP_ENACT_DISCONNECT /
     *             YARP_ENACT_EXISTS.
     * @param reversed true for pull connections, false for push
     *                 connections.
     * @result -1 if no attempt made to connect, 0 on success, 1 on
     *         failure.
     */
    virtual int connect(const yarp::os::Contact& src,
                        const yarp::os::Contact& dest,
                        const yarp::os::ContactStyle& style,
                        int mode,
                        bool reversed) {
        return -1;
    }


    /**
     * Check if this carrier modifies incoming data through the
     * Carrier::modifyIncomingData method.
     *
     * @return true if carrier wants Carrier::modifyIncomingData called.
     */
    virtual bool modifiesIncomingData() {
        return false;
    }

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
    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) {
        return reader;
    }

    /**
     * Determine whether incoming data should be accepted.
     *
     * @param reader for incoming data.
     * @return true if data should be accepted, false if it should be
     *         discarded.
     */
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) {
        return true;
    }

    /**
     * Give carrier a shot at looking at how the connection is set up.
     * Currently only called for recv carriers.
     *
     * @return true if the carrier was correctly configured.
     */
    virtual bool configure(yarp::os::impl::Protocol& proto) {
        return true;
    }

    /**
     * Configure carrier from port administrative commands.
     *
     * @param carrier properties
     */
    virtual void setCarrierParams(const yarp::os::Property& params) { }

    /**
     * Get carrier configuration and deliver it by port administrative
     * commands.
     *
     * @param carrier properties
     */
    virtual void getCarrierParams(yarp::os::Property& params) { }

};

#endif
