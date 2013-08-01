// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>
#include <yarp/os/Vocab.h>

using namespace yarp::os;
using namespace yarp::os::impl;

NetInt32 Vocab::encode(const ConstString& s) {
    char a = '\0';
    char b = '\0';
    char c = '\0';
    char d = '\0';
    if (s.length()>=1) {
        a = s[0];
        if (s.length()>=2) {
            b = s[1];
            if (s.length()>=3) {
                c = s[2];
                if (s.length()>=4) {
                    d = s[3];
                }
            }
        }
    }
    return VOCAB(a,b,c,d);
}


ConstString Vocab::decode(NetInt32 code) {
    ConstString s;
    for (int i=0; i<4; i++) {
        int ch = code%256;
        if (ch>0) {
            s += ((char)ch);
        }
        code /= 256;
    }
    return s;
}


