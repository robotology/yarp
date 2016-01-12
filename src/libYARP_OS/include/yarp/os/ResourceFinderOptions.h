// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick, Daniele Domenichelli
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_RESOURCEFINDEROPTIONS
#define YARP2_RESOURCEFINDEROPTIONS

#include <yarp/os/api.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class ResourceFinderOptions;
    }
}

/**
 *
 * These options are loosely based on 
 *   http://wiki.icub.org/wiki/YARP_ResourceFinder
 *
 * For a User search location:
 *   YARP_CONFIG_HOME is only checked if the searchFlavor is ConfigLike
 *   YARP_DATA_HOME is only checked if the searchFlavor is DataLike
 *
 */
class YARP_OS_API yarp::os::ResourceFinderOptions {
public:
   
    enum SearchLocations {
        NoLocation     = 0x0000,
        Directory      = 0x0001, // Search current directory
        Context        = 0x0002, // Search current context directory
        Robot          = 0x0004, // Search current robot directory
        User           = 0x0008, // Search user directory
        Sysadmin       = 0x0010, // Search system config directories
        Installed      = 0x0020, // Search system data directories + path.d
        NearMainConfig = 0x0040, // Search directory of main config file
                                 // (if one is used, e.g. with --from)
        ClassicContext = 0x1000, // Search old-style context directories
        Default = User | Sysadmin | Installed,
        ModuleDefault = Default | Robot | Context | Directory | ClassicContext | NearMainConfig
    };

    enum DuplicateFilesPolicy {
        First,      // Keep only the first file found
        All         // Keep all the files
    };

    enum SearchFlavor {
        ConfigLike  = 0x0001,
        DataLike    = 0x0002,
        ConfigAndDataLike = ConfigLike | DataLike
    };

    enum MessageFilter {
        ShowNone = 0x0000,
        ShowDirectories = 0x0001,
        ShowErrors = 0x0002,
        ShowFromFlags = 0x0004,
        ShowAll = ShowDirectories | ShowErrors
    };

    SearchLocations searchLocations;
    DuplicateFilesPolicy duplicateFilesPolicy;
    SearchFlavor searchFlavor;
    ConstString resourceType;
    MessageFilter messageFilter;

    ResourceFinderOptions(SearchLocations searchLocations = ModuleDefault,
                          DuplicateFilesPolicy duplicateFilesPolicy = First,
                          SearchFlavor searchFlavor = ConfigAndDataLike,
                          const ConstString& resourceType = "",
                          MessageFilter messageFilter = ShowFromFlags);

    static ResourceFinderOptions findFirstMatch() {
        return ResourceFinderOptions();
    }

    static ResourceFinderOptions findAllMatch() {
        return ResourceFinderOptions(ModuleDefault,All);
    }
};

#endif
