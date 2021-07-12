/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
