/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/SharedLibraryClass.h>
#include "Handler.h"

#include <stdio.h>

using namespace yarp::os;

class Hoopy : public Handler {
public:
    virtual int hello() {
        printf("Hello!\n");
        return 42;
    }
};

YARP_DEFINE_SHARED_SUBCLASS(hoopy,Hoopy,Handler)
