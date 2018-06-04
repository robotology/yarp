/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "HumanStream.h"

class HumanCarrier : public Carrier {
public:

    // First, the easy bits...

    virtual Carrier *create() override {
        return new HumanCarrier();
    }

    virtual std::string getName() const override {
        return "human";
    }

    virtual bool isConnectionless() const override {
        return true;
    }

    virtual bool canAccept() override {
        return true;
    }

    virtual bool canOffer() override {
        return true;
    }

    virtual bool isTextMode() const override {
        // let's be text mode, for human-friendliness
        return true;
    }

    virtual bool canEscape() override {
        return true;
    }

    virtual bool requireAck() override {
        return true;
    }

    virtual bool supportReply() override {
        return true;
    }

    virtual bool isLocal() override {
        return false;
    }

    virtual std::string toString() override {
        return "humans are handy";
    }

    virtual void getHeader(const Bytes& header) override {
        const char *target = "HUMANITY";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) override {
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

    virtual void setParameters(const Bytes& header) override {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override {
        // interpret everything that sendHeader wrote
        Route route = proto.getRoute();
        route.setFromName(proto.is().readLine());
        proto.setRoute(route);
        return proto.is().isOk();
    }

    virtual bool expectExtraHeader(ConnectionState& proto) override {
        // interpret any extra header information sent - optional
        return true;
    }

    virtual bool respondToHeader(ConnectionState& proto) override {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(ConnectionState& proto) override {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() override {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override {
        bool ok = sendIndex(proto,writer);
        if (!ok) return false;
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(ConnectionState& proto,SizedWriter& writer) {
        std::string prefix = "human says ";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override {
        std::string prefix = "human says ";
        std::string compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) std::cout << "YOU DID NOT SAY 'human says '" << std::endl;
        return ok;
    }

    // Acknowledgements, we don't do them

    virtual bool sendAck(ConnectionState& proto) override {
        std::string prefix = "computers rule!\r\n";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override {
        std::string prefix = "computers rule!\r\n";
        std::string compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) std::cout << "YOU DID NOT SAY 'computers rule!'" << std::endl;
        return ok;
    }

};
