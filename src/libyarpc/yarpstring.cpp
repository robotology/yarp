/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "yarp.h"
#include "yarpimpl.h"

#include <yarp/os/ConstString.h>

YARP_DEFINE(yarpStringPtr) yarpStringCreate() {
    yarpStringPtr str = new yarpString;
    if (str!=NULL) {
        str->implementation = new yarp::os::ConstString();
        if (str->implementation==NULL) {
            delete str;
            str = NULL;
        }
    }
    return str;
}


YARP_DEFINE(void) yarpStringFree(yarpStringPtr str) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            delete (yarp::os::ConstString*)(str->implementation);
            str->implementation = NULL;
        }
        delete str;
    }
}


YARP_DEFINE(const char *) yarpStringToC(yarpStringPtr str) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            return ((yarp::os::ConstString*)(str->implementation))->c_str();
        }
    }
    return NULL;
}


YARP_DEFINE(int) yarpStringFromC(yarpStringPtr str, const char *c) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            (*((yarp::os::ConstString*)(str->implementation))) = c;
            return 0;
        }
    }
    return -1;
}


