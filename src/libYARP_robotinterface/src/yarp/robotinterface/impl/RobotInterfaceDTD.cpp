/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/impl/RobotInterfaceDTD.h>

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
#include <tinyxml.h>
#include <vector>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."

namespace yarp {
namespace robotinterface {

// Represent something like this in the xml file
// <!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 1.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV1.0.dtd">

const std::string RobotInterfaceDTD::baseUri("http://www.yarp.it/DTD/yarprobotinterfaceV");
const std::string RobotInterfaceDTD::ext(".dtd");


RobotInterfaceDTD::DocType StringToDocType(const std::string& type)
{
    if (type == "robot") {
        return RobotInterfaceDTD::DocTypeRobot;
    }
    if (type == "devices") {
        return RobotInterfaceDTD::DocTypeDevices;
    }
    if (type == "params") {
        return RobotInterfaceDTD::DocTypeParams;
    }
    if (type == "actions") {
        return RobotInterfaceDTD::DocTypeActions;
    }
    return RobotInterfaceDTD::DocTypeUnknown;
}

std::string DocTypeToString(RobotInterfaceDTD::DocType doctype)
{
    switch (doctype) {
    case RobotInterfaceDTD::DocTypeRobot:
        return std::string("robot");
    case RobotInterfaceDTD::DocTypeDevices:
        return std::string("devices");
    case RobotInterfaceDTD::DocTypeParams:
        return std::string("params");
    case RobotInterfaceDTD::DocTypeActions:
        return std::string("actions");
    default:
        return {};
    }
}

bool RobotInterfaceDTD::valid() const
{
    return type != DocTypeUnknown && majorVersion != 0;
}

void RobotInterfaceDTD::setDefault()
{
    type = RobotInterfaceDTD::DocTypeUnknown;
    identifier = "-//YARP//DTD yarprobotinterface 3.0//EN";
    uri = "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd";
    majorVersion = 1;
    minorVersion = 0;
}

bool RobotInterfaceDTD::parse(TiXmlUnknown* unknownNode, const std::string& curr_filename)
{
    // Very basic and ugly DTD tag parsing as TinyXML does not support it
    // We just need the version numbers.

    // Split tag in token
    std::istringstream iss(unknownNode->ValueStr());
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter<std::vector<std::string>>(tokens));

    // Merge token in quotes (and remove quotes)
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        if (it->at(0) == '"') {
            if (it->at(it->size() - 1) == '"') {
                *it = it->substr(1, it->size() - 2);
            } else {
                std::string s = it->substr(1) + " ";
                for (auto cit = it + 1; cit != tokens.end();) {
                    if (cit->at(cit->size() - 1) == '"') {
                        s += cit->substr(0, cit->size() - 1);
                        cit = tokens.erase(cit);
                        break;
                    }
                    s += *cit + " ";
                    cit = tokens.erase(cit);
                }
                *it = s;
            }
        }
    }

    if (tokens.size() != 5) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown node found" << tokens.size();
    }

    if (tokens.at(0) != "!DOCTYPE") {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown node found";
    }

    type = StringToDocType(tokens.at(1));
    if (type == RobotInterfaceDTD::DocTypeUnknown) {
        SYNTAX_WARNING(unknownNode->Row()) << R"(Unknown document type. Supported document types are: "robot", "devices", "params")";
    }

    if (tokens.at(2) != "PUBLIC") {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Expected \"PUBLIC\", found" << tokens.at(2);
    }

    identifier = tokens.at(3); // For now just skip checks on the identifier
    uri = tokens.at(4);

    // Extract version numbers from the URI
    std::size_t end1 = uri.find(RobotInterfaceDTD::ext, 0);
    if (end1 == std::string::npos) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::size_t start = RobotInterfaceDTD::baseUri.size();
    std::size_t end2 = uri.find(RobotInterfaceDTD::ext, start);
    if (end2 == std::string::npos) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::string versionString = uri.substr(start, end2 - start);
    std::size_t dot = versionString.find('.');
    if (dot == std::string::npos) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::string majorVersionString = versionString.substr(0, dot);
    std::string minorVersionString = versionString.substr(dot + 1);
    std::istringstream majiss(majorVersionString);
    if (!(majiss >> majorVersion)) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Missing version in Url" << uri;
    }
    std::istringstream miniss(minorVersionString);
    if (!(miniss >> minorVersion)) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Missing version in Url" << uri;
    }

    // If we got here, this is a valid DTD declaration
    return true;
}

} // namespace robotinterface
} // namespace yarp
