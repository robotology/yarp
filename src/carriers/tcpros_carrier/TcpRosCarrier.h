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
public:
    TcpRosCarrier() {
        firstRound = true;
        sender = false;
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

    // The initiator of a tcpros topic connection may not retain the initiative.
    virtual bool autoReverse() {
        return true;
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

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "NONONONO";
        for (int i=0; i<8; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        return true;
    }

    virtual void setParameters(const Bytes& header) {
        // no parameters - no carrier variants
    }

    // Now, the initial hand-shaking

    virtual bool prepareSend(Protocol& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(Protocol& proto) {
        // NO HEADER -- WAIT

        //String target = "NONONONO";
        //Bytes b((char*)target.c_str(),8);
        //proto.os().write(b);
        return true;
    }

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

    virtual bool expectReplyToHeader(Protocol& proto) {
        sender = true;
        //TcpRosStream *stream = new TcpRosStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);
        printf("tcpros carrier not implemented yet\n");        
        return true;
    }

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
