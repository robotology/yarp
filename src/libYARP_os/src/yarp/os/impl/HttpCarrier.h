/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_HTTPCARRIER
#define YARP_OS_IMPL_HTTPCARRIER

#include <yarp/os/NetType.h>
#include <yarp/os/Property.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/TcpCarrier.h>

namespace yarp {
namespace os {
namespace impl {

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
class HttpTwoWayStream :
        public TwoWayStream,
        public OutputStream
{
private:
    std::string proc;
    std::string part;
    bool data;
    bool filterData;
    bool chunked;
    TwoWayStream* delegate;
    StringInputStream sis;
    StringOutputStream sos;
    std::string format;
    std::string outer;
    bool isWriter;

public:
    HttpTwoWayStream(TwoWayStream* delegate,
                     const char* txt,
                     const char* prefix,
                     yarp::os::Property& prop,
                     bool writer);

    virtual ~HttpTwoWayStream();

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;
    const Contact& getLocalAddress() const override;
    const Contact& getRemoteAddress() const override;

    bool isOk() const override;
    void reset() override;
    using yarp::os::OutputStream::write;
    void write(const Bytes& b) override;
    virtual void apply(char ch);
    void close() override;
    void beginPacket() override;
    void endPacket() override;

    void flip();
    void finish();
    bool useJson();
    std::string* typeHint();
};


/**
 * Communicating via http.
 */
class HttpCarrier :
        public TcpCarrier
{
private:
    std::string url, input, prefix;
    bool urlDone;
    bool expectPost;
    int contentLength;
    yarp::os::Property prop;
    HttpTwoWayStream* stream;

public:
    HttpCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    bool checkHeader(const Bytes& header, const char* prefix);

    bool checkHeader(const Bytes& header) override;
    void setParameters(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    bool requireAck() const override;
    bool isTextMode() const override;
    bool supportReply() const override;
    bool sendHeader(ConnectionState& proto) override;
    bool expectSenderSpecifier(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
    bool sendIndex(ConnectionState& proto, SizedWriter& writer) override;
    bool expectIndex(ConnectionState& proto) override;
    bool sendAck(ConnectionState& proto) override;
    bool expectAck(ConnectionState& proto) override;
    bool respondToHeader(ConnectionState& proto) override;

    bool reply(ConnectionState& proto, SizedWriter& writer) override;
    bool write(ConnectionState& proto, SizedWriter& writer) override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_HTTPCARRIER
