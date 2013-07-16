// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STRINGINPUTSTREAM_
#define _YARP2_STRINGINPUTSTREAM_

#include <yarp/os/InputStream.h>
#include <yarp/os/impl/String.h>

namespace yarp {
    namespace os {
        namespace impl {
            class StringInputStream;
        }
    }
}

/**
 * An InputStream that reads from String.  For testing purposes.
 */
class yarp::os::impl::StringInputStream : public yarp::os::InputStream {
public:
    using yarp::os::InputStream::read;

    StringInputStream() {
        at = 0;
        data = "";
    }

    void reset() {
        at = 0;
        data = "";
    }

    void reset(const String& str) {
        at = 0;
        data = str;
    }

    void add(const char *txt) {
        data += txt;
    }

    void add(const String& txt) {
        data += txt;
    }

    void add(const Bytes& b) {
        for (size_t i=0; i<b.length(); i++) {
            data += b.get()[i];
        }
    }

    virtual ssize_t read(const Bytes& b) {
        char *base = b.get();
        size_t space = b.length();
        ssize_t ct = 0;
        for (size_t i=0; i<space; i++) {
            if (at<data.length()) {
                base[i] = data[at];
                at++;
                ct++;
            }
        }
        return ct;
    }

    virtual void close() {
    }

    virtual String toString() {
        return data;
    }

    virtual bool isOk() {
        return true;
    }

private:
    String data;
    unsigned int at;
};

#endif
