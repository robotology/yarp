/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_TCPCARRIER_H
#define YARP_OS_IMPL_TCPCARRIER_H

#include <yarp/os/AbstractCarrier.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TcpCarrier;
        }
    }
}

/**
 * Communicating between two ports via TCP.
 */
class yarp::os::impl::TcpCarrier : public AbstractCarrier
{
public:

    TcpCarrier(bool requireAckFlag = true);

    virtual Carrier *create() override;

    virtual std::string getName() const override;

    virtual int getSpecifierCode();

    virtual bool checkHeader(const yarp::os::Bytes& header) override;
    virtual void getHeader(const yarp::os::Bytes& header) override;
    virtual void setParameters(const yarp::os::Bytes& header) override;
    virtual bool requireAck() override;
    virtual bool isConnectionless() const override;
    virtual bool respondToHeader(yarp::os::ConnectionState& proto) override;
    virtual bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

private:
    bool requireAckFlag;
};

#endif // YARP_OS_IMPL_TCPCARRIER_H
