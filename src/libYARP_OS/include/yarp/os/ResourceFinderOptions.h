// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick, Daniele Domenichelli
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RESOURCEFINDEROPTIONS_
#define _YARP2_RESOURCEFINDEROPTIONS_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class ResourceFinderOptions;
    }
}

class YARP_OS_API yarp::os::ResourceFinderOptions {
public:
   
    enum SearchLocations {
        NoLocation  = 0x0000,
        Directory   = 0x0001, // Search current directory
        Context     = 0x0002, // Search current context directory
        Robot       = 0x0004, // Search current robot directory
        User        = 0x0008, // Search user directory
        Sysadmin    = 0x0010, // Search system config directories
        Installed   = 0x0020, // Search system data directories + path.d
        Default = User | Sysadmin | Installed,
        ModuleDefault = Default | Robot | Context | Directory
    };

    enum DuplicateFilesPolicy {
        First,      // Keep only the first file found
        All         // Keep all the files
    };

    SearchLocations searchLocations;
    DuplicateFilesPolicy duplicateFilesPolicy;

    ResourceFinderOptions(SearchLocations searchLocations = ModuleDefault,
                          DuplicateFilesPolicy duplicateFilesPolicy = First);
};

#endif
