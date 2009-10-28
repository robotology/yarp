// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/Name.h>

using namespace yarp::os::impl;

Name::Name(const String& txt) {
    this->txt = txt;
}

bool Name::isRooted() const {
    if (txt.length()>=1) {
        if (txt[0] == '/') {
            return true;
        }
    }
    return false;
}


Address Name::toAddress() const {
    unsigned int mid = YARP_STRSTR(txt,":/");
    if (mid!=String::npos && mid>0) {
        String first = txt.substr(0,mid);
        String second = txt.substr(mid+2);
        if (first.length()>=2) {
            if (first[0]=='/') {
                first = first.substr(1);
            }
        }
        return Address("",-1,first,second);
    } else {
        return Address("",-1,"",txt);
    }
}


