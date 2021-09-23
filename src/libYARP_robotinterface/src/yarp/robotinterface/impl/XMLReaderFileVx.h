/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H
#define YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H

#include <yarp/robotinterface/Robot.h>

#include <yarp/os/Searchable.h>

#include <string>

namespace yarp::robotinterface {
class XMLReaderResult;
} // namespace yarp::robotinterface

namespace yarp::robotinterface::impl {

class XMLReaderFileVx
{
public:
    bool verbose;
    virtual ~XMLReaderFileVx(){};
    virtual yarp::robotinterface::XMLReaderResult getRobotFromFile(const std::string& filename,
                                                                                 const yarp::os::Searchable& config,
                                                                                 bool verbose = false) = 0;
    virtual yarp::robotinterface::XMLReaderResult getRobotFromString(const std::string& xmlString,
                                                                                   const yarp::os::Searchable& config,
                                                                                   bool verbose = false) = 0;
};

} // namespace yarp::robotinterface::impl

#endif // YARP_ROBOTINTERFACE_XMLREADERFILEVX_H
