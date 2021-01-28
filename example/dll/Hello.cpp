/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
