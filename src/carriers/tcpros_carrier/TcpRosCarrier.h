// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef TCPROSCARRIER_INC
#define TCPROSCARRIER_INC

#include <yarp/os/impl/Carrier.h>

#include "TcpRosStream.h"
#include "WireImage.h"
#include "WireBottle.h"
#include "WireTwiddler.h"

namespace yarp {
    namespace os {
        namespace impl {
            class TcpRosCarrier;
            class RosSrvCarrier;
        }
    }
}

#define TCPROS_TRANSLATE_INHIBIT (-1)
#define TCPROS_TRANSLATE_UNKNOWN (0)
#define TCPROS_TRANSLATE_IMAGE (1)
#define TCPROS_TRANSLATE_BOTTLE_BLOB (2)
#define TCPROS_TRANSLATE_TWIDDLER (3)

class yarp::os::impl::TcpRosCarrier : public Carrier {
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
    }

    virtual Carrier *create() {
        return new TcpRosCarrier();
    }

    virtual String getName() {
        return isService?"rossrv":"tcpros";
    }

    virtual bool isConnectionless() {
        return false;
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

    virtual bool canEscape() {
        return false;
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool supportReply() {
        return true;
    }

    virtual bool isPush() {
        // if topic-like, pull ; if service-like, push!
        return isService;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return isService?"rossrv_carrier":"tcpros_carrier";
    }

    virtual void getHeader(const Bytes& header) {
        // no header, will need to do some fancy footwork
        const char *target = "NONONONO";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header);

    virtual void setParameters(const Bytes& header);

    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto);

    virtual bool expectSenderSpecifier(ConnectionState& proto);

    virtual bool expectExtraHeader(ConnectionState& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(ConnectionState& proto) {
        sender = false;
        //TcpRosStream *stream = new TcpRosStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(ConnectionState& proto);

    virtual bool isActive() {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer);

    virtual bool reply(ConnectionState& proto, SizedWriter& writer);

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) {
        return true;
    }

    virtual String getBootstrapCarrierName() { return ""; }

    virtual int connect(const yarp::os::Contact& src,
                        const yarp::os::Contact& dest,
                        const yarp::os::ContactStyle& style,
                        int mode,
                        bool reversed);

};

/*
 *
 * Set up an explicit service carrier, so that we know the
 * direction of data flow as early as possible.  Its name
 * is "rossrv" (see TcpRosCarrier::getName)
 *
 */
class yarp::os::impl::RosSrvCarrier : public TcpRosCarrier {
public:
    RosSrvCarrier() {
        isService = true;
    }

    virtual Carrier *create() {
        return new RosSrvCarrier();
    }
};

#endif
