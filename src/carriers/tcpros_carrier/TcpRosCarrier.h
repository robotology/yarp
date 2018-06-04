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

    virtual Carrier *create() override {
        return new TcpRosCarrier();
    }

    virtual std::string getName() const override {
        return isService?"rossrv":"tcpros";
    }

    virtual bool isConnectionless() const override {
        return false;
    }

    virtual bool canAccept() const override {
        return true;
    }

    virtual bool canOffer() const override {
        return true;
    }

    virtual bool isTextMode() const override {
        return false;
    }

    virtual bool isBareMode() const override {
        return true;
    }

    virtual bool canEscape() const override {
        return false;
    }

    virtual bool requireAck() const override {
        return false;
    }

    virtual bool supportReply() const override {
        return true;
    }

    virtual bool isPush() const override {
        // if topic-like, pull ; if service-like, push!
        return isService;
    }

    virtual bool isLocal() const override {
        return false;
    }

    virtual std::string toString() const override {
        return isService?"rossrv_carrier":"tcpros_carrier";
    }

    virtual void getHeader(Bytes& header) const override {
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

    virtual bool isActive() const override {
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

    virtual std::string getBootstrapCarrierName() const override { return ""; }

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
