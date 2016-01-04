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
#include <yarp/os/NetType.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>

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
    ConstString proc;
    ConstString part;
    bool data;
    bool filterData;
    bool chunked;
    TwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    ConstString format;
    ConstString outer;
    bool isWriter;
public:
    HttpTwoWayStream(TwoWayStream *delegate,
                     const char *txt,
                     const char *prefix,
                     yarp::os::Property& prop,
                     bool writer);

    virtual ~HttpTwoWayStream();

    virtual InputStream& getInputStream();
    virtual OutputStream& getOutputStream();
    virtual const Contact& getLocalAddress();
    virtual const Contact& getRemoteAddress();

    virtual bool isOk();
    virtual void reset();
    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b);
    virtual void apply(char ch);
    virtual void close();
    virtual void beginPacket();
    virtual void endPacket();

    void flip();
    void finish();
    bool useJson();
    ConstString *typeHint();
};



/**
 * Communicating via http.
 */
class yarp::os::impl::HttpCarrier : public TcpCarrier {
private:
    ConstString url, input, prefix;
    bool urlDone;
    bool expectPost;
    int contentLength;
    yarp::os::Property prop;
    HttpTwoWayStream *stream;
public:
    HttpCarrier();

    virtual Carrier *create();

    virtual ConstString getName();

    bool checkHeader(const Bytes& header, const char *prefix);

    virtual bool checkHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual bool requireAck();
    virtual bool isTextMode();
    virtual bool supportReply();
    virtual bool sendHeader(ConnectionState& proto);
    virtual bool expectSenderSpecifier(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);
    virtual bool sendIndex(ConnectionState& proto,SizedWriter& writer);
    virtual bool expectIndex(ConnectionState& proto);
    virtual bool sendAck(ConnectionState& proto);
    virtual bool expectAck(ConnectionState& proto);
    virtual bool respondToHeader(ConnectionState& proto);

    virtual bool reply(ConnectionState& proto, SizedWriter& writer);
    virtual bool write(ConnectionState& proto, SizedWriter& writer);
};

#endif
