// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Name.h>

using namespace yarp::os::impl;
using namespace yarp::os;

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


Contact Name::toAddress() const {
    size_t mid = txt.find(":/");
    if (mid!=String::npos && mid>0) {
        String first = txt.substr(0,mid);
        String second = txt.substr(mid+2);
        if (first.length()>=2) {
            if (first[0]=='/') {
                first = first.substr(1);
            }
        }
        return Contact::bySocket(first,"",-1).addName(second);
    }
    return Contact::bySocket("","",-1).addName(txt);
}


String Name::getCarrierModifier(const char *mod, bool *hasModifier) {
    bool ok = false;
    String work = txt;
    size_t mid = work.find(":/");
    if (mid!=String::npos && mid>0) {
        work = work.substr(0,mid);
        String target = String("+")+mod+".";
        size_t modLoc = work.find(target);
        if (modLoc!=String::npos) {
            work = work.substr(modLoc+target.length(),work.length());
            size_t endLoc = work.find("+");
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


