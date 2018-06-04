/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_XMLRPC_CARRIER_XMLRPCSTREAM_H
#define YARP_XMLRPC_CARRIER_XMLRPCSTREAM_H

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>

#include "XmlRpcClient.h"
#include "XmlRpcServerConnection.h"

namespace yarp {
    namespace os {
        class XmlRpcStream;
    }
}

class yarp::os::XmlRpcStream : public TwoWayStream,
                               public InputStream,
                               public OutputStream
{
private:
    TwoWayStream *delegate;
    YarpXmlRpc::XmlRpcClient client;
    YarpXmlRpc::XmlRpcServerConnection server;
    StringInputStream sis;
    StringOutputStream sos;
    bool sender;
    bool firstRound;
    bool interpretRos;
public:
    XmlRpcStream(TwoWayStream *delegate, bool sender, bool interpretRos) :
            client("notset", 0),
            server(0, nullptr),
            sender(sender),
            interpretRos(interpretRos)
    {
        this->delegate = delegate;
        client.reset();
        server.reset();
        firstRound = true;
    }

    virtual ~XmlRpcStream()
    {
        if (delegate != nullptr) {
            delete delegate;
            delegate = nullptr;
        }
    }

    virtual yarp::os::InputStream& getInputStream() override
    {
        return *this;
    }

    virtual yarp::os::OutputStream& getOutputStream() override
    {
        return *this;
    }


    virtual const yarp::os::Contact& getLocalAddress() override
    {
        return delegate->getLocalAddress();
    }

    virtual const yarp::os::Contact& getRemoteAddress() override
    {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() const override
    {
        return delegate->isOk();
    }

    virtual void reset() override
    {
        delegate->reset();
    }

    virtual void close() override
    {
        delegate->close();
    }

    virtual void beginPacket() override
    {
        delegate->beginPacket();
    }

    virtual void endPacket() override
    {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(const Bytes& b) override;

    virtual void interrupt() override
    {
        delegate->getInputStream().interrupt();
    }

};

#endif // YARP_XMLRPC_CARRIER_XMLRPCSTREAM_H
