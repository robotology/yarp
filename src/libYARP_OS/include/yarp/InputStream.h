// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_INPUTSTREAM_
#define _YARP2_INPUTSTREAM_

#include <yarp/Bytes.h>

namespace yarp {
    class InputStream;
}

/**
 * Simple specification of the minimum functions needed from input streams.
 * The streams could be TCP, UDP, MCAST, ...
 */
class yarp::InputStream {
public:
    InputStream() {
    }
  
    virtual ~InputStream() { }

    virtual void check() {}

    virtual int read() { // throws
        unsigned char result;
        int ct = read(Bytes((char*)&result,1));
        if (ct<1) {
            return -1;
        }
        return result;
    }

    virtual int read(const Bytes& b, int offset, int len) { // throws
        return read(Bytes(b.get()+offset,len));
    }

    virtual int read(const Bytes& b) = 0;

    virtual void close() = 0;

    virtual void interrupt() {
    }

    // These should be called at the beginning and end of logical packets.
    // Streams are encouraged to handle errors and atomicity at the level of 
    // logical packets
    //virtual void beginPacket() = 0;
    //virtual void endPacket() = 0;

};

#endif
