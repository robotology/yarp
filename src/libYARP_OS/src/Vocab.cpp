/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Vocab.h>

using namespace yarp::os;

NetInt32 Vocab::encode(const ConstString& str) {
    char a = '\0';
    char b = '\0';
    char c = '\0';
    char d = '\0';
    if (str.length()>=1) {
        a = str[0];
        if (str.length()>=2) {
            b = str[1];
            if (str.length()>=3) {
                c = str[2];
                if (str.length()>=4) {
                    d = str[3];
                }
            }
        }
    }
    return VOCAB(a,b,c,d);
}


ConstString Vocab::decode(NetInt32 code) {
    ConstString str;
    for (int i=0; i<4; i++) {
        int ch = code%256;
        if (ch>0) {
            str += ((char)ch);
        }
        code /= 256;
    }
    return str;
}


