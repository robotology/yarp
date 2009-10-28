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


String Name::getCarrierModifier(const char *mod, bool *hasModifier) {
    bool ok = false;
    String work = txt;
    unsigned int mid = YARP_STRSTR(work,":/");
    if (mid!=String::npos && mid>0) {
        work = work.substr(0,mid);
        String target = String("+")+mod+".";
        unsigned int modLoc = YARP_STRSTR(work,target.c_str());
        if (modLoc!=String::npos) {
            work = work.substr(modLoc+target.length(),work.length());
            unsigned int endLoc = YARP_STRSTR(work,"+");
            if (endLoc!=String::npos) {
                work = work.substr(0,endLoc);
            }
            ok = true;
        }
    }
    if (hasModifier!=NULL) {
        *hasModifier = ok;
    }
    return ok?work:"";
}


