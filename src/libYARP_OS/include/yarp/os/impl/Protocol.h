/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PROTOCOL_H
#define YARP_OS_IMPL_PROTOCOL_H

#include <yarp/os/Carrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/ShiftStream.h>
#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionState.h>
#include <cstdio>
#include <cstdlib>

namespace yarp {
    namespace os {
        namespace impl {
            class Protocol;
        }
    }
}

/**
 * Connection choreographer.  Handles one side of a single YARP connection.
 * The Protocol object for a connection holds its streams (which may
 * change over time) and its carriers (which may be chained).
 */
class YARP_OS_impl_API yarp::os::impl::Protocol : public yarp::os::OutputProtocol,
                                                  public yarp::os::InputProtocol,
                                                  public yarp::os::ConnectionState
{
public:

    /**
     * Constructor. The Protocol object becomes the owner of
     * the provided stream, and will destroy it at some point.
     */
    Protocol(TwoWayStream *stream);

    /**
     *
     * Destructor.
     *
     */
    virtual ~Protocol()
    {
        closeHelper();
    }

    // Documented in yarp::os::ConnectionState.
    virtual void setRoute(const Route& route) override;

    // Documented in yarp::os::ConnectionState.
    virtual const Route& getRoute() override
    {
        return route;
    }

    // Documented in yarp::os::InputProtocol.
    void interrupt() override;

    // Documented in yarp::os::InputProtocol.
    void close() override
    {
        closeHelper();
    }

    // Documented in yarp::os::ConnectionState.
    TwoWayStream& getStreams() override
    {
        return shift;
    }

    // Documented in yarp::os::ConnectionState.
    void takeStreams(TwoWayStream *streams) override
    {
        shift.takeStream(streams);
        if (streams!=nullptr) {
            active = true;
        }
    }

    // Documented in yarp::os::ConnectionState.
    TwoWayStream *giveStreams() override
    {
        return shift.giveStream();
    }

    // Documented in yarp::os::InputProtocol.
    OutputStream& getOutputStream() override
    {
        return shift.getOutputStream();
    }

    // Documented in yarp::os::InputProtocol.
    InputStream& getInputStream() override
    {
        return shift.getInputStream();
    }

    // Documented in yarp::os::OutputProtocol.
    virtual bool open(const Route& route) override;

    // Documented in yarp::os::OutputProtocol.
    virtual void rename(const Route& route) override
    {
        setRoute(route);
    }

    // Documented in yarp::os::InputProtocol.
    virtual bool open(const ConstString& name) override;

    // Documented in yarp::os::OutputProtocol.
    virtual bool isOk() override
    {
        if (!checkStreams() || recv_delegate_fail || recv_delegate_fail) {
            return false;
        }
        return true;
    }

    // Documented in yarp::os::OutputProtocol.
    virtual bool write(SizedWriter& writer) override;

    // Documented in yarp::os::InputProtocol.
    void reply(SizedWriter& writer) override
    {
        writer.stopWrite();
        delegate->reply(*this, writer);
        pendingReply = false;
    }

    // Documented in yarp::os::InputProtocol.
    virtual OutputProtocol& getOutput() override
    {
        return *this;
    }

    // Documented in yarp::os::OutputProtocol.
    virtual InputProtocol& getInput() override
    {
        return *this;
    }

    // Documented in yarp::os::InputProtocol.
    virtual yarp::os::ConnectionReader& beginRead() override;

    // Documented in yarp::os::InputProtocol.
    virtual void endRead() override
    {
        reader.flushWriter();
        sendAck();  // acknowledge after reply (if there is one)
    }

    // Documented in yarp::os::OutputProtocol.
    virtual void beginWrite() override
    {
        getSendDelegate();
    }

    // Documented in yarp::os::InputProtocol.
    virtual void suppressReply() override
    {
        reader.suppressReply();
    }

    // Documented in yarp::os::ConnectionState.
    virtual bool checkStreams() override
    {
        return shift.isOk();
    }

    // Documented in yarp::os::ConnectionState.
    void setReference(yarp::os::Portable *ref) override
    {
        this->ref = ref;
    }

    // Documented in yarp::os::ConnectionState.
    yarp::os::ConstString getSenderSpecifier() override;

    // Documented in yarp::os::InputProtocol.
    virtual bool setTimeout(double timeout) override
    {
        bool ok = os().setWriteTimeout(timeout);
        if (!ok) return false;
        return is().setReadTimeout(timeout);
    }

    // Documented in yarp::os::InputProtocol.
    virtual void setEnvelope(const yarp::os::ConstString& str) override
    {
        envelope = str;
    }

    // Documented in yarp::os::ConnectionState.
    virtual const ConstString& getEnvelope() override
    {
        return envelope;
    }

    // Documented in yarp::os::ConnectionState.
    Log& getLog() override
    {
        return log;
    }

    // Documented in yarp::os::ConnectionState.
    void setRemainingLength(int len) override
    {
        messageLen = len;
    }

    // Documented in yarp::os::ConnectionState.
    Connection& getConnection() override
    {
        if (delegate==nullptr) {
            return nullConnection;
        }
        return *delegate;
    }

    // Documented in yarp::os::InputProtocol.
    Connection& getReceiver() override
    {
        if (recv_delegate==nullptr) {
            return nullConnection;
        }
        return *recv_delegate;
    }

    // Documented in yarp::os::OutputProtocol.
    Connection& getSender() override
    {
        if (send_delegate==nullptr) {
            return nullConnection;
        }
        return *send_delegate;
    }

    // Documented in yarp::os::InputProtocol.
    virtual void attachPort(yarp::os::Contactable *port) override
    {
        this->port = port;
    }

    // Documented in yarp::os::ConnectionState.
    virtual Contactable *getContactable() override
    {
        return port;
    }

    /**
     *
     * Promise that we'll be making a reply.
     *
     */
    void willReply()
    {
        pendingReply = true;
    }


    // Documented in yarp::os::InputProtocol.
    virtual bool isReplying() override
    {
        return pendingReply;
    }

private:

    /**
     *
     * Scan for a receiver modifier in the carrier options, and
     * cache the result.
     *
     */
    bool getRecvDelegate();

    /**
     *
     * Scan for a sender modifier in the carrier options, and
     * cache the result.
     *
     */
    bool getSendDelegate();

    /**
     *
     * Default implementation for reading what protocol we should
     * be using.
     *
     */
    bool expectProtocolSpecifier();


    /**
     *
     * Read the name of the port on the other side of the connection.
     *
     */
    bool expectSenderSpecifier()
    {
        yAssert(delegate!=nullptr);
        return delegate->expectSenderSpecifier(*this);
    }

    /**
     *
     * Read the various parts of a connection header - the protocol
     * to use, the name of the port on the other side, and any
     * carrier-specific material.  These days, all of these parts
     * may be redefined.
     *
     */
    bool expectHeader();

    /**
     *
     * Switch to a named carrier.  May involve switching to a different
     * kind of network.
     *
     */
    void setCarrier(const ConstString& carrierNameBase);

    /**
     *
     * Send the various parts of a connection header.
     *
     */
    bool sendHeader()
    {
        yAssert(delegate!=nullptr);
        return delegate->sendHeader(*this);
    }

    /**
     *
     * Read a reply/acknowledgement to the header.  This is often a
     * no-op.
     *
     */
    bool expectReplyToHeader()
    {
        yAssert(delegate!=nullptr);
        return delegate->expectReplyToHeader(*this);
    }

    /**
     *
     * Send a reply/acknowledgement to a header.  This is often a
     * no-op.
     *
     */
    bool respondToHeader();

    /**
     *
     * Individual messages may have an index at the beginning.
     * They did a decade ago with YARP1, these days it is
     * entirely unnecessary.  It was just a workaround for
     * some limits in the size of QNX buffers.  New carriers
     * can leave the index classbacks as no-ops.
     *
     */
    bool expectIndex();

    /**
     *
     * In principle we could respond to receipt of a message index.
     * We never have and we never will, but there's a callback for
     * it.
     *
     */
    bool respondToIndex()
    {
        return true;
    }

    /**
     *
     * After sending a message, wait for an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     *
     */
    bool expectAck();

    /**
     *
     * After receiving a message, send an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     *
     */
    bool sendAck();

    /**
     *
     * Close things down - our streams, our carriers.
     *
     */
    void closeHelper();

    int messageLen;   ///< length remaining in current message (if known)
    bool pendingAck;  ///< is an acknowledgement due
    Logger& log;      ///< connection-specific logger
    ShiftStream shift;///< input and output streams
    bool active;      ///< is the connection up and running
    Carrier *delegate;       ///< main carrier specifying behavior of connection
    Carrier *recv_delegate;  ///< modifier for incoming messages
    Carrier *send_delegate;  ///< modifier for outgoing messages
    bool need_recv_delegate; ///< turns false once we've cached recv modifier
    bool need_send_delegate; ///< turns false once we've cached send modifier
    bool recv_delegate_fail; ///< turns true if recv modifier could not be cached
    bool send_delegate_fail; ///< turns true if send modifier could not be cached
    Route route;             ///< names of (sender, carrier, receiver) triplet
    SizedWriter *writer;     ///< writer for current message
    StreamConnectionReader reader;  ///< reader for incoming messages
    yarp::os::Portable *ref; ///< source for current message, so we can
                             ///< bypass serialization on local connections
    ConstString envelope;         ///< envelope for current message
    NullConnection nullConnection; ///< dummy connection
    yarp::os::Contactable *port;   ///< port associated with this connection
    bool pendingReply;  ///< will we be making a reply
};

#endif // YARP_OS_IMPL_PROTOCOL_H
