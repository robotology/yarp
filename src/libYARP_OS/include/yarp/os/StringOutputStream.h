/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_STRINGOUTPUTSTREAM_H
#define YARP_OS_STRINGOUTPUTSTREAM_H

#include <yarp/os/OutputStream.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class StringOutputStream;
    }
}

/**
 * An OutputStream that produces a string.  Handy for testing purposes.
 */
class yarp::os::StringOutputStream : public OutputStream {
public:
    using OutputStream::write;

    StringOutputStream() { }

    ConstString toString() {
        return data;
    }

    void reset() {
        data = "";
    }

    virtual void write(const Bytes& b) {
        ConstString tmp((char*)b.get(),b.length());
        data += tmp;
    }

    virtual void close() {
    }

    virtual bool isOk() { 
        return true;
    }

    const ConstString& str() { return data; }

private:
    ConstString data;
};

#endif // YARP_OS_STRINGOUTPUTSTREAM_H
