// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
        route = Route("null","null","tcp");
        delegate = NULL;
        messageLen = 0;
        pendingAck = false;
        writer = NULL;
        altReader = NULL;
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
        delegate->prepareSend(*this);
    }

    void defaultExpectSenderSpecifier() {
        int len = 0;
        int r = NetType::readFull(is(),number.bytes());
        if (r!=number.length()) {
            throw new IOException("did not get sender name length");
        }
        len = NetType::netInt(number.bytes());
        if (len>1000) len = 1000;
        if (len<1) len = 1;
        // expect a string -- these days null terminated, but not in YARP1
        ManagedBytes b(len+1);
        r = NetType::readFull(is(),Bytes(b.get(),len));
        if (r!=len) {
            throw new IOException("did not get sender name");
        }
        // add null termination for YARP1
        b.get()[len] = '\0';
        String s = b.get();
        setRoute(getRoute().addFromName(s));
    }

    void sendHeader() {
        YARP_ASSERT(delegate!=NULL);
        delegate->sendHeader(*this);
    }

    void defaultSendHeader() {
        sendProtocolSpecifier();
        sendSenderSpecifier();
    }

    void expectHeader() {
        messageLen = 0;
        expectProtocolSpecifier();
        expectSenderSpecifier();
        YARP_ASSERT(delegate!=NULL);
        delegate->expectExtraHeader(*this);
    }

    void expectReplyToHeader() {
        YARP_ASSERT(delegate!=NULL);
        delegate->expectReplyToHeader(*this);
        //writer.reset(delegate->isTextMode());
    }

    void respondToHeader() {
        YARP_ASSERT(delegate!=NULL);
        delegate->respondToHeader(*this);
        os().flush();
    }

    int readYarpInt() {
        int len = NetType::readFull(is(),header.bytes());
        ACE_UNUSED_ARG(len);
        if (len!=header.length()) {
            throw IOException("data stream died");
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
            throw IOException("wrong header length");
        }
        char *base = header.get();
        base[0] = 'Y';
        base[1] = 'A';
        base[6] = 'R';
        base[7] = 'P';
        Bytes code(base+2,4);
        NetType::netInt(x,code);
    }


    void sendIndex() {
        ACE_DEBUG((LM_DEBUG,"Protocol::sendIndex for %s", getRoute().toString().c_str()));
        YARP_ASSERT(delegate!=NULL);
        delegate->sendIndex(*this);
    }

    void defaultSendIndex();

    void sendContent() {
        YARP_ASSERT(writer!=NULL);
        writer->write(os());
        os().flush();
    }

    void expectIndex() {
        pendingAck = true;
        messageLen = 0;
        YARP_ASSERT(delegate!=NULL);
        getStreams().beginPacket();
        delegate->expectIndex(*this);
        reader.reset(is(),&getStreams(),getRoute(),
                     messageLen,delegate->isTextMode());
    }

    void defaultExpectIndex();

    void respondToIndex() {
    }


    void expectAck() {
        YARP_ASSERT(delegate!=NULL);
        if (delegate->requireAck()) {
            int hdr = NetType::readFull(is(),header.bytes());
            if (hdr!=header.length()) {
                throw IOException("did not get acknowledgement header");
            }
            int len = interpretYarpNumber(header.bytes());
            if (len<0) {
                throw IOException("acknowledgement header is bad");
            }
            int len2 = NetType::readDiscard(is(),len);
            if (len!=len2) {
                throw IOException("did not get an acknowledgement of the promised length");
            }
        }
    }

    void sendAck() {
        pendingAck = false;
        YARP_ASSERT(delegate!=NULL);
        if (delegate->requireAck()) {
            delegate->sendAck(*this);
        }
        getStreams().endPacket();
    }

    void defaultSendAck();

    void interrupt() {
        try {
            if (pendingAck) {
                sendAck();
            }
            shift.getInputStream().interrupt();
        } catch (IOException e) {
            YARP_DEBUG(Logger::get(),
                       String("yarp::Protocol::interrupt exception: ") + 
                       e.toString());
        }
    }

    void close() {
        closeHelper();
    }

    void closeHelper() {
        //YARP_DEBUG(Logger::get(),"Protocol object closing");
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
        throw IOException("getRemoteAddress failed");
    }


    ///////////////////////////////////////////////////////////////////////
    // OutputProtocol view

    virtual void open(const Route& route) {
        setRoute(route);
        setCarrier(route.getCarrierName());
        sendHeader();
        expectReplyToHeader();
    }

    virtual void open(const String& name) {
        if (name=="") {
            setCarrier("text");
            setRoute(Route("no-name","no-name","no-carrier"));
        } else {
            expectHeader();
            respondToHeader();
            setRoute(getRoute().addToName(name));
        }
    }

    virtual bool isActive() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isActive();
    }

    virtual bool isTextMode() {
        YARP_ASSERT(delegate!=NULL);
        return delegate->isTextMode();
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
                reader.reset(is(),&getStreams(),getRoute(),
                             messageLen,delegate->isTextMode());
                reply->read(reader);
            }
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
        expectIndex();
        respondToIndex();
        if (altReader!=NULL) {
            YARP_DEBUG(Logger::get(), "alternate reader in operation");
            return *altReader;
        }
        return reader;
    }

    virtual void endRead() {
        if (altReader!=NULL) {
            //altReader->release();
            sendAck();
        } else {
            sendAck();
            reader.flushWriter();
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

private:

    void sendProtocolSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        delegate->getHeader(header.bytes());
        os().write(header.bytes());
        os().flush();
    }

    void expectProtocolSpecifier() {
        int len = NetType::readFull(is(),header.bytes());
        ACE_UNUSED_ARG(len);
        //ACE_OS::printf("len is %d but header is %d\n", len, header.length());
        if (len==-1) {
            throw IOException("no connection");
        }
        if(len!=header.length()) {
            throw IOException("data stream died");
        }
        bool already = false;
        if (delegate!=NULL) {
            if (delegate->checkHeader(header.bytes())) {
                already = true;
            }
        }
        if (!already) {
            delegate = Carriers::chooseCarrier(header.bytes());
        }
        if (delegate==NULL) {
            throw IOException("unrecognized protocol");
        }
        setRoute(getRoute().addCarrierName(delegate->getName()));
        delegate->setParameters(header.bytes());
    }


    void sendSenderSpecifier() {
        const String& senderName = getRoute().getFromName();
        NetType::netInt(senderName.length()+1,number.bytes());
        os().write(number.bytes());
        Bytes b((char*)senderName.c_str(),senderName.length()+1);
        os().write(b);
        os().flush();
    }

    void expectSenderSpecifier() {
        YARP_ASSERT(delegate!=NULL);
        delegate->expectSenderSpecifier(*this);
        ACE_DEBUG((LM_DEBUG,"Sender name is %s",getRoute().getFromName().c_str()));
    }

    int messageLen;
    bool pendingAck;
    Logger& log;
    ManagedBytes header;
    ManagedBytes number;
    ManagedBytes indexHeader;
    ShiftStream shift;
    Carrier *delegate;
    Route route;
    //BufferedConnectionWriter writer;
    SizedWriter *writer;
    StreamConnectionReader reader;
    ConnectionReader *altReader;

};

#endif
