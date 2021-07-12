/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
class YARP_os_API RpcClient : public AbstractContactable
{
public:
    /**
     * Constructor.
     */
    RpcClient();

    /**
     * Destructor.
     */
    ~RpcClient() override;

    // Non-copyable
    RpcClient(const RpcClient& alt) = delete;
    const RpcClient& operator=(const RpcClient& alt) = delete;

    // documented in UnbufferedContactable
    bool read(PortReader& reader, bool willReply = false) override;

    // documented in UnbufferedContactable
    bool reply(PortWriter& writer) override;

    // documented in UnbufferedContactable
    bool replyAndDrop(PortWriter& writer) override;

    void setInputMode(bool expectInput) override;
    void setOutputMode(bool expectOutput) override;
    void setRpcMode(bool expectRpc) override;

    Port& asPort() override;
    const Port& asPort() const override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RPCCLIENT_H
