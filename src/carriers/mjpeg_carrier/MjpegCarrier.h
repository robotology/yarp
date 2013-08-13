// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MJPEGCARRIER_INC
#define MJPEGCARRIER_INC

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/NetType.h>
#include "MjpegStream.h"

#include <string.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MjpegCarrier;
            class MjpegCarrierRaw;
        }
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
class yarp::os::impl::MjpegCarrier : public Carrier {
private:
    bool firstRound;
    bool sender;
public:
    MjpegCarrier() {
        firstRound = true;
        sender = false;
    }

    virtual Carrier *create() {
        return new MjpegCarrier();
    }

    virtual String getName() {
        return "mjpeg";
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
        return false;
    }

    virtual bool isLocal() {
        return false;
    }

    // this is important - flips expected flow of messages
    virtual bool isPush() {
        return false;
    }

    virtual String toString() {
        return "mjpeg_carrier";
    }

    virtual void getHeader(const Bytes& header) {
        // GET /?action=stream HTTP/1.1
        const char *target = "GET /?ac";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }
    }

    virtual bool checkHeader(const Bytes& header) {
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

    virtual void setParameters(const Bytes& header) {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto);

    virtual bool expectSenderSpecifier(ConnectionState& proto) {
        return true;
    }

    virtual bool expectExtraHeader(ConnectionState& proto) {
        String txt;
        do {
            txt = NetType::readLine(proto.is());
            //printf("Got rest of header: %s\n", txt.c_str());
        } while (txt!="");
        return true;
    }

    bool respondToHeader(ConnectionState& proto) {
        String target = "HTTP/1.0 200 OK\r\n\
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

    virtual bool expectReplyToHeader(ConnectionState& proto) {
        String txt;
        do {
            txt = NetType::readLine(proto.is());
            //printf("Got response to header: %s\n", txt.c_str());
        } while (txt!="");

        sender = false;
        MjpegStream *stream = new MjpegStream(proto.giveStreams(),sender,
                                              autoCompression());
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer);

    virtual bool reply(ConnectionState& proto, SizedWriter& writer);

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) {
        return true;
    }

    virtual String getBootstrapCarrierName() { return ""; }

    virtual bool autoCompression() const;
};

#endif
