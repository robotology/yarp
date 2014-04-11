// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef XMLRPCSTREAM_INC
#define XMLRPCSTREAM_INC

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
        client("notset",0),
        server(0,0/*NULL*/),
        sender(sender),
        interpretRos(interpretRos) {
        this->delegate = delegate;
        client.reset();
        server.reset();
        firstRound = true;
    }

    virtual ~XmlRpcStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual yarp::os::InputStream& getInputStream() { return *this; }
    virtual yarp::os::OutputStream& getOutputStream() { return *this; }


    virtual const yarp::os::Contact& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const yarp::os::Contact& getRemoteAddress() {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() {
        return delegate->isOk();
    }

    virtual void reset() {
        delegate->reset();
    }

    virtual void close() {
        delegate->close();
    }

    virtual void beginPacket() {
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    virtual void write(const Bytes& b);

    virtual YARP_SSIZE_T read(const Bytes& b);

    virtual void interrupt() {
        delegate->getInputStream().interrupt();
    }

};

#endif
