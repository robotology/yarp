/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

class XmlRpcStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
private:
    TwoWayStream *delegate;
    YarpXmlRpc::XmlRpcClient client;
    YarpXmlRpc::XmlRpcServerConnection server;
    yarp::os::StringInputStream sis;
    yarp::os::StringOutputStream sos;
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

    yarp::os::InputStream& getInputStream() override
    {
        return *this;
    }

    yarp::os::OutputStream& getOutputStream() override
    {
        return *this;
    }


    const yarp::os::Contact& getLocalAddress() const override
    {
        return delegate->getLocalAddress();
    }

    const yarp::os::Contact& getRemoteAddress() const override
    {
        return delegate->getRemoteAddress();
    }

    bool isOk() const override
    {
        return delegate->isOk();
    }

    void reset() override
    {
        delegate->reset();
    }

    void close() override
    {
        delegate->close();
    }

    void beginPacket() override
    {
        delegate->beginPacket();
    }

    void endPacket() override
    {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    void interrupt() override
    {
        delegate->getInputStream().interrupt();
    }

};

#endif // YARP_XMLRPC_CARRIER_XMLRPCSTREAM_H
