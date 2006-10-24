// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STRINGOUTPUTSTREAM_
#define _YARP2_STRINGOUTPUTSTREAM_

#include <yarp/OutputStream.h>
#include <yarp/String.h>

namespace yarp {
    class StringOutputStream;
}

/**
 * An OutputStream that produces a String.  For testing purposes.
 */
class yarp::StringOutputStream : public OutputStream {
public:
    using OutputStream::write;

    StringOutputStream() { }


    String toString() {
        return data;
    }


    virtual void write(const Bytes& b) { // throws
        //ACE_DEBUG((LM_DEBUG,"StringOutputStream::write"));
        for (int i=0; i<b.length(); i++) {
            data += b.get()[i];
        }
    }

    virtual void close() {
    }

private:
    String data;
};

#endif

