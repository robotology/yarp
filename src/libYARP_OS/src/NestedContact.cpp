// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/NestedContact.h>

using namespace yarp::os;

NestedContact::NestedContact(const ConstString& nFullName) {
    fromString(nFullName);
}

bool NestedContact::fromString(const ConstString& nFullName) {
    fullName = nFullName;
    nodeName = nFullName;
    nestedName = "";
    category = "";
    ConstString::size_type idx = fullName.find("@");
    if (idx!=ConstString::npos) {
        // Great!  Looks like we are using a new syntax suggested 
        // by Lorenzo Natale, /topic@/node
        nestedName = fullName.substr(0,idx);
        nodeName = fullName.substr(idx+1,fullName.length());
        char ch = nestedName[nestedName.length()-1];
        if (ch=='-'||ch=='+'||ch=='1') {
            size_t offset = 1;
            bool ok = true;
            if (ch=='1') {
                ok = false;
                if (nestedName.length()>=2) {
                    char ch0 = nestedName[nestedName.length()-2];
                    if (ch0=='-'||ch0=='+') {
                        offset++;
                        category += ch0;
                        ok = true;
                    }
                }
            }
            if (ok) {
                category += ch;
                nestedName = nestedName.substr(0,nestedName.length()-offset);
            }
        }
        return true;
    } 
    idx = fullName.find("=");
    if (idx!=ConstString::npos) {
        nodeName = fullName.substr(0,idx);
        nestedName = fullName.substr(idx+1,fullName.length());
        idx = nestedName.find("/");
        if (idx==0) return true;
        category = nestedName.substr(0,idx);
        nestedName = nestedName.substr(idx,nestedName.length());
        return true;
    }
    idx = fullName.find("#");
    if (idx!=ConstString::npos) {
        nodeName = fullName.substr(0,idx);
        nestedName = fullName.substr(idx+1,fullName.length());
        char ch = nodeName[nodeName.length()-1];
        if (ch=='-'||ch=='+') {
            category += ch;
            nodeName = nodeName.substr(0,nodeName.length()-1);
        }
        return true;
    }
    return false;
}
