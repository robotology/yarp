// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2012 iCub Facility
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
