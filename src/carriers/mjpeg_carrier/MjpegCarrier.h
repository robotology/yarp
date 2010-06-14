// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MJPEGCARRIER_INC
#define MJPEGCARRIER_INC

#include <yarp/os/impl/Carrier.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/NetType.h>
#include "MjpegStream.h"

#include <string.h>

namespace yarp {
    namespace os {
        namespace impl {
            class MjpegCarrier;
        }
    }
}

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
        for (int i=0; i<8 && i<header.length(); i++) {
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

    virtual bool prepareSend(Protocol& proto) {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(Protocol& proto) {
        String target = "GET /?action=stream HTTP/1.1\n\n";
        Bytes b((char*)target.c_str(),strlen(target.c_str()));
        proto.os().write(b);
        return true;
    }

    virtual bool expectSenderSpecifier(Protocol& proto) {
        return true;
    }

    virtual bool expectExtraHeader(Protocol& proto) {
        String txt;
        do {
            txt = NetType::readLine(proto.is());
            //printf("Got rest of header: %s\n", txt.c_str());
        } while (txt!="");
        return true;
    }

    bool respondToHeader(Protocol& proto) {
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
        printf("*** mjpeg carrier is *very* experimental\n");
        return true;
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        String txt;
        do {
            txt = NetType::readLine(proto.is());
            //printf("Got response to header: %s\n", txt.c_str());
        } while (txt!="");

        sender = false;
        MjpegStream *stream = new MjpegStream(proto.giveStreams(),sender);
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        printf("*** mjpeg carrier is *very* experimental\n");
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

    virtual String getBootstrapCarrierName() { return ""; }
};

#endif
