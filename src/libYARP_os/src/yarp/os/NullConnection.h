/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_NULLCONNECTION_H
#define YARP_OS_NULLCONNECTION_H

#include <yarp/os/api.h>

#include <yarp/os/Connection.h>

namespace yarp {
namespace os {

class YARP_os_API NullConnection : public Connection
{
public:
    bool isValid() const override;
    bool isTextMode() const override;
    bool canEscape() const override;
    void handleEnvelope(const std::string& envelope) override;
    bool requireAck() const override;
    bool supportReply() const override;
    bool isLocal() const override;
    bool isPush() const override;
    bool isConnectionless() const override;
    bool isBroadcast() const override;
    bool isActive() const override;
    bool modifiesIncomingData() const override;
    yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;
    bool acceptIncomingData(yarp::os::ConnectionReader& reader) override;
    bool modifiesOutgoingData() const override;
    const PortWriter& modifyOutgoingData(const PortWriter& writer) override;
    bool acceptOutgoingData(const PortWriter& writer) override;
    bool modifiesReply() const override;
    PortReader& modifyReply(PortReader& reader) override;
    void setCarrierParams(const yarp::os::Property& params) override;
    void getCarrierParams(yarp::os::Property& params) const override;
    void getHeader(yarp::os::Bytes& header) const override;
    void prepareDisconnect() override;
    std::string getName() const override;
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_NULLCONNECTION_H
