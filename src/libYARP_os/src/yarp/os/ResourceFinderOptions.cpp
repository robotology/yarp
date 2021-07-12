/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
