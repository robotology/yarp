/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_RPCCLIENT_H
#define YARP_OS_RPCCLIENT_H

#include <yarp/os/AbstractContactable.h>

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * A port that is specialized as an RPC client.  That is, it expects to
 * connect to a single server, and receive replies on the same connection.
 *
 */
class YARP_OS_API RpcClient : public AbstractContactable
{
public:
    /**
     * Constructor.
     */
    RpcClient();

    /**
     * Destructor.
     */
    virtual ~RpcClient();

    RpcClient(const RpcClient& alt) = delete;
    const RpcClient& operator=(const RpcClient& alt) = delete;

    // documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = false) override;

    // documented in UnbufferedContactable
    virtual bool reply(PortWriter& writer) override;

    // documented in UnbufferedContactable
    virtual bool replyAndDrop(PortWriter& writer) override;


    void setInputMode(bool expectInput) override;
    void setOutputMode(bool expectOutput) override;
    void setRpcMode(bool expectRpc) override;

    virtual Port& asPort() override
    {
        return port;
    }

    virtual const Port& asPort() const override
    {
        return port;
    }

private:
    // an RpcClient may be implemented with a regular port
    Port port;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RPCCLIENT_H
