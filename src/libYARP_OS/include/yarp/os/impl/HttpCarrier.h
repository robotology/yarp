/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_HTTPCARRIER
#define YARP_OS_IMPL_HTTPCARRIER

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
 *   ", " becomes newline
 *   "+" becomes " "
 * This text is then passed on to the port, just as for a normal
 * text connection.
 * For example
 *   "GET /d, list"
 * is the same as telnetting into a port and typing:
 *   CONNECT anon
 *   d
 *   list
 * or typing "list" to "yarp rpc /portname".
 * The response of the port, if any, is output in ugly html.
 *
 * For "POST /" ... not done yet.
 */
class yarp::os::impl::HttpTwoWayStream : public TwoWayStream, public OutputStream
{
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

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;
    virtual const Contact& getLocalAddress() override;
    virtual const Contact& getRemoteAddress() override;

    virtual bool isOk() override;
    virtual void reset() override;
    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;
    virtual void apply(char ch);
    virtual void close() override;
    virtual void beginPacket() override;
    virtual void endPacket() override;

    void flip();
    void finish();
    bool useJson();
    ConstString *typeHint();
};



/**
 * Communicating via http.
 */
class yarp::os::impl::HttpCarrier : public TcpCarrier
{
private:
    ConstString url, input, prefix;
    bool urlDone;
    bool expectPost;
    int contentLength;
    yarp::os::Property prop;
    HttpTwoWayStream *stream;
public:
    HttpCarrier();

    virtual Carrier *create() override;

    virtual ConstString getName() override;

    bool checkHeader(const Bytes& header, const char *prefix);

    virtual bool checkHeader(const Bytes& header) override;
    virtual void setParameters(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isTextMode() override;
    virtual bool supportReply() override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectSenderSpecifier(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) override;
    virtual bool expectIndex(ConnectionState& proto) override;
    virtual bool sendAck(ConnectionState& proto) override;
    virtual bool expectAck(ConnectionState& proto) override;
    virtual bool respondToHeader(ConnectionState& proto) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;
    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;
};

#endif // YARP_OS_IMPL_HTTPCARRIER
