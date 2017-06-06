/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_TEXTCARRIER_H
#define YARP_OS_IMPL_TEXTCARRIER_H

#include <yarp/os/impl/TcpCarrier.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TextCarrier;
        }
    }
}

/**
 * Communicating between two ports via a plain-text protocol.
 */
class yarp::os::impl::TextCarrier : public TcpCarrier
{

public:
    TextCarrier(bool ackVariant = false);

    virtual Carrier *create() override;

    virtual ConstString getName() override;

    virtual ConstString getSpecifierName();

    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isTextMode() override;
    virtual bool supportReply() override;
    virtual bool sendHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
    virtual bool expectSenderSpecifier(ConnectionState& proto) override;
    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer) override;
    virtual bool expectIndex(ConnectionState& proto) override;
    virtual bool sendAck(ConnectionState& proto) override;
    virtual bool expectAck(ConnectionState& proto) override;
    virtual bool respondToHeader(ConnectionState& proto) override;
private:
    bool ackVariant;
};

#endif // YARP_OS_IMPL_TEXTCARRIER_H
