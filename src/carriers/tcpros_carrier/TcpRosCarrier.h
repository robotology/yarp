// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef TCPROSCARRIER_INC
#define TCPROSCARRIER_INC

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Protocol.h>

#include "TcpRosStream.h"

namespace yarp {
    namespace os {
        namespace impl {
            class TcpRosCarrier;
        }
    }
}

class yarp::os::impl::TcpRosCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
    int headerLen1;
    int headerLen2;
    bool isService;
public:
    TcpRosCarrier() {
        firstRound = true;
        sender = false;
        headerLen1 = 0;
        headerLen2 = 0;
        isService = false;
    }

    virtual Carrier *create() {
        return new TcpRosCarrier();
    }

    virtual String getName() {
        return "tcpros";
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
        printf("tcpros push? %d\n", isService);
        return isService;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return "tcpros_carrier";
    }

    virtual void getHeader(const Bytes& header) {
        // no header, will need to do some fancy footwork
        const char *target = "NONONONO";
        for (int i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header);

    virtual void setParameters(const Bytes& header);

    // Now, the initial hand-shaking

    virtual bool prepareSend(Protocol& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(Protocol& proto);

    virtual bool expectSenderSpecifier(Protocol& proto);

    virtual bool expectExtraHeader(Protocol& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(Protocol& proto) {
        sender = false;
        //TcpRosStream *stream = new TcpRosStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);
        printf("tcpros carrier not implemented yet\n");        
        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto);

    virtual bool isActive() {
        return true;
    }


    // Payload time!

    virtual bool write(Protocol& proto, SizedWriter& writer);

    virtual bool reply(Protocol& proto, SizedWriter& writer);

    virtual bool sendIndex(Protocol& proto) {
        return true;
    }

    virtual bool expectIndex(Protocol& proto) {
        return true;
    }

    virtual bool sendAck(Protocol& proto) {
        return true;
    }

    virtual bool expectAck(Protocol& proto) {
        return true;
    }
};

#endif
