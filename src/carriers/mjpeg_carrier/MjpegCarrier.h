/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MJPEGCARRIER_INC
#define MJPEGCARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>
#include "MjpegStream.h"

#include <cstring>

namespace yarp {
    namespace os {
        class MjpegCarrier;
    }
}

/**
 *
 * A carrier for sending/receiving images via mjpeg over http.
 *
 * Picking a random webcam stream:
 *  http://cam.mauirealestate.net:8010/axis-cgi/mjpg/video.cgi?resolution=320x240
 * (it will probably be dead by the time you read this, but finding others
 * is easy...)
 * Register this:
 *   yarp name register /webcam mjpeg+path.axis-cgi/mjpg/video.cgi?resolution=320x240 cam.mauirealestate.net 8010
 * Make a viewer:
 *   yarpview /view
 * Connect:
 *   yarp connect /webcam /view
 * You can also view yarp image ports from a browser.  Do a "yarp name query /portname" to find their port number NNN, then go to:
 *   http://localhost:NNN/?output=stream
 *
 */
class yarp::os::MjpegCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
    std::string envelope;
public:
    MjpegCarrier() {
        firstRound = true;
        sender = false;
    }

    virtual Carrier *create() override {
        return new MjpegCarrier();
    }

    virtual std::string getName() const override {
        return "mjpeg";
    }

    virtual bool isConnectionless() const override {
        return false;
    }

    virtual bool canAccept() override {
        return true;
    }

    virtual bool canOffer() override {
        return true;
    }

    virtual bool isTextMode() const override {
        return false;
    }

    virtual bool canEscape() override {
        return false;
    }

    virtual void handleEnvelope(const std::string& envelope) override {
        this->envelope = envelope;
    }

    virtual bool requireAck() override {
        return false;
    }

    virtual bool supportReply() override {
        return false;
    }

    virtual bool isLocal() override {
        return false;
    }

    // this is important - flips expected flow of messages
    virtual bool isPush() override {
        return false;
    }

    virtual std::string toString() override {
        return "mjpeg_carrier";
    }

    virtual void getHeader(const Bytes& header) override {
        // GET /?action=stream HTTP/1.1
        const char *target = "GET /?ac";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) override {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "GET /?ac";
        for (int i=0; i<8; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        //printf("Got header\n");
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
        return true;
    }

    virtual bool expectExtraHeader(ConnectionState& proto) override {
        std::string txt;
        do {
            txt = proto.is().readLine();
        } while (txt!="");
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override {
        std::string target = "HTTP/1.0 200 OK\r\n\
Connection: close\r\n\
Server: yarp/mjpeg_carrier/0.1\r\n\
Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n\
Pragma: no-cache\r\n\
Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n\
Content-Type: multipart/x-mixed-replace;boundary=boundarydonotcross\r\n\
\r\n\
--boundarydonotcross\r\n";
        Bytes b((char*)target.c_str(),strlen(target.c_str()));
        proto.os().write(b);
        sender = true; // this is a pull connection, not a push
        //MjpegStream *stream = new MjpegStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);
        return true;
    }

    virtual bool expectReplyToHeader(ConnectionState& proto) override {
        std::string txt;
        do {
            txt = proto.is().readLine();
        } while (txt!="");

        sender = false;
        MjpegStream *stream = new MjpegStream(proto.giveStreams(),
                                              autoCompression());
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() override {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) override {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override {
        return true;
    }

    virtual std::string getBootstrapCarrierName() override { return ""; }

    virtual bool autoCompression() const;
};

#endif
