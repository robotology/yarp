// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef TCPROSSTREAM_INC
#define TCPROSSTREAM_INC

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/ConstString.h>
#include "BlobNetworkHeader.h"
#include "WireTwiddler.h"

#include <string>
#include <map>

#include <tcpros_carrier_api.h>

namespace yarp {
    namespace os {
        namespace impl {
            class TcpRosStream;
        }
    }
}


class YARP_tcpros_carrier_API yarp::os::impl::TcpRosStream : public TwoWayStream, 
                                                             public yarp::os::InputStream,
                                                             public yarp::os::OutputStream
{
private:
    TwoWayStream *delegate;
    bool sender;
    int raw;
    bool firstRound;
    BlobNetworkHeader header;
    ManagedBytes scan;
    char *cursor;
    int remaining;
    int phase;
    bool expectTwiddle;
    WireTwiddler twiddler;
    yarp::os::ConstString kind;
    WireTwiddlerReader twiddlerReader;
public:
    TcpRosStream(TwoWayStream *delegate,
                 bool sender,
                 bool service,
                 int raw,
                 const char *kind) :
            delegate(delegate),
            sender(sender),
            raw(raw),
            firstRound(true),
            phase(0),
            expectTwiddle(service && sender),
            kind(kind),
            twiddlerReader(delegate->getInputStream(), twiddler)
    {
        updateKind(kind);
    }

    virtual ~TcpRosStream() {
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
        twiddlerReader.reset();
        delegate->beginPacket();
    }

    virtual void endPacket() {
        delegate->endPacket();
    }

    virtual void write(const Bytes& b);

    virtual ssize_t read(const Bytes& b);

    virtual void interrupt() {
        delegate->getInputStream().interrupt();
    }

    void updateKind(const char *kind);

    static std::map<std::string, std::string> rosToKind();
    static std::string rosToKind(const char *rosname);
};

#endif
