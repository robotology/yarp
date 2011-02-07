// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Hello.h"
#include "ShareHello.h"

YARP_DEFINE_SHARED_CLASS(hello_there,HelloImpl);

void HelloImpl::write(int v) {
    x = v+1;
}

int HelloImpl::read() {
    return x;
}

