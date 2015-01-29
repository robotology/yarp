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
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/ConstString.h>
#include "BlobNetworkHeader.h"
#include "WireTwiddler.h"

#include <string>
#include <map>

#include <tcpros_carrier_api.h>

namespace yarp {
    namespace os {
        class TcpRosStream;
    }
}


class YARP_tcpros_carrier_API yarp::os::TcpRosStream : public TwoWayStream, 
                                                       public InputStream,
                                                       public OutputStream
{
private:
    TwoWayStream *delegate;
    int raw;
    BlobNetworkHeader header;
    ManagedBytes scan;
    char *cursor;
    int remaining;
    int phase;
    bool expectTwiddle;
    WireTwiddler twiddler;
    yarp::os::ConstString kind;
    WireTwiddlerReader twiddlerReader;
    bool initiative;
    bool setInitiative;
public:
    TcpRosStream(TwoWayStream *delegate,
                 bool sender,
                 bool reply,
                 bool service,
                 int raw,
                 const char *kind) :
            delegate(delegate),
            raw(raw),
            phase(0),
            expectTwiddle(service && sender),
            kind(kind),
            twiddlerReader(delegate->getInputStream(), twiddler),
            initiative(false),
            setInitiative(false)
    {
        updateKind(kind,sender,reply);
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

    virtual YARP_SSIZE_T read(const Bytes& b);

    virtual void interrupt() {
        delegate->getInputStream().interrupt();
    }

    void updateKind(const char *kind, bool sender, bool reply);

    static std::map<std::string, std::string> rosToKind();
    static std::string rosToKind(const char *rosname);
    static bool configureTwiddler(WireTwiddler& twiddler, const char *txt, const char *prompt, bool sender, bool reply);
};

#endif
