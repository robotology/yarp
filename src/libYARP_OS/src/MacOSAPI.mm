/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Francesco Romano <francesco.romano@iit.it>
 *          Claudio Fantacci <claudio.fantacci@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#import "yarp/os/impl/MacOSAPI.h"

#import <Foundation/Foundation.h>

void* disableAppNap()
{
    NSProcessInfo *processInfo = [NSProcessInfo processInfo];
    id activity = nil;
    // beginActivityWithOptions:reason: is only in macOS 10.9+
    if ([processInfo respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
        activity = [processInfo beginActivityWithOptions:NSActivityLatencyCritical | NSActivityUserInitiated
                                                 reason:@"YARP requires AppNap off"];
    }
    return activity;
}

void restoreAppNap(void *activityInfo)
{
    id activity = (id)activityInfo;
    if (!activity) return;
    // There is no need to check for macOS versions here,
    // as we are passing an object which is obtained only
    // on the correct version of macOS
    NSProcessInfo *processInfo = [NSProcessInfo processInfo];
    [processInfo endActivity:activity];
}
