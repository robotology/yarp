// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STRINGOUTPUTSTREAM_
#define _YARP2_STRINGOUTPUTSTREAM_

#include <yarp/os/OutputStream.h>
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
class yarp::os::impl::StringOutputStream : public yarp::os::OutputStream {
public:
    using yarp::os::OutputStream::write;

    StringOutputStream() { }


    String toString() {
        return data;
    }

    void reset() {
        data = "";
    }


    virtual void write(const Bytes& b) { // throws
        String tmp((char*)b.get(),b.length());
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

    const String& str() { return data; }

private:
    String data;
};

#endif

