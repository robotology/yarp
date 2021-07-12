/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/YarpPlugin.h>
#include "Handler.h"

#include <stdio.h>

using namespace yarp::os;

int main() {
    YarpPluginSettings settings;
    settings.setLibraryMethodName("hoopy","hoopy");
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
