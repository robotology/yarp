/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/Protocol.h>

#include <iostream>
#include <string>

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;


/**
 *
 * Get a Human to input/output data.  We'll only deal with text.
 *
 */
class HumanStream : public TwoWayStream, public InputStream, public OutputStream {
private:
    bool interrupting;
    bool needInterrupt;
    string inputCache;
    string outputCache;
public:
    HumanStream() {
        interrupting = false;
        needInterrupt = false;
        inputCache = outputCache = "";
    }

    virtual void close() {
        cout << "Bye bye" << endl;
    }

    virtual bool isOk() {
        return true;
    }

    virtual void interrupt() {
        interrupting = true;
        while (needInterrupt) {
            cout << "*** INTERRUPT: Please hit enter ***" << endl;
            for (int i=0; i<10 && needInterrupt; i++) {
                Time::delay(0.1);
            }
        }
    }

    /////////////////////////////////////////////////
    // InputStream

    virtual int read(const Bytes& b) {
        if (interrupting) { return -1; }
        while (inputCache.size() < b.length()) {
            cout << "*** CHECK OTHER TERMINAL FOR SOMETHING TO TYPE:"
                 << endl;
            char buf[1000];
            needInterrupt = true;  // should be mutexed, in real implementation
            cin.getline(buf,1000);
            needInterrupt = false;
            if (interrupting) { return -1; }
            inputCache += buf;
            inputCache += "\r\n";
            cout << "Thank you" << endl;
        }
        memcpy(b.get(),inputCache.c_str(),b.length());
        inputCache = inputCache.substr(b.length());
        return b.length();
    }

    /////////////////////////////////////////////////
    // OutputStream

    virtual void write(const Bytes& b) {
        outputCache.append(b.get(),b.length());
        while (outputCache.find("\n")!=string::npos) {
            size_t idx = outputCache.find("\n");
            string show;
            show.append(outputCache.c_str(),idx);
            cout << "*** TYPE THIS ON THE OTHER TERMINAL: " << show << endl;
            outputCache = outputCache.substr(idx+1);
            Time::delay(1);
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

    virtual const Address& getLocalAddress() {
        // left undefined
        return local;
    }

    virtual const Address& getRemoteAddress() {
        // left undefined
        return remote;
    }

    virtual void reset() {
        inputCache = outputCache = "";
        cout << "Stream reset" << endl;
    }

    virtual void beginPacket() {
        cout << "Packet begins" << endl;
        inputCache = "";
        outputCache = "";
    }

    virtual void endPacket() {
        cout << "Packet ends" << endl;
    }

private:
    Address local, remote;
};

/**
 *
 * A completely new carrier.  Get a Human to carry the data!
 *
 */
class HumanCarrier : public Carrier {
public:

    /////////////////////////////////////////////////
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
        for (int i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) {
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

    virtual void setParameters(const Bytes& header) {
        // no parameters - no carrier variants
    }


    /////////////////////////////////////////////////
    // Now, the initial hand-shaking

    virtual bool prepareSend(Protocol& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(Protocol& proto) {
        // Send the "magic number" for this carrier
        ManagedBytes header(8);
        getHeader(header.bytes());
        proto.os().write(header.bytes());
        if (!proto.os().isOk()) return false;

        // Now we can do whatever we want, as long as somehow
        // we also send the name of the originating port

        // let's just send the port name in plain text terminated with a
        // carriage-return / line-feed
        String from = proto.getRoute().getFromName();
        Bytes b2((char*)from.c_str(),from.length());
        proto.os().write(b2);
        proto.os().write('\r');
        proto.os().write('\n');
        proto.os().flush();
        return proto.os().isOk();
    }

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


    /////////////////////////////////////////////////
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

    /////////////////////////////////////////////////
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

int main(int argc, char *argv[]) {
    Network yarp;
    Carriers::addCarrierPrototype(new HumanCarrier);

    if (argc<2) {
        printf("Please run in two terminals as:\n");
        printf("  carrier_human --server\n");
        printf("  carrier_human --client\n");
        exit(1);
    }

    string mode = argv[1];

    if (mode == "--server") {
        Port out;
        out.open("/test/out");
        bool connected = false;
        while (!connected) {
            connected = Network::connect("/test/out","/test/in","human");
            if (!connected) Time::delay(1);
        }
        Bottle bot;
        bot.fromString("1 2 3");
        out.write(bot);
        out.close();
    }

    if (mode == "--client") {
        Port in;
        in.open("/test/in");
        Bottle bot;
        in.read(bot);
        printf("Got message %s\n", bot.toString().c_str());
        in.close();
    }

    return 0;
}


