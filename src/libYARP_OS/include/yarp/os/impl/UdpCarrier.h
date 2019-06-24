/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
