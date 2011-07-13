// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ABSTRACTCARRIER_
#define _YARP2_ABSTRACTCARRIER_

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Protocol.h>

namespace yarp {
    namespace os {
        namespace impl {
            class AbstractCarrier;
        }
    }
}

/**
 * A starter class for implementing simple carriers.
 * It implements reasonable default behavior.
 */
class YARP_OS_impl_API yarp::os::impl::AbstractCarrier : public Carrier {
public:

    virtual Carrier *create() = 0;

    virtual String getName() = 0;

    virtual bool checkHeader(const yarp::os::Bytes& header) = 0;

    virtual void setParameters(const yarp::os::Bytes& header) {
        // default - no parameters
    }

    virtual void getHeader(const yarp::os::Bytes& header) = 0;


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

    virtual bool canEscape() {
        return true;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return getName();
    }

    // sender
    virtual bool prepareSend(Protocol& proto) {
        return true;
    }

    virtual bool sendHeader(Protocol& proto) {
        return proto.defaultSendHeader();
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        return true;
    }

    virtual bool sendIndex(Protocol& proto) {
        return proto.defaultSendIndex();
    }

    // receiver
    virtual bool expectExtraHeader(Protocol& proto) {
        return true;
    }

    // left abstract, no good default
    virtual bool respondToHeader(Protocol& proto) = 0;

    virtual bool expectIndex(Protocol& proto) {
        return proto.defaultExpectIndex();
    }

    virtual bool expectSenderSpecifier(Protocol& proto) {
        return proto.defaultExpectSenderSpecifier();
    }

    virtual bool sendAck(Protocol& proto) {
        return proto.defaultSendAck();
    }

    virtual bool expectAck(Protocol& proto) {
        return proto.defaultExpectAck();
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

    void createStandardHeader(int specifier,const yarp::os::Bytes& header) {
        Protocol::createYarpNumber(7777+specifier,header);
    }

    virtual bool write(Protocol& proto, SizedWriter& writer) {
        // default behavior upon a write request
        ACE_UNUSED_ARG(writer);
        bool ok = proto.sendIndex();
        if (!ok) return false;
        ok = proto.sendContent();
        if (!ok) return false;
        // proto.expectAck(); //MOVE ack to after reply, if present
        return true;
    }

};

#endif
