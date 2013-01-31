// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "HumanStream.h"

class HumanCarrier : public Carrier {
public:

    // First, the easy bits...

    virtual Carrier *create() {
        return new HumanCarrier();
    }

    virtual String getName() {
        return "human";
    }

    virtual bool isConnectionless() {
        return true;
    }

    virtual bool canAccept() {
        return true;
    }

    virtual bool canOffer() {
        return true;
    }

    virtual bool isTextMode() {
        // let's be text mode, for human-friendliness
        return true;
    }

    virtual bool canEscape() {
        return true;
    }

    virtual bool requireAck() {
        return true;
    }

    virtual bool supportReply() {
        return true;
    }

    virtual bool isLocal() {
        return false;
    }

    virtual String toString() {
        return "humans are handy";
    }

    virtual void getHeader(const Bytes& header) {
        const char *target = "HUMANITY";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "HUMANITY";
        for (size_t i=0; i<8; i++) {
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

    virtual bool sendHeader(Protocol& proto);

    virtual bool expectSenderSpecifier(Protocol& proto) {
        // interpret everything that sendHeader wrote
        proto.setRoute(proto.getRoute().addFromName(NetType::readLine(proto.is())));
        return proto.is().isOk();
    }

    virtual bool expectExtraHeader(Protocol& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    virtual bool respondToHeader(Protocol& proto) {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() {
        return true;
    }


    // Payload time!

    virtual bool write(Protocol& proto, SizedWriter& writer) {
        bool ok = sendIndex(proto);
        if (!ok) return false;
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(Protocol& proto) {
        String prefix = "human says ";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectIndex(Protocol& proto) {
        String prefix = "human says ";
        String compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) cout << "YOU DID NOT SAY 'human says '" << endl;
        return ok;
    }

    // Acknowledgements, we don't do them

    virtual bool sendAck(Protocol& proto) {
        String prefix = "computers rule!\r\n";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectAck(Protocol& proto) {
        String prefix = "computers rule!\r\n";
        String compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) cout << "YOU DID NOT SAY 'computers rule!'" << endl;
        return ok;
    }

};
