/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CARRIER_H
#define YARP_OS_CARRIER_H

#include <yarp/os/Connection.h>

#define YARP_ENACT_CONNECT 1
#define YARP_ENACT_DISCONNECT 2
#define YARP_ENACT_EXISTS 3


namespace yarp {
namespace os {

class SizedWriter;
class Bytes;
class Contact;
class ContactStyle;
class ConnectionReader;
class ConnectionState;
class Face;


/**
 * \brief A base class for connection types (tcp, mcast, shmem, ...) which are
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
class YARP_os_API Carrier : public Connection
{
public:
    /**
     * Factory method.  Get a new object of the same type as this one.
     *
     * @return a new object of the same type as this one.
     */
    virtual Carrier* create() const = 0;

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
    virtual bool checkHeader(const Bytes& header) = 0;

    /**
     * Configure this carrier based on the first 8 bytes of the
     * connection.
     *
     * This will only be called if checkHeader passed.
     *
     * @param header a buffer holding the first 8 bytes received on the
     *               connection
     */
    virtual void setParameters(const Bytes& header) = 0;

    /**
     * Provide 8 bytes describing this connection sufficiently to
     * allow the other side of a connection to select it.
     *
     * @param header a buffer to hold the first 8 bytes to send on a
     *               connection
     */
    void getHeader(Bytes& header) const override = 0;


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
    bool isConnectionless() const override = 0;


    /**
     * Check if this carrier uses a broadcast mechanism.
     *
     * This flag is used to determine whether it is appropriate to send
     * "kill" messages using a carrier or whether they should be sent
     * "out-of-band"
     *
     * @return true if carrier uses a broadcast mechanism.
     */
    bool isBroadcast() const override;

    /**
     * Check if reading is implemented for this carrier.
     *
     * @return true if carrier can read messages
     */
    virtual bool canAccept() const = 0;

    /**
     * Check if writing is implemented for this carrier.
     *
     * @return true if carrier can write messages
     */
    virtual bool canOffer() const = 0;

    /**
     * Check if carrier is textual in nature
     *
     * @return true if carrier is text-based
     */
    bool isTextMode() const override = 0;

    /**
     * Check if carrier can encode administrative messages, as opposed
     * to just user data.  The word escape is used in the sense of
     * escape character or escape sequence here.
     *
     * @return true if carrier can encode administrative messages
     */
    bool canEscape() const override = 0;

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
    void handleEnvelope(const std::string& envelope) override;

    /**
     * Check if carrier has flow control, requiring sent messages
     * to be acknowledged by recipient.
     *
     * @return true if carrier requires acknowledgement.
     */
    bool requireAck() const override = 0;

    /**
     * This flag is used by YARP to determine whether the connection
     * can carry RPC traffic, that is, messages with replies.
     *
     * @return true if carrier supports replies
     */
    bool supportReply() const override = 0;

    /**
     * Check if carrier operates within a single process.
     *
     * In such a case, YARP connections may get completely reorganized
     * in order to optimize them.
     *
     * @return true if carrier will only operate within a single process
     */
    bool isLocal() const override = 0;


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
    bool isPush() const override;

    /**
     * Perform any initialization needed before writing on a connection.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool prepareSend(ConnectionState& proto) = 0;

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
    virtual bool sendHeader(ConnectionState& proto) = 0;

    /**
     * Process reply to header, if one is expected for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectReplyToHeader(ConnectionState& proto) = 0;

    /**
     * Write a message.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool write(ConnectionState& proto, SizedWriter& writer) = 0;


    virtual bool reply(ConnectionState& proto, SizedWriter& writer);

    /**
     * Receive any carrier-specific header.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectExtraHeader(ConnectionState& proto) = 0;

    /**
     * Respond to the header.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool respondToHeader(ConnectionState& proto) = 0;

    /**
     * Expect a message header, if there is one for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectIndex(ConnectionState& proto) = 0;

    /**
     * Expect the name of the sending port.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectSenderSpecifier(ConnectionState& proto) = 0;

    /**
     * Send an acknowledgement, if needed for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool sendAck(ConnectionState& proto) = 0;

    /**
     * Receive an acknowledgement, if expected for this carrier.
     *
     * @param proto the protocol object, which tracks connection state
     * @return true on success, false on failure
     */
    virtual bool expectAck(ConnectionState& proto) = 0;

    /**
     * Check if carrier is alive and error free.
     *
     * @return true if carrier is active.
     */
    bool isActive() const override = 0;

    /**
     * Do cleanup and preparation for the coming disconnect, if
     * necessary.
     */
    void prepareDisconnect() override;


    /**
     * Get name of carrier.
     *
     * @return name of carrier.
     */
    virtual std::string toString() const = 0;

    /**
     * Close the carrier.
     */
    virtual void close();

    /**
     * Destructor.
     */
    virtual ~Carrier();

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
    virtual std::string getBootstrapCarrierName() const;

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
    virtual int connect(const Contact& src,
                        const Contact& dest,
                        const ContactStyle& style,
                        int mode,
                        bool reversed);


    /**
     * Check if this carrier modifies incoming data through the
     * Carrier::modifyIncomingData method.
     *
     * @return true if carrier wants Carrier::modifyIncomingData called.
     */
    bool modifiesIncomingData() const override;

    /**
     * Modify incoming payload data, if appropriate.
     *
     * Doesn't need to be done immediately, it is fine to hold onto a
     * reference to the incoming data reader and use it on demand.
     * This can be handy in order to avoid unnecessary copies.
     *
     * @param reader for incoming data.
     * @return reader for modified version of incoming data.
     *
     * @note If the ConnectionReader returned is not the same as the one in
     *       input, the setParentConnectionReader(&reader) should be called for
     *       the new one, or the envelope will not be handled correctly.
     */
    ConnectionReader& modifyIncomingData(ConnectionReader& reader) override;

    /**
     * Determine whether incoming data should be accepted.
     *
     * @param reader for incoming data.
     * @return true if data should be accepted, false if it should be
     *         discarded.
     */
    bool acceptIncomingData(ConnectionReader& reader) override;

    /**
     * Check if this carrier modifies outgoing data through the
     * Carrier::modifyOutgoingData method.
     *
     * @return true if carrier wants Carrier::modifyOutgoingData called.
     */
    bool modifiesOutgoingData() const override;

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
    const PortWriter& modifyOutgoingData(const PortWriter& writer) override;

    /**
     * Check if this carrier modifies outgoing data through the
     * Carrier::modifyReply method.
     *
     * @return true if carrier wants Carrier::modifyReply called.
     */
    bool modifiesReply() const override;

    /**
     * Modify reply payload data, if appropriate.
     *
     * @param reader for the replied message.
     * @return reader for modified version of the replied message.
     */
    PortReader& modifyReply(PortReader& reader) override;

    /**
     * Determine whether outgoing data should be accepted.
     *
     * @param writer for outgoing data.
     * @return true if data should be accepted, false if it should be
     *         discarded.
     */
    bool acceptOutgoingData(const PortWriter& writer) override;

    /**
     * Give carrier a shot at looking at how the connection is set up.
     *
     * @return true if the carrier was correctly configured.
     */
    virtual bool configure(ConnectionState& proto);

    virtual bool configureFromProperty(yarp::os::Property& options);

    /**
     * Configure carrier from port administrative commands.
     *
     * @param params properties
     */
    void setCarrierParams(const Property& params) override;
    /**
     * Get carrier configuration and deliver it by port administrative
     * commands.
     *
     * @param params properties
     */
    void getCarrierParams(Property& params) const override;

    /**
     * Create new Face object that the carrier needs.
     *
     */
    virtual yarp::os::Face* createFace() const;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CARRIER_H
