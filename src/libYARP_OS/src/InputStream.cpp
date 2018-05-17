/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/InputStream.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os;

// slow implementation - only relevant for textmode operation

std::string InputStream::readLine(int terminal, bool *success) {
    std::string buf("");
    bool done = false;
    int esc = 0;
    if (success != nullptr) {
        *success = true;
    }
    while (!done) {
        int v = read();
        if (v<0) {
            if (success != nullptr) {
                *success = false;
            }
            return "";
        }
        char ch = (char)v;
        if (v=='\\') {
            esc++;
        }
        if (v!=0&&v!='\r'&&v!='\n') {
            if (v!='\\'||esc>=2) {
                while (esc) {
                    buf += '\\';
                    esc--;
                }
            }
            if (v!='\\') {
                buf += ch;
            }
        }
        if (ch==terminal) {
            if (!esc) {
                done = true;
            } else {
                esc = 0;
            }
        }
    }
    return buf;
}

YARP_SSIZE_T InputStream::readFull(const Bytes& b) {
    YARP_SSIZE_T off = 0;
    YARP_SSIZE_T fullLen = b.length();
    YARP_SSIZE_T remLen = fullLen;
    YARP_SSIZE_T result = 1;
    while (result>0&&remLen>0) {
        result = read(b, off, remLen);
        if (result>0) {
            remLen -= result;
            off += result;
        }
    }
    return (result<=0)?-1:fullLen;
}

YARP_SSIZE_T InputStream::readDiscard(size_t len) {
    if (len<100) {
        char buf[100];
        Bytes b(buf, len);
        return readFull(b);
    }
    ManagedBytes b(len);
    return readFull(b.bytes());
}
