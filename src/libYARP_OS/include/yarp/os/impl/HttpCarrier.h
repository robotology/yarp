// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_HTTPCARRIER_
#define _YARP2_HTTPCARRIER_

#include <yarp/os/impl/TcpCarrier.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/Property.h>

namespace yarp {
    namespace os {
        namespace impl {
            class HttpCarrier;
            class HttpTwoWayStream;
        }
    }
}

/**
 * Minimal http connection support.
 * Most useful for connections to YARP ports from browsers.
 * No support for connections in the opposite direction.
 * This carrier is recruited when a port sees a TCP stream that
 * begins with "GET /".
 *
 * For "GET /", the URL following the "/" is
 * currently translated to text as follows:
 *   "," becomes newline
 *   "+" becomes " "
 * This text is then passed on to the port, just as for a normal
 * text connection.
 * For example
 *   "GET /d,list"
 * is the same as telnetting into a port and typing:
 *   CONNECT anon
 *   d
 *   list
 * or typing "list" to "yarp rpc /portname".
 * The response of the port, if any, is output in ugly html.
 *
 * For "POST /" ... not done yet.
 */
class yarp::os::impl::HttpTwoWayStream : public TwoWayStream, public OutputStream {
private:
    String proc;
    String part;
    bool data;
    bool filterData;
    bool chunked;
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    String format;
    String outer;
public:
    HttpTwoWayStream(TwoWayStream *delegate, const char *txt,
                     const char *prefix,
                     yarp::os::Property& prop);

    virtual ~HttpTwoWayStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return sis; }
    virtual OutputStream& getOutputStream() { return *this; }


    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void write(const Bytes& b) { // throws
        if (chunked) {
            delegate->getOutputStream().write(b);
        } else {
            for (size_t i=0; i<b.length(); i++) {
                apply(b.get()[i]);
            }
        }
    }

    virtual void apply(char ch);

    virtual void close() {
        apply('\n');
        apply('\n');
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    void flip() {
        sis.add("r\n");
    }

    void finish() {
        sis.add("q\n");
    }

    bool useJson() {
        return format=="json";
    }

    String *typeHint() {
        return &outer;
    }
};



/**
 * Communicating via http.
 */
class yarp::os::impl::HttpCarrier : public TcpCarrier {
private:
    String url, input, prefix;
    bool urlDone;
    bool expectPost;
    int contentLength;
    yarp::os::Property prop;
    HttpTwoWayStream *stream;
public:
    HttpCarrier() {
        url = "";
        input = "";
        urlDone = false;
        expectPost = false;
        contentLength = 0;
        stream = 0 /*NULL*/;
    }

    virtual String getName() {
        return "http";
    }

    bool checkHeader(const Bytes& header, const char *prefix) {
        if (header.length()==8) {
            String target = prefix;
            for (unsigned int i=0; i<target.length(); i++) {
                if (!(target[i]==header.get()[i])) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    virtual bool checkHeader(const Bytes& header) {
        bool ok = checkHeader(header,"GET /");
        if (!ok) {
            // http carrier accepts POST /form but not general posts
            // (leave that to xmlrpc carrier)
            ok = checkHeader(header,"POST /fo");
        } else {
            // make sure it isn't a MJPEG stream get
            ok = !checkHeader(header,"GET /?ac");
        }
        return ok;
    }

    virtual void setParameters(const Bytes& header) {
        if (header.length()==8) {
            bool adding = false;
            for (unsigned int j=0; j<8; j++) {
                char ch = header.get()[j];
                if (adding) {
                    if (ch!=' ') {
                        url += ch;
                    } else {
                        urlDone = true;
                        break;
                    }
                }
                if (ch=='/') {
                    adding = true;
                }
            }
        }
    }

    virtual void getHeader(const Bytes& header) {
        if (header.length()==8) {
            String target = "GET / HT";
            for (int i=0; i<8; i++) {
                header.get()[i] = target[i];
            }
        }
    }

    virtual Carrier *create() {
        return new HttpCarrier();
    }

    virtual bool requireAck() {
        return false;
    }

    virtual bool isTextMode() {
        return true;
    }


    virtual bool supportReply() {
        return false;
    }

    virtual bool sendHeader(Protocol& proto) {
        printf("not yet meant to work\n");
        String target = "GET / HTTP/1.1";
        Bytes b((char*)target.c_str(),8);
        proto.os().write(b);
        return true;

    }

    bool expectSenderSpecifier(Protocol& proto);

    bool expectReplyToHeader(Protocol& proto) {
        // expect and ignore CONTENT lines
        String result = NetType::readLine(proto.is());
        return true;
    }


    bool sendIndex(Protocol& proto) {
        // no index
        return true;
    }

    bool expectIndex(Protocol& proto) {
        // no index
        return true;
    }

    bool sendAck(Protocol& proto) {
        // no acknowledgement
        return true;
    }

    virtual bool expectAck(Protocol& proto) {
        // no acknowledgement
        return true;
    }

    bool respondToHeader(Protocol& proto) {
        stream = new HttpTwoWayStream(proto.giveStreams(),
                                      input.c_str(),
                                      prefix.c_str(),
                                      prop);
        proto.takeStreams(stream);
        return true;
    }

    virtual bool reply(Protocol& proto, SizedWriter& writer);

    virtual bool write(Protocol& proto, SizedWriter& writer);
};

#endif
