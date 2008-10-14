// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_STRINGINPUTSTREAM_
#define _YARP2_STRINGINPUTSTREAM_

#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/IOException.h>

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
class yarp::os::impl::StringInputStream : public InputStream {
public:
    using InputStream::read;

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
        for (int i=0; i<b.length(); i++) {
            data += b.get()[i];
        }
    }

    virtual int read(const Bytes& b) {
        char *base = b.get();
        int space = b.length();
        int ct = 0;
        for (int i=0; i<space; i++) {
            if (at<data.length()) {
                base[i] = data[at];
                at++;
                ct++;
            }
        }
        /*
          else {
          throw IOException("end of stream");
          break;
          }
        */
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
