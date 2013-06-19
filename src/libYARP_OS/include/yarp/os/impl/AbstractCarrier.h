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

    virtual void setParameters(const yarp::os::Bytes& header);

    virtual void getHeader(const yarp::os::Bytes& header) = 0;


    virtual bool isConnectionless();
    virtual bool supportReply();
    virtual bool canAccept();
    virtual bool canOffer();
    virtual bool isTextMode();
    virtual bool requireAck();
    virtual bool canEscape();
    virtual bool isLocal();
    virtual String toString();

    // sender
    virtual bool prepareSend(Protocol& proto);
    virtual bool sendHeader(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);
    virtual bool sendIndex(Protocol& proto);

    // receiver
    virtual bool expectExtraHeader(Protocol& proto);
    virtual bool respondToHeader(Protocol& proto) = 0; // left abstract, no good default
    virtual bool expectIndex(Protocol& proto);
    virtual bool expectSenderSpecifier(Protocol& proto);
    virtual bool sendAck(Protocol& proto);
    virtual bool expectAck(Protocol& proto);

    virtual bool isActive();

    virtual void setCarrierParams(const yarp::os::Property& params);
    virtual void getCarrierParams(yarp::os::Property& params);

    // some default implementations of protocol phases used by
    // certain YARP carriers

    bool defaultSendHeader(Protocol& proto);

protected:
    int getSpecifier(const Bytes& b);
    void createStandardHeader(int specifier,const yarp::os::Bytes& header);
    virtual bool write(Protocol& proto, SizedWriter& writer);
    bool sendProtocolSpecifier(Protocol& proto);
    bool sendSenderSpecifier(Protocol& proto);
};

#endif
