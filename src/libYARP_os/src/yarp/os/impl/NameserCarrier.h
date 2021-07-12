/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_NAMESERCARRIER_H
#define YARP_OS_IMPL_NAMESERCARRIER_H

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/TcpCarrier.h>


namespace yarp {
namespace os {
namespace impl {

/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class NameserTwoWayStream :
        public TwoWayStream,
        InputStream
{
private:
    TwoWayStream* delegate;
    std::string pendingRead;
    std::string swallowRead;

public:
    NameserTwoWayStream(TwoWayStream* delegate);

    virtual ~NameserTwoWayStream();

    InputStream& getInputStream() override;
    OutputStream& getOutputStream() override;
    const Contact& getLocalAddress() const override;
    const Contact& getRemoteAddress() const override;

    bool isOk() const override;
    void reset() override;
    void close() override;
    void beginPacket() override;
    void endPacket() override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;
};


/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class NameserCarrier :
        public TcpCarrier
{
private:
    bool firstSend;

public:
    NameserCarrier();

    std::string getName() const override;
    std::string getSpecifierName() const;

    Carrier* create() const override;

    bool checkHeader(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    bool requireAck() const override;
    bool isTextMode() const override;
    bool supportReply() const override;
    bool canEscape() const override;
    bool sendHeader(ConnectionState& proto) override;
    bool expectSenderSpecifier(ConnectionState& proto) override;
    bool expectIndex(ConnectionState& proto) override;
    bool sendAck(ConnectionState& proto) override;
    bool expectAck(ConnectionState& proto) override;
    bool respondToHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
    bool write(ConnectionState& proto, SizedWriter& writer) override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_NAMESERCARRIER_H
