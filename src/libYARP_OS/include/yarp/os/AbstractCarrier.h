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

    virtual Carrier *create() override = 0;

    virtual ConstString getName() override = 0;

    virtual bool checkHeader(const yarp::os::Bytes& header) override = 0;

    virtual void setParameters(const yarp::os::Bytes& header) override;

    virtual void getHeader(const yarp::os::Bytes& header) override = 0;


    virtual bool isConnectionless() override;
    virtual bool supportReply() override;
    virtual bool canAccept() override;
    virtual bool canOffer() override;
    virtual bool isTextMode() override;
    virtual bool requireAck() override;
    virtual bool canEscape() override;
    virtual bool isLocal() override;
    virtual ConstString toString() override;

    // sender
    virtual bool prepareSend(ConnectionState& proto) override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    // receiver
    virtual bool expectExtraHeader(ConnectionState& proto) override;
    virtual bool respondToHeader(ConnectionState& proto) override = 0; // left abstract, no good default
    virtual bool expectIndex(ConnectionState& proto) override;
    virtual bool expectSenderSpecifier(ConnectionState& proto) override;
    virtual bool sendAck(ConnectionState& proto) override;
    virtual bool expectAck(ConnectionState& proto) override;

    virtual bool isActive() override;

    virtual void setCarrierParams(const yarp::os::Property& params) override;
    virtual void getCarrierParams(yarp::os::Property& params) override;

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
    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;
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
