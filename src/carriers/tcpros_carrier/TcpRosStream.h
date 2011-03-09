// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef TCPROSSTREAM_INC
#define TCPROSSTREAM_INC

#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/ManagedBytes.h>
#include "BlobNetworkHeader.h"

namespace yarp {
    namespace os {
        namespace impl {
            class TcpRosStream;
        }
    }
}


class yarp::os::impl::TcpRosStream : public TwoWayStream, 
                                     public InputStream,
                                     public OutputStream
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
public:
    TcpRosStream(TwoWayStream *delegate, bool sender,
                 bool service, int raw) : sender(sender), raw(raw) {
        this->delegate = delegate;
        firstRound = true;
        phase = 0;
        expectTwiddle = service&&sender;
    }

    virtual ~TcpRosStream() {
        if (delegate!=NULL) {
            delete delegate;
            delegate = NULL;
        }
    }

    virtual InputStream& getInputStream() { return *this; }
    virtual OutputStream& getOutputStream() { return *this; }


    virtual const Address& getLocalAddress() {
        return delegate->getLocalAddress();
    }

    virtual const Address& getRemoteAddress() {
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

    virtual int read(const Bytes& b);

    virtual void interrupt() { 
        delegate->getInputStream().interrupt();
    }

};

#endif
