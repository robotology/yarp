/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_ROBOTINTERFACEDTD_H
#define YARP_ROBOTINTERFACE_IMPL_ROBOTINTERFACEDTD_H

#include <yarp/robotinterface/XMLReader.h>
#include <yarp/robotinterface/Action.h>
#include <yarp/robotinterface/Device.h>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/Robot.h>
#include <yarp/robotinterface/Types.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <yarp/robotinterface/api.h>

class TiXmlUnknown;

namespace yarp {
namespace robotinterface {

// Represent something like this in the xml file
// <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 1.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV1.0.dtd">
class RobotInterfaceDTD
{
public:
    enum DocType {
        DocTypeUnknown = 0,
        DocTypeRobot,
        DocTypeDevices,
        DocTypeParams,
        DocTypeActions,
    };

    RobotInterfaceDTD() :
        type(DocTypeUnknown),
        identifier(""),
        uri(""),
        majorVersion(0),
        minorVersion(0) {}

    bool parse(TiXmlUnknown* unknownNode, const std::string& curr_filename);

    bool valid();

    void setDefault();

    DocType type;
    std::string identifier;
    std::string uri;
    unsigned int majorVersion;
    unsigned int minorVersion;

    static const std::string baseUri;
    static const std::string ext;
};


std::string DocTypeToString(RobotInterfaceDTD::DocType doctype);

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_ROBOTINTERFACEDTD_H
