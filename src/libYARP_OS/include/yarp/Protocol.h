// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PROTOCOL_
#define _YARP2_PROTOCOL_

#include <yarp/Carrier.h>
#include <yarp/Logger.h>
#include <yarp/String.h>
#include <yarp/TwoWayStream.h>
#include <yarp/Carriers.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/StreamConnectionReader.h>
#include <yarp/ManagedBytes.h>
#include <yarp/NetType.h>
#include <yarp/ShiftStream.h>
#include <yarp/os/Portable.h>

#define throw_IOException(e) YARP_DEBUG(Logger::get(),e)

namespace yarp {
    class Protocol;
}

/**
 * Connection Communication choreographer.  Handles a single YARP connection,
 * and insulates ports from the details of the particular Carrier in use.
 */
class yarp::Protocol : public OutputProtocol, public InputProtocol {
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
        messageLen = 0;
        pendingAck = false;
        writer = NULL;
        altReader = NULL;
        ref = NULL;
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

    void setRoute(const Route& route) {
        this->route = route;
    }

    const Route& getRoute() {
        return route;
    }

    void setCarrier(const String& carrierName) {
        setRoute(getRoute().addCarrierName(carrierName));
        YARP_ASSERT(delegate==NULL);
        delegate = Carriers::chooseCarrier(carrierName);
        /*
        if (delegate==NULL) {
            throw new IOException("no such carrier");
        }
        */
        if (delegate!=NULL) {
            delegate->prepareSend(*this);
        }
    }

    bool defaultExpectSenderSpecifier() {
        int len = 0;
        int r = NetType::readFull(is(),number.bytes());
        if (r!=number.length()) {
            throw_IOException("did not get sender name length");
            return false;
        }
        len = NetType::netInt(number.bytes());
        if (len>1000) len = 1000;
        if (len<1) len = 1;
        // expect a string -- these days null terminated, but not in YARP1
        ManagedBytes b(len+1);
        r = NetType::readFull(is(),Bytes(b.get(),len));
        if (r!=len) {
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
        int len = NetType::readFull(is(),header.bytes());
        ACE_UNUSED_ARG(len);
        if (len!=header.length()) {
            throw_IOException("data stream died");
            return -1;
        }
        return interpretYarpNumber(header.bytes());
    }

    void writeYarpInt(int n) {
        createYarpNumber(n,header.bytes());
        os().write(header.bytes());
    }

    static int interpretYarpNumber(const Bytes& b) {
        if (b.length()==8) {
            char *base = b.get();
            if (base[0]=='Y' && base[1]=='A' &&
                base[6]=='R' && base[7]=='P') {
                Bytes b2(b.get()+2,4);
                int x = NetType::netInt(b2);
                return x;
            }
        }
        return -1;
    }

    static void createYarpNumber(int x,const Bytes& header) {
        if (header.length()!=8) {
            ACE_OS::printf("wrong header length");
            ACE_OS::exit(1);
        }
        char *base = header.get();
        base[0] = 'Y';
        base[1] = 'A';
        base[6] = 'R';
        base[7] = 'P';
        Bytes code(base+2,4);
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
        return true;
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
            int hdr = NetType::readFull(is(),header.bytes());
            if (hdr!=header.length()) {
                throw_IOException("did not get acknowledgement header");
                return false;
            }
            int len = interpretYarpNumber(header.bytes());
            if (len<0) {
                throw_IOException("acknowledgement header is bad");
                return false;
            }
            int len2 = NetType::readDiscard(is(),len);
            if (len!=len2) {
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
        return true;
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
        //             String("yarp::Protocol::interrupt exception: ") + 
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
        ACE_DEBUG((LM_ERROR,"Protocol::getRemoteAddress not yet implemented"));
        //throw IOException("getRemoteAddress failed");
        return nullAddress;
    }
    


    ///////////////////////////////////////////////////////////////////////
    // OutputProtocol view

    virtual bool open(const Route& route) {
        setRoute(route);
        setCarrier(route.getCarrierName());
        if (delegate==NULL) {
            return false;
        }
        bool ok = sendHeader();
        if (!ok) return false;
        return expectReplyToHeader();
    }

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

    virtual void write(SizedWriter& writer) {
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
                reply->read(reader);
            }
            expectAck(); //MOVE ack to after reply, if present
        }
        this->writer = NULL;
    }

    virtual OutputProtocol& getOutput() {
        return *this;
    }

    virtual InputProtocol& getInput() {
        return *this;
    }


    virtual ConnectionReader& beginRead() {
        if (delegate!=NULL) {
            expectIndex();
            respondToIndex();
            if (altReader!=NULL) {
                YARP_DEBUG(Logger::get(), "alternate reader in operation");
                return *altReader;
            }
        }
        return reader;
    }

    virtual void suppressReply() {
        reader.suppressReply();
    }

    virtual void endRead() {
        if (altReader!=NULL) {
            //altReader->release();
            sendAck();
        } else {
            reader.flushWriter();
            sendAck();  //MOVE ack to after reply, if present
        }
    }

    virtual bool checkStreams() {
        return shift.isOk();
    }

    virtual void resetStreams() {
        shift.reset();
    }


    void setReader(ConnectionReader *altReader) {
        this->altReader = altReader;
    }


    void setReference(yarp::os::Portable *ref) {
        this->ref = ref;
    }

private:

    bool sendProtocolSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        delegate->getHeader(header.bytes());
        os().write(header.bytes());
        os().flush();
        return os().isOk();
    }

    bool expectProtocolSpecifier() {
        int len = NetType::readFull(is(),header.bytes());
        ACE_UNUSED_ARG(len);
        //ACE_OS::printf("len is %d but header is %d\n", len, header.length());
        if (len==-1) {
            throw_IOException("no connection");
            return false;
        }
        if(len!=header.length()) {
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
                String msg = "* Error. Protocol not found.\r\n* Hello. You appear to be trying to communicate with a YARP Port.\r\n* The first 8 bytes sent to a YARP Port are criticial for identifying the\r\n* protocol you wish to speak.\r\n* The first 8 bytes you sent were not associated with any particular protocol.\r\n* If you are a human, try typing \"CONNECT foo\" followed by a <RETURN>.\r\n* The 8 bytes \"CONNECT \" correspond to a simple text-mode protocol.\r\n* Goodbye.\r\n";
                Bytes b((char*)msg.c_str(),msg.length());
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
        const String& senderName = getRoute().getFromName();
        NetType::netInt(senderName.length()+1,number.bytes());
        os().write(number.bytes());
        Bytes b((char*)senderName.c_str(),senderName.length()+1);
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



    int messageLen;
    bool pendingAck;
    Logger& log;
    ManagedBytes header;
    ManagedBytes number;
    ManagedBytes indexHeader;
    ShiftStream shift;
    bool active;
    Carrier *delegate;
    Route route;
    //BufferedConnectionWriter writer;
    SizedWriter *writer;
    StreamConnectionReader reader;
    ConnectionReader *altReader;
    yarp::os::Portable *ref;
    Address nullAddress;
};

#endif
