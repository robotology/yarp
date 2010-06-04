// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef XMLRPCCARRIER_INC
#define XMLRPCCARRIER_INC

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Protocol.h>
#include "XmlRpcStream.h"

namespace yarp {
    namespace os {
        namespace impl {
            class XmlRpcCarrier;
        }
    }
}

class yarp::os::impl::XmlRpcCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
public:
    XmlRpcCarrier() {
        firstRound = true;
        sender = false;
    }

    virtual Carrier *create() {
        return new XmlRpcCarrier();
    }

    virtual String getName() {
        return "xmlrpc";
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
        return true;
    }

    virtual bool canEscape() {
        return true;
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool supportReply() {
        return true;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return "xmlrpc_carrier";
    }

    virtual void getHeader(const Bytes& header) {
        const char *target = "POST /RC";
        for (int i=0; i<6 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) {
        // for now, expect XMLRPC++, which posts with uri /RPC2
        if (header.length()!=8) {
            return false;
        }
        const char *target = "POST /";
        for (int i=0; i<6; i++) {
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
        String target = "POST /RP";
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        return true;
    }

    virtual bool expectSenderSpecifier(Protocol& proto);

    virtual bool expectExtraHeader(Protocol& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(Protocol& proto) {
        sender = false;
        XmlRpcStream *stream = new XmlRpcStream(proto.giveStreams(),sender);
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        sender = true;
        XmlRpcStream *stream = new XmlRpcStream(proto.giveStreams(),sender);
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
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
