/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H
#define YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H

#include <yarp/robotinterface/experimental/Robot.h>

#include <yarp/os/Searchable.h>

#include <string>

namespace yarp {
namespace robotinterface {

namespace experimental {
class XMLReaderResult;
} // namespace experimental

namespace impl {

class XMLReaderFileVx
{
public:
    bool verbose;
    virtual ~XMLReaderFileVx(){};
    virtual yarp::robotinterface::experimental::XMLReaderResult getRobotFromFile(const std::string& filename,
                                                                                 const yarp::os::Searchable& config,
                                                                                 bool verbose = false) = 0;
    virtual yarp::robotinterface::experimental::XMLReaderResult getRobotFromString(const std::string& xmlString,
                                                                                   const yarp::os::Searchable& config,
                                                                                   bool verbose = false) = 0;
};

} // namespace impl
} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADERFILEVX_H
