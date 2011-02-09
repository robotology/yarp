// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Hello.h"

#include <yarp/os/SharedLibraryClass.h>

// just for testing, we make two distinct copies of factory function
YARP_DEFINE_SHARED_CLASS(hello_there,HelloImpl);
YARP_DEFINE_DEFAULT_SHARED_CLASS(HelloImpl);

void HelloImpl::write(int v) {
    x = v+1;
}

int HelloImpl::read() {
    return x;
}

