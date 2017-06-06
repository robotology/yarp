/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_ABSTRACTCARRIER_H
#define YARP_OS_ABSTRACTCARRIER_H

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>

namespace yarp {
    namespace os {
        class AbstractCarrier;
    }
}

/**
 * \brief A starter class for implementing simple carriers.
 *
 * It implements reasonable default behavior.
 */
class YARP_OS_API yarp::os::AbstractCarrier : public Carrier {
public:

    virtual Carrier *create() YARP_OVERRIDE = 0;

    virtual ConstString getName() YARP_OVERRIDE = 0;

    virtual bool checkHeader(const yarp::os::Bytes& header) YARP_OVERRIDE = 0;

    virtual void setParameters(const yarp::os::Bytes& header) YARP_OVERRIDE;

    virtual void getHeader(const yarp::os::Bytes& header) YARP_OVERRIDE = 0;


    virtual bool isConnectionless() YARP_OVERRIDE;
    virtual bool supportReply() YARP_OVERRIDE;
    virtual bool canAccept() YARP_OVERRIDE;
    virtual bool canOffer() YARP_OVERRIDE;
    virtual bool isTextMode() YARP_OVERRIDE;
    virtual bool requireAck() YARP_OVERRIDE;
    virtual bool canEscape() YARP_OVERRIDE;
    virtual bool isLocal() YARP_OVERRIDE;
    virtual ConstString toString() YARP_OVERRIDE;

    // sender
    virtual bool prepareSend(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool sendHeader(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool expectReplyToHeader(ConnectionState& proto) YARP_OVERRIDE;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    // receiver
    virtual bool expectExtraHeader(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool respondToHeader(ConnectionState& proto) YARP_OVERRIDE = 0; // left abstract, no good default
    virtual bool expectIndex(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool expectSenderSpecifier(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool sendAck(ConnectionState& proto) YARP_OVERRIDE;
    virtual bool expectAck(ConnectionState& proto) YARP_OVERRIDE;

    virtual bool isActive() YARP_OVERRIDE;

    virtual void setCarrierParams(const yarp::os::Property& params) YARP_OVERRIDE;
    virtual void getCarrierParams(yarp::os::Property& params) YARP_OVERRIDE;

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
    void createStandardHeader(int specifier, const yarp::os::Bytes& header);
    virtual bool write(ConnectionState& proto, SizedWriter& writer) YARP_OVERRIDE;
    bool sendConnectionStateSpecifier(ConnectionState& proto);
    bool sendSenderSpecifier(ConnectionState& proto);

    static int interpretYarpNumber(const yarp::os::Bytes& b) {
        if (b.length()==8) {
            char *base = b.get();
            if (base[0]=='Y' && base[1]=='A' &&
                base[6]=='R' && base[7]=='P') {
                yarp::os::Bytes b2(b.get()+2, 4);
                int x = NetType::netInt(b2);
                return x;
            }
        }
        return -1;
    }

    static void createYarpNumber(int x, const yarp::os::Bytes& header) {
        if (header.length()!=8) {
            return;
        }
        char *base = header.get();
        base[0] = 'Y';
        base[1] = 'A';
        base[6] = 'R';
        base[7] = 'P';
        yarp::os::Bytes code(base+2, 4);
        NetType::netInt(x, code);
    }
};

#endif // YARP_OS_ABSTRACTCARRIER_H
