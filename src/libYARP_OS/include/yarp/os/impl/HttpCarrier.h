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
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/impl/StringOutputStream.h>

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
    HttpTwoWayStream(TwoWayStream *delegate,
                     const char *txt,
                     const char *prefix,
                     yarp::os::Property& prop);

    virtual ~HttpTwoWayStream();

    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const Address& getLocalAddress();
    virtual const Address& getRemoteAddress();

    virtual bool isOk();
    virtual void reset();
    virtual void write(const Bytes& b);
    virtual void apply(char ch);
    virtual void close();
    virtual void beginPacket();
    virtual void endPacket();

    void flip();
    void finish();
    bool useJson();
    String *typeHint();
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
    HttpCarrier();

    virtual Carrier *create();

    virtual String getName();

    bool checkHeader(const Bytes& header, const char *prefix);

    virtual bool checkHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual bool requireAck();
    virtual bool isTextMode();
    virtual bool supportReply();
    virtual bool sendHeader(Protocol& proto);
    virtual bool expectSenderSpecifier(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);
    virtual bool sendIndex(Protocol& proto);
    virtual bool expectIndex(Protocol& proto);
    virtual bool sendAck(Protocol& proto);
    virtual bool expectAck(Protocol& proto);
    virtual bool respondToHeader(Protocol& proto);

    virtual bool reply(Protocol& proto, SizedWriter& writer);
    virtual bool write(Protocol& proto, SizedWriter& writer);
};

#endif
