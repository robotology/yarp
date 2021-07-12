/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_IMPL_ROBOTINTERFACEDTD_H
#define YARP_ROBOTINTERFACE_IMPL_ROBOTINTERFACEDTD_H

#include <yarp/robotinterface/api.h>
#include <yarp/robotinterface/Action.h>
#include <yarp/robotinterface/Device.h>
#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/Robot.h>
#include <yarp/robotinterface/Types.h>
#include <yarp/robotinterface/XMLReader.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class TiXmlUnknown;

namespace yarp {
namespace robotinterface {

// Represent something like this in the xml file
// <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 1.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV1.0.dtd">
class RobotInterfaceDTD
{
public:
    enum DocType
    {
        DocTypeUnknown = 0,
        DocTypeRobot,
        DocTypeDevices,
        DocTypeParams,
        DocTypeActions,
    };

    RobotInterfaceDTD() = default;

    bool parse(TiXmlUnknown* unknownNode, const std::string& curr_filename);

    bool valid() const;

    void setDefault();

    DocType type {DocTypeUnknown};
    std::string identifier;
    std::string uri;
    unsigned int majorVersion {0};
    unsigned int minorVersion {0};

    static const std::string baseUri;
    static const std::string ext;
};


std::string DocTypeToString(RobotInterfaceDTD::DocType doctype);

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_ROBOTINTERFACEDTD_H
