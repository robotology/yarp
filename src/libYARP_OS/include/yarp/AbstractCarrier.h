// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_ABSTRACTCARRIER_
#define _YARP2_ABSTRACTCARRIER_

#include <yarp/Carrier.h>
#include <yarp/Protocol.h>

namespace yarp {
    class AbstractCarrier;
}

/**
 * A starter class for implementing simple carriers.
 * It implements reasonable default behavior.
 */
class yarp::AbstractCarrier : public Carrier {
public:

    virtual Carrier *create() = 0;

    virtual String getName() = 0;

    virtual bool checkHeader(const Bytes& header) = 0;

    virtual void setParameters(const Bytes& header) {
        // default - no parameters
    }

    virtual void getHeader(const Bytes& header) = 0;


    virtual bool isConnectionless() {
        // conservative choice - shortcuts are taken for connection
        return true;
    }

    virtual bool supportReply() {
        return !isConnectionless();
    }


    virtual bool canAccept() {
        return true;
    }

    virtual bool canOffer() {
        return true;
    }

    virtual bool isTextMode() {
        return false;
    }

    virtual bool requireAck() {
        return false;
    }

    virtual String toString() {
        return getName();
    }

    // all remaining may throw IOException

    // sender
    virtual void prepareSend(Protocol& proto) {
    }

    virtual void sendHeader(Protocol& proto) {
        proto.defaultSendHeader();
    }

    virtual void expectReplyToHeader(Protocol& proto) {
    }

    virtual void sendIndex(Protocol& proto) {
        proto.defaultSendIndex();
    }

    // receiver
    virtual void expectExtraHeader(Protocol& proto) {
    }

    // left abstract, no good default
    virtual void respondToHeader(Protocol& proto) = 0;

    virtual void expectIndex(Protocol& proto) {
        proto.defaultExpectIndex();
    }

    virtual void expectSenderSpecifier(Protocol& proto) {
        proto.defaultExpectSenderSpecifier();
    }

    virtual void sendAck(Protocol& proto) {
        proto.defaultSendAck();
    }

    virtual void expectAck(Protocol& proto) {
        proto.defaultExpectAck();
    }

    virtual bool isActive() {
        return true;
    }


    //virtual void close() {
    //ShiftStream::close();
    //}

protected:

    int getSpecifier(const Bytes& b) {
        int x = Protocol::interpretYarpNumber(b);
        if (x>=0) {
            return x-7777;
        }
        return x;
    }

    void createStandardHeader(int specifier,const Bytes& header) {
        Protocol::createYarpNumber(7777+specifier,header);
    }

    virtual void write(Protocol& proto, SizedWriter& writer) {
        // default behavior upon a write request
        ACE_UNUSED_ARG(writer);
        proto.sendIndex();
        proto.sendContent();
        // proto.expectAck(); //MOVE ack to after reply, if present
    }

};

#endif
