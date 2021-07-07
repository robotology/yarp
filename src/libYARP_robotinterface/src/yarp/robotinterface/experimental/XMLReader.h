/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_EXPERIMENTAL_XMLREADER_H
#define YARP_ROBOTINTERFACE_EXPERIMENTAL_XMLREADER_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_ROBOTINTERFACE_EXPERIMENTAL_XMLREADER_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/experimental/XMLReader.h> file is deprecated in favour of <yarp/dev/XMLReader.h>")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/robotinterface/XMLReader.h>

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_ROBOTINTERFACE_EXPERIMENTAL_ROBOT_H_ON_PURPOSE
#include <yarp/robotinterface/experimental/Robot.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_ROBOTINTERFACE_EXPERIMENTAL_ROBOT_H_ON_PURPOSE

namespace yarp {
namespace robotinterface {
namespace experimental {

YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::XMLReader") yarp::robotinterface::XMLReader XMLReader;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::XMLReaderResult") yarp::robotinterface::XMLReaderResult XMLReaderResult;

} // namespace experimental
} // namespace robotinterface
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_ROBOTINTERFACE_EXPERIMENTAL_XMLREADER_H
