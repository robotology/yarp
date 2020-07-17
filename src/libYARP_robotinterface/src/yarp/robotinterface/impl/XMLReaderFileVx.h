/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H
#define YARP_ROBOTINTERFACE_IMPL_XMLREADERFILEVX_H

#include <yarp/robotinterface/Robot.h>

#include <string>

namespace yarp {
namespace robotinterface {
class XMLReaderResult;

namespace impl {

class XMLReaderFileVx
{
public:
    bool verbose;
    virtual ~XMLReaderFileVx(){};
    virtual XMLReaderResult getRobotFromFile(const std::string& filename, bool verbose = false) = 0;
    virtual XMLReaderResult getRobotFromString(const std::string& xmlString, bool verbose = false) = 0;
};

} // namespace impl
} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_XMLREADERFILEVX_H
