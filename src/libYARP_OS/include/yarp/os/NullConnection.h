/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NULLCONNECTION_H
#define YARP_OS_NULLCONNECTION_H

#include <yarp/os/api.h>

#include <yarp/os/Connection.h>

namespace yarp {
namespace os {

class YARP_OS_API NullConnection : public Connection
{
public:
    virtual bool isValid() const override;
    virtual bool isTextMode() const override;
    virtual bool canEscape() const override;
    virtual void handleEnvelope(const std::string& envelope) override;
    virtual bool requireAck() const override;
    virtual bool supportReply() const override;
    virtual bool isLocal() const override;
    virtual bool isPush() const override;
    virtual bool isConnectionless() const override;
    virtual bool isBroadcast() const override;
    virtual bool isActive() const override;
    virtual bool modifiesIncomingData() const override;
    virtual yarp::os::ConnectionReader& modifyIncomingData(yarp::os::ConnectionReader& reader) override;
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader) override;
    virtual bool modifiesOutgoingData() const override;
    virtual const PortWriter& modifyOutgoingData(const PortWriter& writer) override;
    virtual bool acceptOutgoingData(const PortWriter& writer) override;
    virtual bool modifiesReply() const override;
    virtual PortReader& modifyReply(PortReader& reader) override;
    virtual void setCarrierParams(const yarp::os::Property& params) override;
    virtual void getCarrierParams(yarp::os::Property& params) const override;
    virtual void getHeader(yarp::os::Bytes& header) const override;
    virtual void prepareDisconnect() override;
    virtual std::string getName() const override;
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_NULLCONNECTION_H
