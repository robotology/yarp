// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NAMESERCARRIER_
#define _YARP2_NAMESERCARRIER_

#include <yarp/TwoWayStream.h>
#include <yarp/TcpCarrier.h>

namespace yarp {
    class NameserCarrier;
    class NameserTwoWayStream;
}

/**
 * Communicating between two ports via a variant plain-text protocol 
 * originally designed for the yarp name server.  For backwards 
 * compatibility
 */
class yarp::NameserTwoWayStream : public TwoWayStream, InputStream {
private:
    TwoWayStream *delegate;
    String pendingRead;
    String swallowRead;
public:
    NameserTwoWayStream(TwoWayStream *delegate) : 
        delegate(delegate) {
        pendingRead = "";
        swallowRead = "VER ";
    }

    virtual ~NameserTwoWayStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return *this; }
    virtual OutputStream& getOutputStream() { 
        return delegate->getOutputStream(); 
    }

    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void close() {
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    virtual int read(const Bytes& b) {
        // assume it is ok for name_ser to go byte-by-byte
        // since this protocol will be phased out
        if (b.length()<=0) {
            return 0;
        }
        Bytes tmp(b.get(),1);
        while (swallowRead.length()>0) {
            int r = delegate->getInputStream().read(tmp);
            if (r<=0) { return r; }
            swallowRead = swallowRead.substr(1,swallowRead.length()-1);
        }
        if (pendingRead.length()>0) {
            b.get()[0] = pendingRead[0];
            pendingRead = pendingRead.substr(1,pendingRead.length()-1);
            return 1;
        }
        int r = delegate->getInputStream().read(tmp);
        if (r<=0) { return r; }
        if (tmp.get()[0]=='\n') {
            pendingRead = "";
            swallowRead = "NAME_SERVER ";
        }
        return r;
    }
};


/**
 * Communicating between two ports via a variant plain-text protocol 
 * originally designed for the yarp name server.  For backwards 
 * compatibility
 */
class yarp::NameserCarrier : public TcpCarrier {
private:
    bool firstSend;
public:
    NameserCarrier() {
        firstSend = true;
    }

    virtual String getName() {
        return "name_ser";
    }

    virtual String getSpecifierName() {
        return "NAME_SER";
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                if (!(target[i]==header.get()[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }   
    }

    virtual Carrier *create() {
        return new NameserCarrier();
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool isTextMode() {
        return true;
    }

    virtual bool supportReply() {
        return false;
    }

    virtual bool canEscape() {
        return false;
    }

    virtual bool sendHeader(Protocol& proto) {
        String target = getSpecifierName();
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        proto.os().flush();
        return proto.os().isOk();
    }

    bool expectSenderSpecifier(Protocol& proto) {
        proto.setRoute(proto.getRoute().addFromName("anon"));
        return true;
    }

    bool sendIndex(Protocol& proto) {
        String target = firstSend?"VER ":"NAME_SERVER ";
        Bytes b((char*)target.c_str(),target.length());
        proto.os().write(b);
        proto.os().flush();
        firstSend = false;
        return proto.os().isOk();
    }

    bool expectIndex(Protocol& proto) {
        return true;
    }

    bool sendAck(Protocol& proto) {
        return true;
    }

    virtual bool expectAck(Protocol& proto) {
        return true;
    }

    virtual bool respondToHeader(Protocol& proto) {
        // I am the receiver
        NameserTwoWayStream *stream = 
            new NameserTwoWayStream(proto.giveStreams());
        proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        // I am the sender
        //NameserTwoWayStream *stream = 
        //  new NameserTwoWayStream(proto.giveStreams(),true);
        //proto.takeStreams(stream);
        return true;
    }
};

#endif
