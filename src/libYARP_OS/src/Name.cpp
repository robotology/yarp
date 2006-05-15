// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/Name.h>

using namespace yarp;

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
    int mid = txt.strstr(":/");
    if (mid!=-1 && mid>0) {
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


