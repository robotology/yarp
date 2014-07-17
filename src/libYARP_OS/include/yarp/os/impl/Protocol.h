// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PROTOCOL_
#define _YARP2_PROTOCOL_

#include <yarp/os/Carrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/NetType.h>
#include <yarp/os/ShiftStream.h>
#include <yarp/os/Portable.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Protocol;
        }
    }
}

/**
 * Connection choreographer.  Handles one side of a single YARP connection.
 */
class YARP_OS_impl_API yarp::os::impl::Protocol : public yarp::os::OutputProtocol, public yarp::os::InputProtocol, public yarp::os::ConnectionState {
public:

    /**
     * Constructor. The Protocol object becomes the owner of 
     * the provided stream, and will destroy it at some point.
     */
    Protocol(TwoWayStream *stream) :
        log(Logger::get()) {
        shift.takeStream(stream);
        active = true;
        route = Route("null","null","tcp");
        delegate = NULL;
        recv_delegate = NULL;
        send_delegate = NULL;
        need_recv_delegate = false;
        need_send_delegate = false;
        messageLen = 0;
        pendingAck = false;
        writer = NULL;
        ref = NULL;
        reader.setProtocol(this);
        envelope = "";
        port = NULL;
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~Protocol() {
        closeHelper();
    }

    // Documented in yarp::os::ConnectionState.
    virtual void setRoute(const Route& route);

    // Documented in yarp::os::ConnectionState.
    virtual const Route& getRoute() {
        return route;
    }

    // Documented in yarp::os::InputProtocol.
    void interrupt() {
        if (active) {
            if (pendingAck) {
                sendAck();
            }
            shift.getInputStream().interrupt();
            active = false;
        }
    }

    // Documented in yarp::os::InputProtocol.
    void close() {
        closeHelper();
    }

    // Documented in yarp::os::ConnectionState.
    TwoWayStream& getStreams() {
        return shift;
    }

    // Documented in yarp::os::ConnectionState.
    void takeStreams(TwoWayStream *streams) {
        shift.takeStream(streams);
        if (streams!=NULL) {
            active = true;
        }
    }

    // Documented in yarp::os::ConnectionState.
    TwoWayStream *giveStreams() {
        return shift.giveStream();
    }

    // Documented in yarp::os::InputProtocol.
    OutputStream& getOutputStream() {
        return shift.getOutputStream();
    }

    // Documented in yarp::os::InputProtocol.
    InputStream& getInputStream() {
        return shift.getInputStream();
    }

    // Documented in yarp::os::OutputProtocol.
    virtual bool open(const Route& route);

    // Documented in yarp::os::OutputProtocol.
    virtual void rename(const Route& route) {
        setRoute(route);
    }

    // Documented in yarp::os::InputProtocol.
    virtual bool open(const ConstString& name) {
        if (name=="") {
            setCarrier("text");
            if (delegate!=NULL) {
                return false;
            }
            setRoute(Route("no-name","no-name","no-carrier"));
        } else {
            setRoute(getRoute().addToName(name));
            bool ok = expectHeader();
            if (!ok) return false;
            return respondToHeader();
        }
        return true;
    }

    // Documented in yarp::os::OutputProtocol.
    virtual bool isOk() {
        return checkStreams();
    }

    // Documented in yarp::os::OutputProtocol.
    virtual bool write(SizedWriter& writer) {
        bool replied = false;
        writer.stopWrite();
        this->writer = &writer;
        if (getConnection().isActive()) {
            YARP_ASSERT(delegate!=NULL);
            getStreams().beginPacket();
            bool ok = delegate->write(*this,writer);
            getStreams().endPacket();
            PortReader *reply = writer.getReplyHandler();
            if (reply!=NULL) {
                if (!delegate->supportReply()) {
                    YARP_INFO(log,String("connection ") + getRoute().toString() + " does not support replies (try \"tcp\" or \"text_ack\")");
                }
                if (ok) {
                    reader.reset(is(),&getStreams(), getRoute(),
                                 messageLen,delegate->isTextMode(),
                                 delegate->isBareMode());
                    replied = reply->read(reader);
                }
            }
            expectAck(); //MOVE ack to after reply, if present
        }
        this->writer = NULL;
        return replied;
    }

    // Documented in yarp::os::InputProtocol.
    void reply(SizedWriter& writer) {
        writer.stopWrite();
        delegate->reply(*this,writer);
    }

    // Documented in yarp::os::InputProtocol.
    virtual OutputProtocol& getOutput() {
        return *this;
    }

    // Documented in yarp::os::OutputProtocol.
    virtual InputProtocol& getInput() {
        return *this;
    }

    // Documented in yarp::os::InputProtocol.
    virtual yarp::os::ConnectionReader& beginRead() {
        getRecvDelegate();
        if (delegate!=NULL) {
            bool ok = false;
            while (!ok) {
                ok = expectIndex();
                if (!ok) {
                    if (!is().isOk()) {
                        // go ahead, we'll be shutting down...
                        ok = true;
                    }
                }
            }
            respondToIndex();
        }
        return reader;
    }

    // Documented in yarp::os::InputProtocol.
    virtual void endRead() {
        reader.flushWriter();
        sendAck();  // acknowledge after reply (if there is one)
    }

    // Documented in yarp::os::OutputProtocol.
    virtual void beginWrite() {
        getSendDelegate();
    }

    // Documented in yarp::os::InputProtocol.
    virtual void suppressReply() {
        reader.suppressReply();
    }

    // Documented in yarp::os::ConnectionState.
    virtual bool checkStreams() {
        return shift.isOk();
    }

    // Documented in yarp::os::ConnectionState.
    void setReference(yarp::os::Portable *ref) {
        this->ref = ref;
    }

    // Documented in yarp::os::ConnectionState.
    yarp::os::ConstString getSenderSpecifier();

    // Documented in yarp::os::InputProtocol.
    virtual bool setTimeout(double timeout) {
        bool ok = os().setWriteTimeout(timeout);
        if (!ok) return false;
        return is().setReadTimeout(timeout);
    }

    // Documented in yarp::os::InputProtocol.
    virtual void setEnvelope(const yarp::os::ConstString& str) {
        envelope = str;
    }

    // Documented in yarp::os::ConnectionState.
    virtual const ConstString& getEnvelope() {
        return envelope;
    }

    // Documented in yarp::os::ConnectionState.
    Log& getLog() {
        return log;
    }

    // Documented in yarp::os::ConnectionState.
    void setRemainingLength(int len) {
        messageLen = len;
    }

    // Documented in yarp::os::ConnectionState.
    Connection& getConnection() {
        if (delegate==NULL) {
            return nullConnection;
        }
        return *delegate;
    }

    // Documented in yarp::os::InputProtocol.
    Connection& getReceiver() {
        if (recv_delegate==NULL) {
            return nullConnection;
        }
        return *recv_delegate;
    }

    // Documented in yarp::os::OutputProtocol.
    Connection& getSender() {
        if (send_delegate==NULL) {
            return nullConnection;
        }
        return *send_delegate;
    }

    // Documented in yarp::os::InputProtocol.
    virtual void attachPort(yarp::os::Contactable *port) {
        this->port = port;
    }

    // Documented in yarp::os::ConnectionState.
    virtual Contactable *getContactable() {
        return port;
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
     * be using.  These defaults go way back to YARP1, maybe a decade ago?
     *
     */
    bool expectProtocolSpecifier() {
        char buf[8];
        yarp::os::Bytes header((char*)&buf[0],sizeof(buf));
        YARP_SSIZE_T len = is().readFull(header);
        if (len==-1) {
            YARP_DEBUG(log,"no connection");
            return false;
        }
        if((size_t)len!=header.length()) {
            YARP_DEBUG(log,"data stream died");
            return false;
        }
        bool already = false;
        if (delegate!=NULL) {
            if (delegate->checkHeader(header)) {
                already = true;
            }
        }
        if (!already) {
            delegate = Carriers::chooseCarrier(header);
            if (delegate==NULL) {
                // carrier not found; send a message
                String msg = "* Error. Protocol not found.\r\n* Hello. You appear to be trying to communicate with a YARP Port.\r\n* The first 8 bytes sent to a YARP Port are critical for identifying the\r\n* protocol you wish to speak.\r\n* The first 8 bytes you sent were not associated with any particular protocol.\r\n* If you are a human, try typing \"CONNECT foo\" followed by a <RETURN>.\r\n* The 8 bytes \"CONNECT \" correspond to a simple text-mode protocol.\r\n* Goodbye.\r\n";
                yarp::os::Bytes b((char*)msg.c_str(),msg.length());
                os().write(b);
                os().flush();
            }
        }
        if (delegate==NULL) {
            YARP_DEBUG(log,"unrecognized protocol");
            return false;
        }
        setRoute(getRoute().addCarrierName(delegate->getName()));
        delegate->setParameters(header);
        return true;
    }


    /**
     *
     * Read the name of the port on the other side of the connection.
     *
     */
    bool expectSenderSpecifier() {
        YARP_ASSERT(delegate!=NULL);
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
    bool expectHeader() {
        messageLen = 0;
        bool ok = expectProtocolSpecifier();
        if (!ok) return false;
        ok = expectSenderSpecifier();
        if (!ok) return false;
        YARP_ASSERT(delegate!=NULL);
        ok = delegate->expectExtraHeader(*this);
        return ok;
    }

    /**
     *
     * Switch to a named carrier.  May involve switching to a different
     * kind of network.
     *
     */
    void setCarrier(const String& carrierNameBase) {
        String carrierName = carrierNameBase;
        if (carrierNameBase=="") carrierName = "tcp";
        setRoute(getRoute().addCarrierName(carrierName));
        if (delegate==NULL) {
            delegate = Carriers::chooseCarrier(carrierName);
            if (delegate!=NULL) {
                if (delegate->modifiesIncomingData()) {
                    if (active) {
                        fprintf(stderr,"Carrier \"%s\" cannot be used this way, try \"tcp+recv.%s\" instead.\n",carrierName.c_str(),carrierName.c_str());
                    }
                    close();
                    return;
                }
                delegate->prepareSend(*this);
            }
        }
    }

    /**
     *
     * Send the various parts of a connection header.
     *
     */
    bool sendHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->sendHeader(*this);
    }

    /**
     *
     * Read a reply/acknowledgement to the header.  This is often a
     * no-op.
     *
     */
    bool expectReplyToHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->expectReplyToHeader(*this);
    }

    /**
     *
     * Send a reply/acknowledgement to a header.  This is often a
     * no-op.
     *
     */
    bool respondToHeader() {
        YARP_ASSERT(delegate!=NULL);
        bool ok = delegate->respondToHeader(*this);
        if (!ok) return false;
        os().flush();
        return os().isOk();
    }

    /**
     *
     * Individual messages may have an index at the beginning.
     * They did a decade ago with YARP1, these days it is 
     * entirely unnecessary.  It was just a workaround for 
     * some limits in the size of QNX buffers.  New carriers
     * can leave the index classbacks as no-ops.
     *
     */
    bool expectIndex() {
        pendingAck = true;
        messageLen = 0;
        getStreams().beginPacket();
        ref = NULL;
        bool ok = false;
        if (delegate!=NULL) {
            ok = delegate->expectIndex(*this);
        }
        if (ok) {
            reader.reset(is(),&getStreams(),getRoute(),
                         messageLen,delegate->isTextMode(),
                         delegate->isBareMode());
            if (ref!=NULL) {
                reader.setReference(ref);
            }
        } else {
            reader.reset(is(),&getStreams(),getRoute(),0,false);
        }
        return ok;
    }

    /**
     *
     * In principle we could respond to receipt of a message index.
     * We never have and we never will, but there's a callback for
     * it.
     *
     */
    bool respondToIndex() {
        return true;
    }

    /**
     *
     * After sending a message, wait for an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     *
     */
    bool expectAck() {
        YARP_ASSERT(delegate!=NULL);
        if (delegate->requireAck()) {
            return delegate->expectAck(*this);
        }
        return true;
    }

    /**
     *
     * After receiving a message, send an acknowledgement of receipt
     * (if the carrier is one that makes acknowledgements).
     *
     */
    bool sendAck() {
        bool ok = true;
        pendingAck = false;
        if (delegate==NULL) return false;
        if (delegate->requireAck()) {
            ok = delegate->sendAck(*this);
        }
        getStreams().endPacket();
        return ok;
    }

    /**
     *
     * Close things down - our streams, our carriers.
     *
     */
    void closeHelper() {
        active = false;
        if (pendingAck) {
            sendAck();
        }
        shift.close();
        if (delegate!=NULL) {
            delegate->close();
            delete delegate;
            delegate = NULL;
        }
        if (recv_delegate!=NULL) {
            recv_delegate->close();
            delete recv_delegate;
            recv_delegate = NULL;
        }
        if (send_delegate!=NULL) {
            send_delegate->close();
            delete send_delegate;
            send_delegate = NULL;
        }
    }

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
    Route route;             ///< names of (sender,carrier,receiver) triplet
    SizedWriter *writer;     ///< writer for current message
    StreamConnectionReader reader;  ///< reader for incoming messages
    yarp::os::Portable *ref; ///< source for current message, so we can
                             ///< bypass serialization on local connections
    String envelope;         ///< envelope for current message
    NullConnection nullConnection; ///< dummy connection
    yarp::os::Contactable *port;   ///< port associated with this connection
};

#endif
