/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef TCPROSCARRIER_INC
#define TCPROSCARRIER_INC

#include <yarp/os/Carrier.h>

#include "TcpRosStream.h"
#include "WireImage.h"
#include "WireBottle.h"
#include "WireTwiddler.h"
#include "RosHeader.h"

namespace yarp {
    namespace os {
        class TcpRosCarrier;
        class RosSrvCarrier;
    }
}

#define TCPROS_TRANSLATE_INHIBIT (-1)
#define TCPROS_TRANSLATE_UNKNOWN (0)
#define TCPROS_TRANSLATE_IMAGE (1)
#define TCPROS_TRANSLATE_BOTTLE_BLOB (2)
#define TCPROS_TRANSLATE_TWIDDLER (3)

class yarp::os::TcpRosCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
    int headerLen1;
    int headerLen2;
    int raw;
    int translate;
    WireImage wi;
    RosWireImage ri;
    SizedWriterTail wt;
    int seq;
    WireTwiddler twiddler;
    WireTwiddlerWriter twiddler_output;
    yarp::os::ConstString kind;
    bool persistent;
    ConstString wire_type;
    ConstString user_type;
    ConstString md5sum;
    ConstString message_definition;

    ConstString getRosType(ConnectionState& proto);

protected:
    bool isService;
public:
    TcpRosCarrier() {
        firstRound = true;
        sender = false;
        headerLen1 = 0;
        headerLen2 = 0;
        isService = false;
        raw = -1;
        translate = TCPROS_TRANSLATE_UNKNOWN;
        seq = 0;
        persistent = true;
    }

    virtual Carrier *create() override {
        return new TcpRosCarrier();
    }

    virtual ConstString getName() override {
        return isService?"rossrv":"tcpros";
    }

    virtual bool isConnectionless() override {
        return false;
    }

    virtual bool canAccept() override {
        return true;
    }

    virtual bool canOffer() override {
        return true;
    }

    virtual bool isTextMode() override {
        return false;
    }

    virtual bool isBareMode() override {
        return true;
    }

    virtual bool canEscape() override {
        return false;
    }

    virtual bool requireAck() override {
        return false;
    }

    virtual bool supportReply() override {
        return true;
    }

    virtual bool isPush() override {
        // if topic-like, pull ; if service-like, push!
        return isService;
    }

    virtual bool isLocal() override {
        return false;
    }

    virtual ConstString toString() override {
        return isService?"rossrv_carrier":"tcpros_carrier";
    }

    virtual void getHeader(const Bytes& header) override {
        // no header, will need to do some fancy footwork
        const char *target = "NONONONO";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) override;

    virtual void setParameters(const Bytes& header) override;

    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override {
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override;

    virtual bool expectExtraHeader(ConnectionState& proto) override {
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override {
        sender = false;
        return true;
    }

    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    virtual bool isActive() override {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) override {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override {
        return true;
    }

    virtual ConstString getBootstrapCarrierName() override { return ""; }

    virtual int connect(const yarp::os::Contact& src,
                        const yarp::os::Contact& dest,
                        const yarp::os::ContactStyle& style,
                        int mode,
                        bool reversed) override;

private:
    void processRosHeader(RosHeader& header);

};

/*
 *
 * Set up an explicit service carrier, so that we know the
 * direction of data flow as early as possible.  Its name
 * is "rossrv" (see TcpRosCarrier::getName)
 *
 */
class yarp::os::RosSrvCarrier : public TcpRosCarrier {
public:
    RosSrvCarrier() {
        isService = true;
    }

    virtual Carrier *create() override {
        return new RosSrvCarrier();
    }
};

#endif
