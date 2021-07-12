/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MJPEGCARRIER_INC
#define MJPEGCARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>
#include <yarp/os/ConnectionState.h>
#include "MjpegStream.h"
#include "MjpegLogComponent.h"

#include <cstring>

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
class MjpegCarrier :
        public yarp::os::Carrier
{
private:
    bool firstRound;
    bool sender;
    std::string envelope;
public:
    MjpegCarrier() {
        firstRound = true;
        sender = false;
    }

    Carrier *create() const override {
        return new MjpegCarrier();
    }

    std::string getName() const override {
        return "mjpeg";
    }

    bool isConnectionless() const override {
        return false;
    }

    bool canAccept() const override {
        return true;
    }

    bool canOffer() const override {
        return true;
    }

    bool isTextMode() const override {
        return false;
    }

    bool canEscape() const override {
        return false;
    }

    void handleEnvelope(const std::string& envelope) override {
        this->envelope = envelope;
    }

    bool requireAck() const override {
        return false;
    }

    bool supportReply() const override {
        return false;
    }

    bool isLocal() const override {
        return false;
    }

    // this is important - flips expected flow of messages
    bool isPush() const override {
        return false;
    }

    std::string toString() const override {
        return "mjpeg_carrier";
    }

    void getHeader(yarp::os::Bytes& header) const override {
        // GET /?action=stream HTTP/1.1
        const char *target = "GET /?ac";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    bool checkHeader(const yarp::os::Bytes& header) override {
        if (header.length()!=8) {
            return false;
        }
        const char *target = "GET /?ac";
        for (int i=0; i<8; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        yCTrace(MJPEGCARRIER, "Got header");
        return true;
    }

    void setParameters(const yarp::os::Bytes& header) override {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    bool prepareSend(yarp::os::ConnectionState& proto) override {
        // nothing special to do
        return true;
    }

    bool sendHeader(yarp::os::ConnectionState& proto) override;

    bool expectSenderSpecifier(yarp::os::ConnectionState& proto) override {
        return true;
    }

    bool expectExtraHeader(yarp::os::ConnectionState& proto) override {
        std::string txt;
        do {
            txt = proto.is().readLine();
        } while (txt!="");
        return true;
    }

    bool respondToHeader(yarp::os::ConnectionState& proto) override {
        std::string target = "HTTP/1.0 200 OK\r\n\
Connection: close\r\n\
Server: yarp/mjpeg_carrier/0.1\r\n\
Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n\
Pragma: no-cache\r\n\
Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n\
Content-Type: multipart/x-mixed-replace;boundary=boundarydonotcross\r\n\
\r\n\
--boundarydonotcross\r\n";
        yarp::os::Bytes b((char*)target.c_str(),strlen(target.c_str()));
        proto.os().write(b);
        sender = true; // this is a pull connection, not a push
        //MjpegStream *stream = new MjpegStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);
        return true;
    }

    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override {
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

    bool isActive() const override {
        return true;
    }


    // Payload time!

    bool write(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    bool reply(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) override;

    virtual bool sendIndex(yarp::os::ConnectionState& proto, yarp::os::SizedWriter& writer) {
        return true;
    }

    bool expectIndex(yarp::os::ConnectionState& proto) override {
        return true;
    }

    bool sendAck(yarp::os::ConnectionState& proto) override {
        return true;
    }

    bool expectAck(yarp::os::ConnectionState& proto) override {
        return true;
    }

    std::string getBootstrapCarrierName() const override { return {}; }

    virtual bool autoCompression() const;
};

#endif
