/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
            class UdpCarrier;
        }
    }
}

/**
 * Communicating between two ports via UDP.
 */
class yarp::os::impl::UdpCarrier : public AbstractCarrier
{
public:
    UdpCarrier();

    virtual Carrier *create() override;

    virtual std::string getName() const override;

    virtual int getSpecifierCode();

    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual void setParameters(const Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isConnectionless() const override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
};

#endif // YARP_OS_IMPL_UDPCARRIER_H
