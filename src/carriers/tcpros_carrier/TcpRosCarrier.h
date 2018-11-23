/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    std::string kind;
    bool persistent;
    std::string wire_type;
    std::string user_type;
    std::string md5sum;
    std::string message_definition;

    std::string getRosType(ConnectionState& proto);

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

    Carrier *create() const override {
        return new TcpRosCarrier();
    }

    std::string getName() const override {
        return isService?"rossrv":"tcpros";
    }

    bool isConnectionless() const override {
        return false;
    }

    bool canAccept() const override {
        return true;
    }

    bool canOffer() const override {
        return true;
    }

    bool isTextMode() const override {
        return false;
    }

    bool isBareMode() const override {
        return true;
    }

    bool canEscape() const override {
        return false;
    }

    bool requireAck() const override {
        return false;
    }

    bool supportReply() const override {
        return true;
    }

    bool isPush() const override {
        // if topic-like, pull ; if service-like, push!
        return isService;
    }

    bool isLocal() const override {
        return false;
    }

    std::string toString() const override {
        return isService?"rossrv_carrier":"tcpros_carrier";
    }

    void getHeader(Bytes& header) const override {
        // no header, will need to do some fancy footwork
        const char *target = "NONONONO";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    bool checkHeader(const Bytes& header) override;

    void setParameters(const Bytes& header) override;

    // Now, the initial hand-shaking

    bool prepareSend(ConnectionState& proto) override {
        return true;
    }

    bool sendHeader(ConnectionState& proto) override;

    bool expectSenderSpecifier(ConnectionState& proto) override;

    bool expectExtraHeader(ConnectionState& proto) override {
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override {
        sender = false;
        return true;
    }

    bool expectReplyToHeader(ConnectionState& proto) override;

    bool isActive() const override {
        return true;
    }


    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) override;

    bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    bool expectIndex(ConnectionState& proto) override {
        return true;
    }

    bool sendAck(ConnectionState& proto) override {
        return true;
    }

    bool expectAck(ConnectionState& proto) override {
        return true;
    }

    std::string getBootstrapCarrierName() const override { return {}; }

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

    Carrier *create() const override {
        return new RosSrvCarrier();
    }
};

#endif
