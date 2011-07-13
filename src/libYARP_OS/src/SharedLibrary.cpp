// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <ace/ACE.h>
#include <ace/DLL.h>

#include <yarp/os/SharedLibrary.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

#define HELPER(x) (*((ACE_DLL *)implementation))

bool SharedLibrary::open(const char *filename) {
    close();
    implementation = new ACE_DLL();
    YARP_ASSERT(implementation);
    int result = HELPER(implementation).open(filename);
    if (result!=0) {
        close();
        return false;
    }
    return true;
}

bool SharedLibrary::close() {
    if (implementation!=NULL) {
        HELPER(implementation).close();
        implementation = NULL;
    }
    return true;
}

void *SharedLibrary::getSymbol(const char *symbolName) {
    if (implementation==NULL) return NULL;
    return HELPER(implementation).symbol(symbolName);
}

#endif

