/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_RPCSERVER_H
#define YARP_OS_RPCSERVER_H

#include <yarp/os/AbstractContactable.h>

namespace yarp {
    namespace os {
        class RpcServer;
    }
}

/**
 * \ingroup comm_class
 *
 * A port that is specialized as an RPC server.  That is, it expects to
 * receive connections from multiple clients, and to reply to them via
 * those connections.
 *
 */
class YARP_OS_API yarp::os::RpcServer : public AbstractContactable {
public:
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using AbstractContactable::open;
#endif // YARP_NO_DEPRECATED

    /**
     * Constructor.
     */
    RpcServer();

    /**
     * Destructor.
     */
    virtual ~RpcServer();

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer,
                       PortWriter *callback = nullptr) const override;

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer, PortReader& reader,
                       PortWriter *callback = nullptr) const override;

    // documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = true) override;

    virtual void setInputMode(bool expectInput) override;
    virtual void setOutputMode(bool expectOutput) override;
    virtual void setRpcMode(bool expectRpc) override;

    virtual Port& asPort() override {
        return port;
    }

    virtual const Port& asPort() const override {
        return port;
    }

private:
    // an RpcServer may be implemented with a regular port
    Port port;

    // forbid copy constructor and assignment operator by making them private
    // and not implementing them
    RpcServer(const RpcServer& alt);
    const RpcServer& operator = (const RpcServer& alt);
};

#endif // YARP_OS_RPCSERVER_H
