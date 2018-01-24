/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."


namespace RobotInterface {

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

    bool parse(TiXmlUnknown* unknownNode, std::string curr_filename);

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

} // namespace
