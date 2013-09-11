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
     * This becomes owner of shiftstream
     */
    Protocol(TwoWayStream *stream) :
        log(Logger::get()) {
        shift.takeStream(stream);
        active = true;
        route = Route("null","null","tcp");
        delegate = NULL;
        recv_delegate = NULL;
        need_recv_delegate = false;
        messageLen = 0;
        pendingAck = false;
        writer = NULL;
        ref = NULL;
        reader.setProtocol(this);
        envelope = "";
    }

    virtual ~Protocol() {
        closeHelper();
    }

    void setRoute(const Route& route);

    const Route& getRoute() {
        return route;
    }

    void interrupt() {
        if (active) {
            if (pendingAck) {
                sendAck();
            }
            shift.getInputStream().interrupt();
            active = false;
        }
    }

    void close() {
        closeHelper();
    }

    TwoWayStream& getStreams() {
        return shift;
    }

    void takeStreams(TwoWayStream *streams) {
        shift.takeStream(streams);
        if (streams!=NULL) {
            active = true;
        }
    }

    TwoWayStream *giveStreams() {
        return shift.giveStream();
    }

    OutputStream& os() {
        return shift.getOutputStream();
    }

    InputStream& is() {
        return shift.getInputStream();
    }

    OutputStream& getOutputStream() {
        return os();
    }

    InputStream& getInputStream() {
        return is();
    }


    ///////////////////////////////////////////////////////////////////////
    // OutputProtocol view

    virtual bool open(const Route& route);

    virtual void rename(const Route& route) {
        setRoute(route);
    }


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

    virtual bool isOk() {
        return checkStreams();
    }

    virtual bool write(SizedWriter& writer) {
        bool replied = false;
        writer.stopWrite();
        this->writer = &writer;
        if (getConnection().isActive()) {
            YARP_ASSERT(delegate!=NULL);
            getStreams().beginPacket();
            delegate->write(*this,writer);
            getStreams().endPacket();
            PortReader *reply = writer.getReplyHandler();
            if (reply!=NULL) {
                if (!delegate->supportReply()) {
                    YARP_INFO(log,String("connection ") + getRoute().toString() + " does not support replies (try \"tcp\" or \"text_ack\")");
                }
                reader.reset(is(),&getStreams(), getRoute(),
                             messageLen,delegate->isTextMode());
                replied = reply->read(reader);
            }
            expectAck(); //MOVE ack to after reply, if present
        }
        this->writer = NULL;
        return replied;
    }

    void reply(SizedWriter& writer) {
        writer.stopWrite();
        delegate->reply(*this,writer);
    }

    virtual OutputProtocol& getOutput() {
        return *this;
    }

    virtual InputProtocol& getInput() {
        return *this;
    }


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

    virtual void suppressReply() {
        reader.suppressReply();
    }

    virtual void endRead() {
        reader.flushWriter();
        sendAck();  //MOVE ack to after reply, if present
    }

    virtual bool checkStreams() {
        return shift.isOk();
    }

    void setReference(yarp::os::Portable *ref) {
        this->ref = ref;
    }

    yarp::os::ConstString getSenderSpecifier();

    virtual bool setTimeout(double timeout) {
        bool ok = os().setWriteTimeout(timeout);
        if (!ok) return false;
        return is().setReadTimeout(timeout);
    }

    virtual void setEnvelope(const yarp::os::ConstString& str) {
        envelope = str;
    }

    const String& getEnvelope() {
        return envelope;
    }

    Log& getLog() {
        return log;
    }

    void setRemainingLength(int len) {
        messageLen = len;
    }

    Connection& getConnection() {
        if (delegate==NULL) {
            return nullConnection;
        }
        return *delegate;
    }

    // transitional hook, should go away when we have real chaining
    Connection& getReceiver() {
        if (recv_delegate==NULL) {
            return nullConnection;
        }
        return *recv_delegate;
    }

private:

    bool getRecvDelegate();

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


    bool expectSenderSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->expectSenderSpecifier(*this);
    }

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

    void setCarrier(const String& carrierName) {
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

    bool sendHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->sendHeader(*this);
    }

    bool expectReplyToHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->expectReplyToHeader(*this);
    }

    bool respondToHeader() {
        YARP_ASSERT(delegate!=NULL);
        bool ok = delegate->respondToHeader(*this);
        if (!ok) return false;
        os().flush();
        return os().isOk();
    }

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
                         messageLen,delegate->isTextMode());
            if (ref!=NULL) {
                reader.setReference(ref);
            }
        } else {
            reader.reset(is(),&getStreams(),getRoute(),0,false);
        }
        return ok;
    }

    bool respondToIndex() {
        return true;
    }

    bool expectAck() {
        YARP_ASSERT(delegate!=NULL);
        if (delegate->requireAck()) {
            return delegate->expectAck(*this);
        }
        return true;
    }

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
    }

    int messageLen;
    bool pendingAck;
    Logger& log;
    ShiftStream shift;
    bool active;
    Carrier *delegate, *recv_delegate;
    bool need_recv_delegate;
    Route route;
    SizedWriter *writer;
    StreamConnectionReader reader;
    yarp::os::Portable *ref;
    String envelope;
    NullConnection nullConnection;
};

#endif
