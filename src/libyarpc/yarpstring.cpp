// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "yarp.h"
#include "yarpimpl.h"

#include <yarp/os/impl/String.h>

YARP_DEFINE(yarpStringPtr) yarpStringCreate() {
    yarpStringPtr str = new yarpString;
    if (str!=NULL) {
        str->implementation = new yarp::os::impl::String();
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
            delete (yarp::os::impl::String*)(str->implementation);
            str->implementation = NULL;
        }
        delete str;
    }
}


YARP_DEFINE(const char *) yarpStringToC(yarpStringPtr str) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            return ((yarp::os::impl::String*)(str->implementation))->c_str();
        }
    }
    return NULL;
}


YARP_DEFINE(int) yarpStringFromC(yarpStringPtr str, const char *c) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            (*((yarp::os::impl::String*)(str->implementation))) = c;
            return 0;
        }
    }
    return -1;
}


