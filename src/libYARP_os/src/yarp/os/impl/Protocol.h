/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PROTOCOL_H
#define YARP_OS_IMPL_PROTOCOL_H

#include <yarp/os/Carrier.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/InputProtocol.h>
#include <yarp/os/NullConnection.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/ShiftStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Connection choreographer.  Handles one side of a single YARP connection.
 * The Protocol object for a connection holds its streams (which may
 * change over time) and its carriers (which may be chained).
 */
class YARP_os_impl_API Protocol :
        public yarp::os::OutputProtocol,
        public yarp::os::InputProtocol,
        public yarp::os::ConnectionState
{
public:
    /**
     * Constructor. The Protocol object becomes the owner of
     * the provided stream, and will destroy it at some point.
     */
    Protocol(TwoWayStream* stream);

    /**
     * Destructor.
     */
     ~Protocol() override;

    // yarp::os::ConnectionState.
    void setRoute(const Route& route) override;
    const Route& getRoute() const override;
    TwoWayStream& getStreams() override;
    void takeStreams(TwoWayStream* streams) override;
    TwoWayStream* giveStreams() override;
    bool checkStreams() const override;
    void setReference(yarp::os::Portable* ref) override;
    std::string getSenderSpecifier() const override;
    const std::string& getEnvelope() const override;
    void setRemainingLength(int len) override;
    Connection& getConnection() override;
    Contactable* getContactable() const override;


    // yarp::os::InputProtocol.
    bool open(const std::string& name) override;
    void close() override;
    void interrupt() override;
    OutputStream& getOutputStream() override;
    InputStream& getInputStream() override;
    void reply(SizedWriter& writer) override;
    OutputProtocol& getOutput() override;
    yarp::os::ConnectionReader& beginRead() override;
    void endRead() override;
    void suppressReply() override;
    bool setTimeout(double timeout) override;
    void setEnvelope(const std::string& str) override;
    Connection& getReceiver() override;
    void attachPort(yarp::os::Contactable* port) override;
    bool isReplying() const override;


    // yarp::os::OutputProtocol.
    bool open(const Route& route) override;
    void rename(const Route& route) override;
    bool isOk() const override;
    bool write(SizedWriter& writer) override;
    InputProtocol& getInput() override;
    void beginWrite() override;
    Connection& getSender() override;


    /**
     * Promise that we'll be making a reply.
     */
    void willReply()
    {
        pendingReply = true;
    }

private:
    /**
     * Scan for a receiver modifier in the carrier options, and
     * cache the result.
     */
    bool getRecvDelegate();

    /**
     * Scan for a sender modifier in the carrier options, and
     * cache the result.
     */
    bool getSendDelegate();

    /**
     * Default implementation for reading what protocol we should
     * be using.
     */
    bool expectProtocolSpecifier();

    /**
     * Read the name of the port on the other side of the connection.
     */
    bool expectSenderSpecifier();

    /**
     * Read the various parts of a connection header - the protocol
     * to use, the name of the port on the other side, and any
     * carrier-specific material.  These days, all of these parts
     * may be redefined.
     */
    bool expectHeader();

    /**
     * Switch to a named carrier.  May involve switching to a different
     * kind of network.
     */
    void setCarrier(const std::string& carrierNameBase);

    /**
     * Send the various parts of a connection header.
     */
    bool sendHeader();

    /**
     * Read a reply/acknowledgement to the header.  This is often a
     * no-op.
     */
    bool expectReplyToHeader();

    /**
     * Send a reply/acknowledgement to a header.  This is often a
     * no-op.
     */
    bool respondToHeader();

    /**
     * Individual messages may have an index at the beginning.
     * They did a decade ago with YARP1, these days it is
     * entirely unnecessary.  It was just a workaround for
     * some limits in the size of QNX buffers.  New carriers
     * can leave the index classbacks as no-ops.
     */
    bool expectIndex();

    /**
     * In principle we could respond to receipt of a message index.
     * We never have and we never will, but there's a callback for
     * it.
     */
    bool respondToIndex();

    /**
     * After sending a message, wait for an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     */
    bool expectAck();

    /**
     * After receiving a message, send an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     */
    bool sendAck();

    /**
     * Close things down - our streams, our carriers.
     */
    void closeHelper();

    int messageLen;                ///< length remaining in current message (if known)
    bool pendingAck;               ///< is an acknowledgement due
    ShiftStream shift;             ///< input and output streams
    bool active;                   ///< is the connection up and running
    Carrier* delegate;             ///< main carrier specifying behavior of connection
    Carrier* recv_delegate;        ///< modifier for incoming messages
    Carrier* send_delegate;        ///< modifier for outgoing messages
    bool need_recv_delegate;       ///< turns false once we've cached recv modifier
    bool need_send_delegate;       ///< turns false once we've cached send modifier
    bool recv_delegate_fail;       ///< turns true if recv modifier could not be cached
    bool send_delegate_fail;       ///< turns true if send modifier could not be cached
    Route route;                   ///< names of (sender, carrier, receiver) triplet
    SizedWriter* writer;           ///< writer for current message
    StreamConnectionReader reader; ///< reader for incoming messages
    yarp::os::Portable* ref;       ///< source for current message, so we can bypass serialization on local connections
    std::string envelope;          ///< envelope for current message
    NullConnection nullConnection; ///< dummy connection
    yarp::os::Contactable* port;   ///< port associated with this connection
    bool pendingReply;             ///< will we be making a reply
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PROTOCOL_H
