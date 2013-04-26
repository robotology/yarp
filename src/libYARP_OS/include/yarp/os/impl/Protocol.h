// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PROTOCOL_
#define _YARP2_PROTOCOL_

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/ShiftStream.h>
#include <yarp/os/Portable.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/PlatformSize.h>

#define throw_IOException(e) YARP_DEBUG(Logger::get(),e)

namespace yarp {
    namespace os {
        namespace impl {
            class Protocol;
        }
    }
}

/**
 * Connection Communication choreographer.  Handles a single YARP connection,
 * and insulates ports from the details of the particular Carrier in use.
 */
class YARP_OS_impl_API yarp::os::impl::Protocol : public OutputProtocol, public InputProtocol {
public:

    // everything could throw IOException

    /**
     * This becomes owner of shiftstream
     */
    Protocol(TwoWayStream *stream) :
        log(Logger::get()), header(8), number(4), indexHeader(10) {
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
        /*
          if (delegate!=NULL) {
          delegate->close();
          delete delegate;
          delegate = NULL;
          }
          shift.close();
        */
        closeHelper();
    }

    void setRoute(const Route& route);

    const Route& getRoute() {
        return route;
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

    bool defaultExpectSenderSpecifier() {
        int len = 0;
        ssize_t r = NetType::readFull(is(),number.bytes());
        if ((size_t)r!=number.length()) {
            throw_IOException("did not get sender name length");
            return false;
        }
        len = NetType::netInt(number.bytes());
        if (len>1000) len = 1000;
        if (len<1) len = 1;
        // expect a string -- these days null terminated, but not in YARP1
        yarp::os::ManagedBytes b(len+1);
        r = NetType::readFull(is(),yarp::os::Bytes(b.get(),len));
        if ((int)r!=len) {
            throw_IOException("did not get sender name");
            return false;
        }
        // add null termination for YARP1
        b.get()[len] = '\0';
        String s = b.get();
        setRoute(getRoute().addFromName(s));
        return true;
    }

    bool sendHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->sendHeader(*this);
    }

    bool defaultSendHeader() {
        bool ok = sendProtocolSpecifier();
        if (!ok) return false;
        return sendSenderSpecifier();
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

    bool expectReplyToHeader() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->expectReplyToHeader(*this);
        //writer.reset(delegate->isTextMode());
    }

    bool respondToHeader() {
        YARP_ASSERT(delegate!=NULL);
        bool ok = delegate->respondToHeader(*this);
        if (!ok) return false;
        os().flush();
        return os().isOk();
    }

    int readYarpInt() {
        ssize_t len = NetType::readFull(is(),header.bytes());
        if ((size_t)len!=header.length()) {
            throw_IOException("data stream died");
            return -1;
        }
        return interpretYarpNumber(header.bytes());
    }

    void writeYarpInt(int n) {
        createYarpNumber(n,header.bytes());
        os().write(header.bytes());
    }

    static int interpretYarpNumber(const yarp::os::Bytes& b) {
        if (b.length()==8) {
            char *base = b.get();
            if (base[0]=='Y' && base[1]=='A' &&
                base[6]=='R' && base[7]=='P') {
                yarp::os::Bytes b2(b.get()+2,4);
                int x = NetType::netInt(b2);
                return x;
            }
        }
        return -1;
    }

    static void createYarpNumber(int x,const yarp::os::Bytes& header) {
        if (header.length()!=8) {
            ACE_OS::printf("wrong header length");
            ACE_OS::exit(1);
        }
        char *base = header.get();
        base[0] = 'Y';
        base[1] = 'A';
        base[6] = 'R';
        base[7] = 'P';
        yarp::os::Bytes code(base+2,4);
        NetType::netInt(x,code);
    }


    bool sendIndex() {
        YARP_DEBUG(Logger::get(), String("Sending a message on connection ") + getRoute().toString());;
        YARP_ASSERT(delegate!=NULL);
        return delegate->sendIndex(*this);
    }

    bool defaultSendIndex();

    bool sendContent() {
        YARP_ASSERT(writer!=NULL);
        writer->write(os());
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

    bool defaultExpectIndex();

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

    bool defaultExpectAck() {
        YARP_ASSERT(delegate!=NULL);
        if (delegate->requireAck()) {
            ssize_t hdr = NetType::readFull(is(),header.bytes());
            if ((size_t)hdr!=header.length()) {
                throw_IOException("did not get acknowledgement header");
                return false;
            }
            int len = interpretYarpNumber(header.bytes());
            if (len<0) {
                throw_IOException("acknowledgement header is bad");
                return false;
            }
            size_t len2 = NetType::readDiscard(is(),len);
            if ((size_t)len!=len2) {
                throw_IOException("did not get an acknowledgement of the promised length");
                return false;
            }
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

    bool defaultSendAck();

    void interrupt() {
        if (active) {
            if (pendingAck) {
                sendAck();
            }
            shift.getInputStream().interrupt();
            active = false;
        }
        //} catch (IOException e) {
        //  YARP_DEBUG(Logger::get(),
        //             String("Protocol::interrupt exception: ") +
        //             e.toString());
    }

    void close() {
        closeHelper();
    }

    void closeHelper() {
        //YARP_DEBUG(Logger::get(),"Protocol object closing");
        active = false;
        try {
            if (pendingAck) {
                sendAck();
            }
        } catch (IOException e) {
            // ok, comms shutting down
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
        //YARP_DEBUG(Logger::get(),"Protocol object closed");
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

    const Address& getRemoteAddress() {
        YARP_SPRINTF0(Logger::get(),
                      error,
                      "Protocol::getRemoteAddress not yet implemented");
        //throw IOException("getRemoteAddress failed");
        return nullAddress;
    }



    ///////////////////////////////////////////////////////////////////////
    // OutputProtocol view

    virtual bool open(const Route& route);

    virtual void rename(const Route& route) {
        setRoute(route);
    }


    virtual bool open(const String& name) {
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

    virtual bool isActive() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isActive();
    }

    virtual bool isOk() {
        return checkStreams();
    }

    virtual bool isTextMode() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isTextMode();
    }

    virtual bool supportReply() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->supportReply();
    }

    virtual bool isConnectionless() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isConnectionless();
    }

    virtual bool isBroadcast() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isBroadcast();
    }

    virtual void prepareDisconnect() {
        YARP_ASSERT(delegate!=NULL);
        delegate->prepareDisconnect();
    }


    virtual bool write(SizedWriter& writer) {
        bool replied = false;
        writer.stopWrite();
        this->writer = &writer;
        if (isActive()) {
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


    bool defaultReply(SizedWriter& writer) {
        writer.write(os());
        return os().isOk();
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

    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) {
        if (recv_delegate) {
            return recv_delegate->modifyIncomingData(reader);
        }
        return reader;
    }

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) {
        if (recv_delegate) {
            return recv_delegate->acceptIncomingData(reader);
        }
        return true;
    }

    virtual bool skipIncomingData(yarp::os::ConnectionReader& reader);

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

    virtual void resetStreams() {
        shift.reset();
    }


    void setReference(yarp::os::Portable *ref) {
        this->ref = ref;
    }

    String getSenderSpecifier();

    virtual bool setTimeout(double timeout) {
        bool ok = os().setWriteTimeout(timeout);
        if (!ok) return false;
        return is().setReadTimeout(timeout);
    }

    virtual void setEnvelope(const String& str) {
        envelope = str;
    }

    const String& getEnvelope() {
        return envelope;
    }

    virtual void setInputCarrierParams(const yarp::os::Property& params) {
        if(recv_delegate)
            recv_delegate->setCarrierParams(params);
    }

    virtual void getInputCarrierParams(yarp::os::Property& params) { 
        if(recv_delegate)
            recv_delegate->getCarrierParams(params);
    }

    virtual void setOutputCarrierParams(const yarp::os::Property& params) {
        if(delegate)
            delegate->setCarrierParams(params);
    }
    
    virtual void getOutputCarrierParams(yarp::os::Property& params) { 
        if(delegate)
            delegate->getCarrierParams(params);
    }

private:

    bool getRecvDelegate();

    bool sendProtocolSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        delegate->getHeader(header.bytes());
        os().write(header.bytes());
        os().flush();
        return os().isOk();
    }

    bool expectProtocolSpecifier() {
        ssize_t len = NetType::readFull(is(),header.bytes());
        //ACE_OS::printf("len is %d but header is %d\n", len, header.length());
        if (len==-1) {
            throw_IOException("no connection");
            return false;
        }
        if((size_t)len!=header.length()) {
            throw_IOException("data stream died");
            return false;
        }
        bool already = false;
        if (delegate!=NULL) {
            if (delegate->checkHeader(header.bytes())) {
                already = true;
            }
        }
        if (!already) {
            //try {
            delegate = Carriers::chooseCarrier(header.bytes());
            //} catch (IOException e) {
            if (delegate==NULL) {
                // carrier not found; send a message
                String msg = "* Error. Protocol not found.\r\n* Hello. You appear to be trying to communicate with a YARP Port.\r\n* The first 8 bytes sent to a YARP Port are critical for identifying the\r\n* protocol you wish to speak.\r\n* The first 8 bytes you sent were not associated with any particular protocol.\r\n* If you are a human, try typing \"CONNECT foo\" followed by a <RETURN>.\r\n* The 8 bytes \"CONNECT \" correspond to a simple text-mode protocol.\r\n* Goodbye.\r\n";
                yarp::os::Bytes b((char*)msg.c_str(),msg.length());
                os().write(b);
                os().flush();
            }
        }
        if (delegate==NULL) {
            throw_IOException("unrecognized protocol");
            return false;
        }
        setRoute(getRoute().addCarrierName(delegate->getName()));
        delegate->setParameters(header.bytes());
        return true;
    }


    bool sendSenderSpecifier() {
        const String senderName = getSenderSpecifier();
        //const String& senderName = getRoute().getFromName();
        NetType::netInt((int)senderName.length()+1,number.bytes());
        os().write(number.bytes());
        yarp::os::Bytes b((char*)senderName.c_str(),senderName.length()+1);
        os().write(b);
        os().flush();
        return os().isOk();
    }

    bool expectSenderSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->expectSenderSpecifier(*this);
        //ACE_DEBUG((LM_DEBUG,"Sender name is %s",getRoute().getFromName().c_str()));
    }

    bool canEscape() {
        if (delegate==NULL) { return true; }
        return delegate->canEscape();
    }

    bool isLocal() {
        if (delegate==NULL) { return false; }
        return delegate->isLocal();
    }

    bool isPush() {
        if (delegate==NULL) { return true; }
        return delegate->isPush();
    }

    int messageLen;
    bool pendingAck;
    Logger& log;
    yarp::os::ManagedBytes header;
    yarp::os::ManagedBytes number;
    yarp::os::ManagedBytes indexHeader;
    ShiftStream shift;
    bool active;
    Carrier *delegate, *recv_delegate;
    bool need_recv_delegate;
    Route route;
    //BufferedConnectionWriter writer;
    SizedWriter *writer;
    StreamConnectionReader reader;
    yarp::os::Portable *ref;
    Address nullAddress;
    String envelope;
};

#endif
