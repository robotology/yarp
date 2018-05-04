/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    ConstString pendingRead;
    ConstString swallowRead;
public:
    NameserTwoWayStream(TwoWayStream *delegate);

    virtual ~NameserTwoWayStream();

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;
    virtual const Contact& getLocalAddress() override;
    virtual const Contact& getRemoteAddress() override;

    virtual bool isOk() override;
    virtual void reset() override;
    virtual void close() override;
    virtual void beginPacket() override;
    virtual void endPacket() override;

    virtual bool setReadTimeout(double timeout) override;

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const yarp::os::Bytes& b) override;
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

    virtual ConstString getName() override;
    virtual ConstString getSpecifierName();

    virtual Carrier *create() override;

    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isTextMode() override;
    virtual bool supportReply() override;
    virtual bool canEscape() override;
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
