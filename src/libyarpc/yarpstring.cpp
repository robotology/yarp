/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "yarp.h"
#include "yarpimpl.h"

#include <string>

YARP_DEFINE(yarpStringPtr) yarpStringCreate() {
    yarpStringPtr str = new yarpString;
    if (str!=NULL) {
        str->implementation = new std::string();
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
            delete (std::string*)(str->implementation);
            str->implementation = NULL;
        }
        delete str;
    }
}


YARP_DEFINE(const char *) yarpStringToC(yarpStringPtr str) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            return ((std::string*)(str->implementation))->c_str();
        }
    }
    return NULL;
}


YARP_DEFINE(int) yarpStringFromC(yarpStringPtr str, const char *c) {
    if (str!=NULL) {
        if (str->implementation!=NULL) {
            (*((std::string*)(str->implementation))) = c;
            return 0;
        }
    }
    return -1;
}
