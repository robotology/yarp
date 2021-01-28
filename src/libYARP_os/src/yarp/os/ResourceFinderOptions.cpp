/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ResourceFinderOptions.h>

using namespace yarp::os;

ResourceFinderOptions::ResourceFinderOptions(SearchLocations searchLocations,
                                             DuplicateFilesPolicy duplicateFilesPolicy,
                                             SearchFlavor searchFlavor,
                                             const std::string& resourceType,
                                             MessageFilter messageFilter)
{
    this->searchLocations = searchLocations;
    this->duplicateFilesPolicy = duplicateFilesPolicy;
    this->searchFlavor = searchFlavor;
    this->resourceType = resourceType;
    this->messageFilter = messageFilter;
}
