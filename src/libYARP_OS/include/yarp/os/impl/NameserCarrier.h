/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_NAMESERCARRIER_H
#define YARP_OS_IMPL_NAMESERCARRIER_H

#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/TcpCarrier.h>


namespace yarp {
    namespace os {
        namespace impl {
            class NameserCarrier;
            class NameserTwoWayStream;
        }
    }
}

/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class yarp::os::impl::NameserTwoWayStream : public TwoWayStream, InputStream
{
private:
    TwoWayStream *delegate;
    std::string pendingRead;
    std::string swallowRead;
public:
    NameserTwoWayStream(TwoWayStream *delegate);

    virtual ~NameserTwoWayStream();

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;
    virtual const Contact& getLocalAddress() const override;
    virtual const Contact& getRemoteAddress() const override;

    virtual bool isOk() const override;
    virtual void reset() override;
    virtual void close() override;
    virtual void beginPacket() override;
    virtual void endPacket() override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const yarp::os::Bytes& b) override;
};


/**
 * Communicating between two ports via a variant plain-text protocol
 * originally designed for the yarp name server.  For backwards
 * compatibility
 */
class yarp::os::impl::NameserCarrier : public TcpCarrier
{
private:
    bool firstSend;
public:
    NameserCarrier();

    virtual std::string getName() const override;
    std::string getSpecifierName() const;

    virtual Carrier *create() override;

    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual bool requireAck() const override;
    virtual bool isTextMode() const override;
    virtual bool supportReply() const override;
    virtual bool canEscape() const override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectSenderSpecifier(ConnectionState& proto) override;
    virtual bool expectIndex(ConnectionState& proto) override;
    virtual bool sendAck(ConnectionState& proto) override;
    virtual bool expectAck(ConnectionState& proto) override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;
};

#endif // YARP_OS_IMPL_NAMESERCARRIER_H
