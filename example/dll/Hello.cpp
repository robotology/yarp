/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Hello.h"

#include <yarp/os/SharedLibraryClass.h>

// just for testing, we make two distinct copies of factory function
YARP_DEFINE_DEFAULT_SHARED_CLASS(HelloImpl);
YARP_DEFINE_SHARED_SUBCLASS(hello_there,HelloImpl,Hello);

void HelloImpl::write(int v) {
    x = v+1;
}

int HelloImpl::read() {
    return x;
}

