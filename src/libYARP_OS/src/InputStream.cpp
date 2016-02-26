/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/InputStream.h>
#include <yarp/os/ManagedBytes.h>

using namespace yarp::os;

// slow implementation - only relevant for textmode operation

ConstString InputStream::readLine(int terminal, bool *success) {
    ConstString buf("");
    bool done = false;
    int esc = 0;
    if (success!=NULL) *success = true;
    while (!done) {
        int v = read();
        if (v<0) {
            if (success!=NULL) *success = false;
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
        if (v<0) { 
            if (success!=NULL) *success = false;
            return "";
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
        result = read(b,off,remLen);
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
        Bytes b(buf,len);
        return readFull(b);
    }
    ManagedBytes b(len);
    return readFull(b.bytes());
}

