/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "XMLReader.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"
#include "Robot.h"
#include "Types.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <tinyxml.h>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <yarp/robotinterface/api.h>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."

namespace yarp {
namespace robotinterface {

// Represent something like this in the xml file
// <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 1.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV1.0.dtd">
class YARP_robotinterface_API RobotInterfaceDTD
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


std::string YARP_robotinterface_API DocTypeToString(RobotInterfaceDTD::DocType doctype);

} // namespace robotinterface
} // namespace yarp
