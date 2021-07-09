/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
