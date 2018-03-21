/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Name.h>

using namespace yarp::os;

Name::Name(const ConstString& txt) {
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
    if (mid!=ConstString::npos && mid>0) {
        ConstString first = txt.substr(0, mid);
        ConstString second = txt.substr(mid+2);
        if (first.length()>=2) {
            if (first[0]=='/') {
                first = first.substr(1);
            }
        }
        return Contact(second, first, "", -1);
    }
    return Contact(txt, "", "", -1);
}


ConstString Name::getCarrierModifier(const char *mod, bool *hasModifier) {
    bool ok = false;
    ConstString work = txt;
    size_t mid = work.find(":/");
    if (mid!=ConstString::npos && mid>0) {
        work = work.substr(0, mid);
        ConstString target = ConstString("+")+mod+".";
        size_t modLoc = work.find(target);
        if (modLoc!=ConstString::npos) {
            work = work.substr(modLoc+target.length(), work.length());
            size_t endLoc = work.find('+');
            if (endLoc!=ConstString::npos) {
                work = work.substr(0, endLoc);
            }
            ok = true;
        }
    }
    if (hasModifier!=nullptr) {
        *hasModifier = ok;
    }
    return ok?work:"";
}
