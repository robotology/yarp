// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2012 iCub Facility
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/YarpPlugin.h>
#include "Handler.h"

#include <stdio.h>

using namespace yarp::os;

int main() {
    YarpPluginSettings settings;
    settings.dll_name = "hoopy";
    settings.fn_name = "hoopy";
    YarpPlugin<Handler> handler;
    if (!handler.open(settings)) {
        settings.reportFailure();
        printf("Maybe set LD_LIBRARY_PATH or equivalent.\n");
        return 1;
    }
    printf("Found handler!\n");
    Handler *hoopy = handler.create();
    if (!hoopy) {
        printf("Failed to create handler\n");
        return 1;
    }
    int result = hoopy->hello();
    printf("Result is %d\n", result);
    handler.destroy(hoopy);
    return 0;
}
