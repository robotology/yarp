/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/Carrier.h>
#include <yarp/os/Carriers.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/Protocol.h>

#include <iostream>
#include <string>

/**
 *
 * Get a Human to input/output data.  We'll only deal with text.
 *
 */
class HumanStream : public yarp::os::TwoWayStream, public yarp::os::InputStream, public yarp::os::OutputStream {
private:
    bool interrupting;
    bool needInterrupt;
    std::string inputCache;
    std::string outputCache;
public:
    HumanStream() {
        interrupting = false;
        needInterrupt = false;
        inputCache = outputCache = "";
    }

    virtual void close() {
        yInfo() << "Bye bye";
    }

    virtual bool isOk() {
        return true;
    }

    virtual void interrupt() {
        interrupting = true;
        while (needInterrupt) {
            yInfo() << "*** INTERRUPT: Please hit enter ***";
            for (int i=0; i<10 && needInterrupt; i++) {
                yarp::os::Time::delay(0.1);
            }
        }
    }

    /////////////////////////////////////////////////
    // InputStream

    virtual ssize_t read(yarp::os::Bytes& b) {
        if (interrupting) { return -1; }
        while (inputCache.size() < b.length()) {
            yInfo() <<"*** CHECK OTHER TERMINAL FOR SOMETHING TO TYPE:";
            char buf[1000];
            needInterrupt = true;  // should be mutexed, in real implementation
            std::cin.getline(buf,1000);
            needInterrupt = false;
            if (interrupting) { return -1; }
            inputCache += buf;
            inputCache += "\r\n";
            yInfo() << "Thank you";
        }
        memcpy(b.get(),inputCache.c_str(),b.length());
        inputCache = inputCache.substr(b.length());
        return b.length();
    }

    /////////////////////////////////////////////////
    // OutputStream

    virtual void write(const yarp::os::Bytes& b) {
        outputCache.append(b.get(),b.length());
        while (outputCache.find("\n")!=std::string::npos) {
            size_t idx = outputCache.find("\n");
            std::string show;
            show.append(outputCache.c_str(),idx);
            yInfo() << "*** TYPE THIS ON THE OTHER TERMINAL: " << show;
            outputCache = outputCache.substr(idx+1);
            yarp::os::Time::delay(1);
        }
    }

    /////////////////////////////////////////////////
    // TwoWayStream

    virtual InputStream& getInputStream() {
        return *this;
    }

    virtual OutputStream& getOutputStream() {
        return *this;
    }

    virtual const yarp::os::Contact& getLocalAddress() {
        // left undefined
        return local;
    }

    virtual const yarp::os::Contact& getRemoteAddress() {
        // left undefined
        return remote;
    }

    virtual void reset() {
        inputCache = outputCache = "";
        yInfo() << "Stream reset";
    }

    virtual void beginPacket() {
        yInfo() << "Packet begins";
        inputCache = "";
        outputCache = "";
    }

    virtual void endPacket() {
        yInfo() << "Packet ends";
    }

private:
    yarp::os::Contact local, remote;
};

/**
 *
 * A completely new carrier.  Get a Human to carry the data!
 *
 */
class HumanCarrier : public yarp::os::Carrier {
public:

    /////////////////////////////////////////////////
    // First, the easy bits...

    virtual yarp::os::Carrier *create() const {
        return new HumanCarrier();
    }

    virtual std::string getName() const {
        return "human";
    }

    virtual bool isConnectionless() const {
        return true;
    }

    virtual bool canAccept() const {
        return true;
    }

    virtual bool canOffer() const {
        return true;
    }

    virtual bool isTextMode() const {
        // let's be text mode, for human-friendliness
        return true;
    }

    virtual bool canEscape() const {
        return true;
    }

    virtual bool requireAck() const {
        return true;
    }

    virtual bool supportReply() const {
        return true;
    }

    virtual bool isLocal() const {
        return false;
    }

    virtual std::string toString() const {
        return "humans are handy";
    }

    virtual void getHeader(const yarp::os::Bytes& header) {
        const char *target = "HUMANITY";
        for (int i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const yarp::os::Bytes& header) {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "HUMANITY";
        for (int i=0; i<8; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        return true;
    }

    virtual void setParameters(const yarp::os::Bytes& header) {
        // no parameters - no carrier variants
    }


    /////////////////////////////////////////////////
    // Now, the initial hand-shaking

    virtual bool prepareSend(yarp::os::ConnectionState& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(yarp::os::ConnectionState& proto) {
        // Send the "magic number" for this carrier
        yarp::os::ManagedBytes header(8);
        getHeader(header.bytes());
        proto.os().write(header.bytes());
        if (!proto.os().isOk()) return false;

        // Now we can do whatever we want, as long as somehow
        // we also send the name of the originating port

        // let's just send the port name in plain text terminated with a
        // carriage-return / line-feed
        std::string from = proto.getRoute().getFromName();
        yarp::os::Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().write('\r');
        proto.os().write('\n');
        proto.os().flush();
        return proto.os().isOk();
    }

    virtual bool expectSenderSpecifier(yarp::os::ConnectionState& proto) {
        // interpret everything that sendHeader wrote
        proto.setRoute(proto.getRoute().addFromName(proto.is().readLine()));
        return proto.is().isOk();
    }

    virtual bool expectExtraHeader(yarp::os::ConnectionState& proto) {
        // interpret any extra header information sent - optional
        return true;
    }

    virtual bool respondToHeader(yarp::os::ConnectionState& proto) {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(yarp::os::ConnectionState& proto) {
        // SWITCH TO NEW STREAM TYPE
        HumanStream *stream = new HumanStream();
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() {
        return true;
    }


    /////////////////////////////////////////////////
    // Payload time!

    virtual bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) {
        bool ok = sendIndex(proto);
        if (!ok) return false;
        writer.write(proto.os());
        return proto.os().isOk();
    }

    virtual bool sendIndex(yarp::os::ConnectionState& proto) {
        std::string prefix = "human says ";
        yarp::os::Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectIndex(yarp::os::ConnectionState& proto) {
        std::string prefix = "human says ";
        std::string compare = prefix;
        yarp::os::Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) yInfo() << "YOU DID NOT SAY 'human says '";
        return ok;
    }

    /////////////////////////////////////////////////
    // Acknowledgements, we don't do them

    virtual bool sendAck(yarp::os::ConnectionState& proto) {
        std::string prefix = "computers rule!\r\n";
        yarp::os::Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.os().write(b2);
        return true;
    }

    virtual bool expectAck(yarp::os::ConnectionState& proto) {
        std::string prefix = "computers rule!\r\n";
        std::string compare = prefix;
        yarp::os::Bytes b2((char*)prefix.c_str(),prefix.length());
        proto.is().read(b2);
        bool ok = proto.is().isOk() && (prefix==compare);
        if (!ok) yInfo() << "YOU DID NOT SAY 'computers rule!'";
        return ok;
    }

};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;
    yarp::os::Carriers::addCarrierPrototype(new HumanCarrier);

    if (argc<2) {
        yInfo() << "Please run in two terminals as:\n";
        yInfo() << "  carrier_human --server\n";
        yInfo() << "  carrier_human --client\n";
        std::exit(1);
    }

    std::string mode = argv[1];

    if (mode == "--server") {
        yarp::os::Port out;
        out.open("/test/out");
        bool connected = false;
        while (!connected) {
            connected = yarp::os::Network::connect("/test/out","/test/in","human");
            if (!connected) yarp::os::Time::delay(1);
        }
        yarp::os::Bottle bot;
        bot.fromString("1 2 3");
        out.write(bot);
        out.close();
    }

    if (mode == "--client") {
        yarp::os::Port in;
        in.open("/test/in");
        yarp::os::Bottle bot;
        in.read(bot);
        yInfo() << "Got message " << bot.toString().c_str();
        in.close();
    }

    return 0;
}
