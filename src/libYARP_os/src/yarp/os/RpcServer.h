/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
class YARP_os_API RpcServer : public AbstractContactable
{
public:
    /**
     * Constructor.
     */
    RpcServer();

    /**
     * Destructor.
     */
    ~RpcServer() override;

    /**
     * Write cannot be called by RPCServer
     */
    virtual bool write(const PortWriter& writer,
                       const PortWriter* callback = nullptr) const override;

    /**
     * Write cannot be called by RPCServer
     */
    virtual bool write(const PortWriter& writer,
                       PortReader& reader,
                       const PortWriter* callback = nullptr) const override;

    // documented in UnbufferedContactable
    bool read(PortReader& reader, bool willReply = true) override;

    void setInputMode(bool expectInput) override;
    void setOutputMode(bool expectOutput) override;
    void setRpcMode(bool expectRpc) override;

    Port& asPort() override
    {
        return port;
    }

    const Port& asPort() const override
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
