/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_RPCSERVER_H
#define YARP_OS_RPCSERVER_H

#include <yarp/os/AbstractContactable.h>

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * A port that is specialized as an RPC server.  That is, it expects to
 * receive connections from multiple clients, and to reply to them via
 * those connections.
 *
 */
class YARP_OS_API RpcServer : public AbstractContactable
{
public:
    /**
     * Constructor.
     */
    RpcServer();

    /**
     * Destructor.
     */
    virtual ~RpcServer();

    // documented in UnbufferedContactable
    virtual bool write(const PortWriter& writer,
                       const PortWriter* callback = nullptr) const override;

    // documented in UnbufferedContactable
    virtual bool write(const PortWriter& writer,
                       PortReader& reader,
                       const PortWriter* callback = nullptr) const override;

    // documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = true) override;

    virtual void setInputMode(bool expectInput) override;
    virtual void setOutputMode(bool expectOutput) override;
    virtual void setRpcMode(bool expectRpc) override;

    virtual Port& asPort() override
    {
        return port;
    }

    virtual const Port& asPort() const override
    {
        return port;
    }

private:
    // an RpcServer may be implemented with a regular port
    Port port;

    // forbid copy constructor and assignment operator by making them private
    // and not implementing them
    RpcServer(const RpcServer& alt);
    const RpcServer& operator=(const RpcServer& alt);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RPCSERVER_H
