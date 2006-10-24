// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_TWOWAYSTREAM_
#define _YARP2_TWOWAYSTREAM_

#include <yarp/Address.h>
#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>

namespace yarp {
    class TwoWayStream;
}

/**
 * An object containing an InputStream + OutputStream pair.
 */
class yarp::TwoWayStream {
public:
    virtual ~TwoWayStream() {
    }

    virtual InputStream& getInputStream() = 0; // throws
    virtual OutputStream& getOutputStream() = 0; // throws

    virtual const Address& getLocalAddress() = 0; // throws
    virtual const Address& getRemoteAddress() = 0; // throws

    virtual bool isOk() = 0;

    virtual void reset() = 0;

    virtual void close() = 0; // throws

    // These should be called at the beginning and end of logical packets.
    // Streams are encouraged to handle errors and atomicity at the level of 
    // logical packets
    virtual void beginPacket() = 0;
    virtual void endPacket() = 0;
};

#endif

