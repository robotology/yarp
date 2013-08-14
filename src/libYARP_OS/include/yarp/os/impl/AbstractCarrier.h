// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ABSTRACTCARRIER_
#define _YARP2_ABSTRACTCARRIER_

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/Logger.h>

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
    virtual bool prepareSend(ConnectionState& proto);
    virtual bool sendHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    // receiver
    virtual bool expectExtraHeader(ConnectionState& proto);
    virtual bool respondToHeader(ConnectionState& proto) = 0; // left abstract, no good default
    virtual bool expectIndex(ConnectionState& proto);
    virtual bool expectSenderSpecifier(ConnectionState& proto);
    virtual bool sendAck(ConnectionState& proto);
    virtual bool expectAck(ConnectionState& proto);

    virtual bool isActive();

    virtual void setCarrierParams(const yarp::os::Property& params);
    virtual void getCarrierParams(yarp::os::Property& params);

    // some default implementations of protocol phases used by
    // certain YARP carriers

    bool defaultSendHeader(ConnectionState& proto);
    bool defaultExpectIndex(ConnectionState& proto);
    bool defaultSendIndex(ConnectionState& proto, SizedWriter& writer);
    bool defaultExpectAck(ConnectionState& proto);
    bool defaultSendAck(ConnectionState& proto);

    int readYarpInt(ConnectionState& proto);
    void writeYarpInt(int n, ConnectionState& proto);

protected:
    int getSpecifier(const Bytes& b);
    void createStandardHeader(int specifier,const yarp::os::Bytes& header);
    virtual bool write(ConnectionState& proto, SizedWriter& writer);
    bool sendConnectionStateSpecifier(ConnectionState& proto);
    bool sendSenderSpecifier(ConnectionState& proto);

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
};

#endif
