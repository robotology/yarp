/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Carrier.h>
#include <yarp/os/Route.h>
#include <yarp/os/SizedWriter.h>
#include "HumanStream.h"

class HumanCarrier : public Carrier {
public:

    // First, the easy bits...

    Carrier *create() const override {
        return new HumanCarrier();
    }

    std::string getName() const override {
        return "human";
    }

    bool isConnectionless() const override {
        return true;
    }

    bool canAccept() const override {
        return true;
    }

    bool canOffer() const override {
        return true;
    }

    bool isTextMode() const override {
        // let's be text mode, for human-friendliness
        return true;
    }

    bool canEscape() const override {
        return true;
    }

    bool requireAck() const override {
        return true;
    }

    bool supportReply() const override {
        return true;
    }

    bool isLocal() const override {
        return false;
    }

    std::string toString() const override {
        return "humans are handy";
    }

    void getHeader(Bytes& header) const override {
        const char *target = "HUMANITY";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    bool checkHeader(const Bytes& header) override {
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

    void setParameters(const Bytes& header) override {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    bool prepareSend(ConnectionState& proto) override {
        // nothing special to do
        return true;
    }

    bool sendHeader(ConnectionState& proto) override;

    bool expectSenderSpecifier(ConnectionState& proto) override {
        // interpret everything that sendHeader wrote
        Route route = proto.getRoute();
        route.setFromName(proto.is().readLine());
        proto.setRoute(route);
        return proto.is().isOk();
    }

    bool expectExtraHeader(ConnectionState& proto) override {
        // interpret any extra header information sent - optional
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    bool expectReplyToHeader(ConnectionState& proto) override {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    bool isActive() const override {
        return true;
    }


    // Payload time!

    bool write(ConnectionState& proto, SizedWriter& writer) override {
        bool ok = sendIndex(proto,writer);
        if (!ok) {
            return false;
        }
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(ConnectionState& proto,SizedWriter& writer) {
        std::string prefix = "human says ";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    bool expectIndex(ConnectionState& proto) override {
        std::string prefix = "human says ";
        std::string compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) {
            std::cout << "YOU DID NOT SAY 'human says '" << std::endl;
        }
        return ok;
    }

    // Acknowledgements, we don't do them

    bool sendAck(ConnectionState& proto) override {
        std::string prefix = "computers rule!\r\n";
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    bool expectAck(ConnectionState& proto) override {
        std::string prefix = "computers rule!\r\n";
        std::string compare = prefix;
        Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) {
            std::cout << "YOU DID NOT SAY 'computers rule!'" << std::endl;
        }
        return ok;
    }

};
