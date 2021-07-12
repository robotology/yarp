/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_UDPCARRIER_H
#define YARP_OS_IMPL_UDPCARRIER_H

#include <yarp/os/AbstractCarrier.h>
#include <yarp/os/impl/DgramTwoWayStream.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Communicating between two ports via UDP.
 */
class UdpCarrier :
        public AbstractCarrier
{
public:
    UdpCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    virtual int getSpecifierCode() const;

    bool checkHeader(const Bytes& header) override;
    void getHeader(Bytes& header) const override;
    void setParameters(const Bytes& header) override;
    bool requireAck() const override;
    bool isConnectionless() const override;
    bool respondToHeader(ConnectionState& proto) override;
    bool expectReplyToHeader(ConnectionState& proto) override;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_UDPCARRIER_H
