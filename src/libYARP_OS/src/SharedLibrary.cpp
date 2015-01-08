// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>

#ifdef YARP_HAS_ACE
#  include <ace/ACE.h>
#  include <ace/DLL.h>
#else
#  include <dlfcn.h>
#endif

#include <yarp/os/SharedLibrary.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

#define HELPER(x) (*((ACE_DLL *)implementation))

SharedLibrary::SharedLibrary() :
    implementation(NULL) {
}

SharedLibrary::SharedLibrary(const char *filename) :
    implementation(NULL) {
    open(filename);
}

SharedLibrary::~SharedLibrary() {
    close();
}

bool SharedLibrary::open(const char *filename) {
    close();
#ifdef YARP_HAS_ACE
    implementation = new ACE_DLL();
    yAssert(implementation);
    int result = HELPER(implementation).open(filename);
    if (result!=0) {
        close();
        return false;
    }
    return true;
#else
    implementation = dlopen(filename, RTLD_LAZY);
    return implementation!=NULL;
#endif
}

bool SharedLibrary::close() {
    if (implementation!=NULL) {
#ifdef YARP_HAS_ACE
        HELPER(implementation).close();
#else
        dlclose(implementation);
#endif
        implementation = NULL;
    }
    return true;
}

void *SharedLibrary::getSymbol(const char *symbolName) {
    if (implementation==NULL) return NULL;
#ifdef YARP_HAS_ACE
    return HELPER(implementation).symbol(symbolName);
#else
    return dlsym(implementation,symbolName);
#endif
}

bool SharedLibrary::isValid() const {
    return implementation != NULL;
}
