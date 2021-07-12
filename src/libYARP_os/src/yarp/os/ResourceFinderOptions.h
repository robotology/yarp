/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RESOURCEFINDEROPTIONS_H
#define YARP_OS_RESOURCEFINDEROPTIONS_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

/**
 * These options are loosely based on
 *   http://wiki.icub.org/wiki/YARP_ResourceFinder
 *
 * For a User search location:
 *   YARP_CONFIG_HOME is only checked if the searchFlavor is ConfigLike
 *   YARP_DATA_HOME is only checked if the searchFlavor is DataLike
 */
class YARP_os_API ResourceFinderOptions
{
public:
    enum SearchLocations
    {
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

    enum DuplicateFilesPolicy
    {
        First, // Keep only the first file found
        All    // Keep all the files
    };

    enum SearchFlavor
    {
        ConfigLike  = 0x0001,
        DataLike    = 0x0002,
        ConfigAndDataLike = ConfigLike | DataLike
    };

    enum MessageFilter
    {
        ShowNone = 0x0000,
        ShowDirectories = 0x0001,
        ShowErrors = 0x0002,
        ShowFromFlags = 0x0004,
        ShowAll = ShowDirectories | ShowErrors
    };

    SearchLocations searchLocations;
    DuplicateFilesPolicy duplicateFilesPolicy;
    SearchFlavor searchFlavor;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) resourceType;
    MessageFilter messageFilter;

    ResourceFinderOptions(SearchLocations searchLocations = ModuleDefault,
                          DuplicateFilesPolicy duplicateFilesPolicy = First,
                          SearchFlavor searchFlavor = ConfigAndDataLike,
                          const std::string& resourceType = "",
                          MessageFilter messageFilter = ShowFromFlags);

    static ResourceFinderOptions findFirstMatch()
    {
        return ResourceFinderOptions();
    }

    static ResourceFinderOptions findAllMatch()
    {
        return ResourceFinderOptions(ModuleDefault, All);
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RESOURCEFINDER_H
