/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_TEXTCARRIER_H
#define YARP_OS_IMPL_TEXTCARRIER_H

#include <yarp/os/impl/TcpCarrier.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Communicating between two ports via a plain-text protocol.
 */
class TextCarrier : public TcpCarrier
{

public:
    TextCarrier(bool ackVariant = false);

    Carrier* create() const override;

    std::string getName() const override;

    virtual std::string getSpecifierName() const;

    bool checkHeader(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    bool requireAck() const override;
    bool isTextMode() const override;
    bool supportReply() const override;
    bool sendHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
    bool expectSenderSpecifier(ConnectionState& proto) override;
    bool sendIndex(ConnectionState& proto, SizedWriter& writer) override;
    bool expectIndex(ConnectionState& proto) override;
    bool sendAck(ConnectionState& proto) override;
    bool expectAck(ConnectionState& proto) override;
    bool respondToHeader(ConnectionState& proto) override;

private:
    bool ackVariant;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TEXTCARRIER_H
