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

    virtual Carrier *create();

    virtual ConstString getName();

    virtual int getSpecifierCode();

    virtual bool checkHeader(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual bool requireAck();
    virtual bool isConnectionless();
    virtual bool respondToHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);
};

#endif // YARP_OS_IMPL_UDPCARRIER_H
