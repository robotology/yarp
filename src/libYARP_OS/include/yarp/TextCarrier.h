// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TEXTCARRIER_
#define _YARP2_TEXTCARRIER_

#include <yarp/TcpCarrier.h>

namespace yarp {
    class TextCarrier;
}

/**
 * Communicating between two ports via a plain-text protocol.
 */
class yarp::TextCarrier : public TcpCarrier {
private:
    bool ackVariant;
public:
    TextCarrier(bool ackVariant = false) {
        this->ackVariant = ackVariant;
    }

    virtual String getName() {
        if (ackVariant) {
            return "text_ack";
        }
        return "text";
    }

    virtual String getSpecifierName() {
        if (ackVariant) {
            return "CONNACK ";
        }
        return "CONNECT ";
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                if (!(target[i]==header.get()[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = getSpecifierName();
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }   
    }

    virtual Carrier *create() {
        return new TextCarrier(ackVariant);
    }

    virtual bool requireAck() {
        return ackVariant;
    }

    virtual bool isTextMode() {
        return true;
    }


    virtual bool supportReply() {
        return requireAck();
    }

    virtual bool sendHeader(Protocol& proto) {
        String target = getSpecifierName();
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        String from = proto.getRoute().getFromName();
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().write('\r');
        proto.os().write('\n');
        proto.os().flush();
        return proto.os().isOk();
    }

    bool expectReplyToHeader(Protocol& proto) {
        if (ackVariant) {
            // expect and ignore welcome line
            String result = NetType::readLine(proto.is());
        }
        return true;
    }

    bool expectSenderSpecifier(Protocol& proto) {
        ACE_DEBUG((LM_DEBUG,"TextCarrier::expectSenderSpecifier"));
        proto.setRoute(proto.getRoute().addFromName(NetType::readLine(proto.is())));
        return true;
    }

    bool sendIndex(Protocol& proto) {
        return true;
    }

    bool expectIndex(Protocol& proto) {
        return true;
    }

    bool sendAck(Protocol& proto) {
        if (ackVariant) {
            String from = "<ACK>\r\n";
            Bytes b2((char*)from.c_str(),from.length());
            proto.os().write(b2);
            proto.os().flush();
        }
        return proto.os().isOk();
    }

    virtual bool expectAck(Protocol& proto) {
        if (ackVariant) {
            // expect and ignore acknowledgement
            String result = NetType::readLine(proto.is());
        }
        return true;
    }

    bool respondToHeader(Protocol& proto) {
        String from = "Welcome ";
        from += proto.getRoute().getFromName();
        from += "\r\n";
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().flush();
        return proto.os().isOk();
    }
};

#endif
