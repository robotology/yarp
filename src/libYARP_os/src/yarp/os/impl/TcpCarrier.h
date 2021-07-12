/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_TCPCARRIER_H
#define YARP_OS_IMPL_TCPCARRIER_H

#include <yarp/os/AbstractCarrier.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Communicating between two ports via TCP.
 */
class YARP_os_impl_API TcpCarrier :
        public AbstractCarrier
{
public:
    TcpCarrier(bool requireAckFlag = true);

    Carrier* create() const override;

    std::string getName() const override;

    virtual int getSpecifierCode() const;

    bool checkHeader(const yarp::os::Bytes& header) override;
    void getHeader(yarp::os::Bytes& header) const override;
    void setParameters(const yarp::os::Bytes& header) override;
    bool requireAck() const override;
    bool isConnectionless() const override;
    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

private:
    bool requireAckFlag;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TCPCARRIER_H
