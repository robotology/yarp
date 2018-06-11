/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef TCPROSSTREAM_INC
#define TCPROSSTREAM_INC

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include <string>
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
    std::string kind;
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
            header(BlobNetworkHeader{0,0,0}),
            cursor(nullptr),
            remaining(0),
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

    virtual yarp::os::InputStream& getInputStream() override { return *this; }
    virtual yarp::os::OutputStream& getOutputStream() override { return *this; }


    virtual const yarp::os::Contact& getLocalAddress() const override {
        return delegate->getLocalAddress();
    }

    virtual const yarp::os::Contact& getRemoteAddress() const override {
        return delegate->getRemoteAddress();
    }

    virtual bool isOk() const override {
        return delegate->isOk();
    }

    virtual void reset() override {
        delegate->reset();
    }

    virtual void close() override {
        delegate->close();
    }

    virtual void beginPacket() override {
        twiddlerReader.reset();
        delegate->beginPacket();
    }

    virtual void endPacket() override {
        delegate->endPacket();
    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual yarp::conf::ssize_t read(Bytes& b) override;

    virtual void interrupt() override {
        delegate->getInputStream().interrupt();
    }

    void updateKind(const char *kind, bool sender, bool reply);

    static std::map<std::string, std::string> rosToKind();
    static std::string rosToKind(const char *rosname);
    static bool configureTwiddler(WireTwiddler& twiddler, const char *txt, const char *prompt, bool sender, bool reply);
};

#endif
