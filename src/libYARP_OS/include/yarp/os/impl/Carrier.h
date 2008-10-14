// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_CARRIER_
#define _YARP2_CARRIER_

#include <yarp/os/impl/ShiftStream.h>
#include <yarp/Bytes.h>
#include <yarp/os/impl/SizedWriter.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Carrier;
            class Protocol;
        }
    }
}

/**
 * Class representing one specific protocol for sending and receiving
 * YARP messages.  YARP communication occurs through a series of
 * defined phases that make it easy to duplicate output.  Any process
 * that goes through the same phases can be described as a Carrier and
 * then made available (through the Carriers class) for connections.
 */
class yarp::os::impl::Carrier : public ShiftStream {
public:


    virtual Carrier *create() = 0;

    virtual String getName() = 0;
    virtual bool checkHeader(const yarp::os::Bytes& header) = 0;
    virtual void setParameters(const yarp::os::Bytes& header) = 0;
    virtual void getHeader(const yarp::os::Bytes& header) = 0;


    virtual bool isConnectionless() = 0;
    virtual bool canAccept() = 0;
    virtual bool canOffer() = 0;
    virtual bool isTextMode() = 0;
    virtual bool canEscape() = 0;
    virtual bool requireAck() = 0;
    virtual bool supportReply() = 0;
    virtual bool isLocal() = 0;

    // all remaining may throw IOException

    //virtual void close() = 0; // from ShiftStream

    // sender
    virtual bool prepareSend(Protocol& proto) = 0;
    virtual bool sendHeader(Protocol& proto) = 0;
    virtual bool expectReplyToHeader(Protocol& proto) = 0;
    virtual bool sendIndex(Protocol& proto) = 0;

    virtual bool write(Protocol& proto, SizedWriter& writer) = 0;

    // receiver
    virtual bool expectExtraHeader(Protocol& proto) = 0;
    virtual bool respondToHeader(Protocol& proto) = 0;
    virtual bool expectIndex(Protocol& proto) = 0;
    virtual bool expectSenderSpecifier(Protocol& proto) = 0;
    virtual bool sendAck(Protocol& proto) = 0;
    virtual bool expectAck(Protocol& proto) = 0;

    virtual bool isActive() = 0;

    virtual String toString() = 0;
};

#endif

