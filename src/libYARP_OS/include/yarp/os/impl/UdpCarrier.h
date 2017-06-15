/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

    virtual ConstString getName() override;

    virtual int getSpecifierCode();

    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual void setParameters(const Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isConnectionless() override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;
};

#endif // YARP_OS_IMPL_UDPCARRIER_H
