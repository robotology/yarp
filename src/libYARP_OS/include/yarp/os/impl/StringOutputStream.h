// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STRINGOUTPUTSTREAM_
#define _YARP2_STRINGOUTPUTSTREAM_

#include <yarp/os/impl/OutputStream.h>
#include <yarp/os/impl/String.h>

namespace yarp {
    namespace os {
        namespace impl {
            class StringOutputStream;
        }
    }
}

/**
 * An OutputStream that produces a String.  For testing purposes.
 */
class yarp::os::impl::StringOutputStream : public OutputStream {
public:
    using OutputStream::write;

    StringOutputStream() { }


    String toString() {
        return data;
    }


    virtual void write(const Bytes& b) { // throws
        String tmp;
        YARP_STRSET(tmp,(char*)b.get(),b.length(),0);
        data += tmp;
        //for (int i=0; i<b.length(); i++) {
        //data += b.get()[i];
        //}
    }

    virtual void close() {
    }

    virtual bool isOk() { 
        return true;
    }

private:
    String data;
};

#endif

