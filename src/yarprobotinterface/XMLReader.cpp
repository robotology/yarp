/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "XMLReader.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"
#include "Robot.h"
#include "Types.h"

#include <debugStream/Debug.h>

#include <tinyxml.h>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include <yarp/os/Property.h>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Syntax error while loading" << curr_filename << "at line" << line << "."


// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0


namespace {

// Represent something like this in the xml file
// <!DOCTYPE robot PUBLIC "-//YARP//DTD robotInterface 1.0//EN" "http://www.icub.org/DTD/robotInterfaceV1.0.dtd">
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

    bool valid() { return type != DocTypeUnknown && majorVersion != 0;}

    void setDefault() {
        type = RobotInterfaceDTD::DocTypeUnknown;
        identifier = "-//YARP//DTD robotInterface 1.0//EN";
        uri = "http://www.icub.org/DTD/robotInterfaceV1.0.dtd";
        majorVersion = 1;
        minorVersion = 0;
    }

    DocType type;
    std::string identifier;
    std::string uri;
    unsigned int majorVersion;
    unsigned int minorVersion;

    static const std::string baseUri;
    static const std::string ext;
};

const std::string RobotInterfaceDTD::baseUri("http://www.icub.org/DTD/robotInterfaceV");
const std::string RobotInterfaceDTD::ext(".dtd");


RobotInterfaceDTD::DocType StringToDocType(const std::string &type) {
    if (!type.compare("robot")) {
        return RobotInterfaceDTD::DocTypeRobot;
    } else if (!type.compare("devices")) {
        return RobotInterfaceDTD::DocTypeDevices;
    } else if (!type.compare("params")) {
        return RobotInterfaceDTD::DocTypeParams;
    } else if (!type.compare("actions")) {
        return RobotInterfaceDTD::DocTypeActions;
    }
    return RobotInterfaceDTD::DocTypeUnknown;
}

std::string DocTypeToString(RobotInterfaceDTD::DocType doctype) {
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
        return std::string();
    }
}

bool RobotInterfaceDTD::parse(TiXmlUnknown* unknownNode, std::string curr_filename) {
    // Very basic and ugly DTD tag parsing as TinyXML does not support it
    // We just need the version numbers.

    // Split tag in token
    std::istringstream iss(unknownNode->ValueStr());
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              std::back_inserter<std::vector<std::string> >(tokens));

    // Merge token in quotes (and remove quotes)
    for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        if(it->at(0) == '"' ) {
            if (it->at(it->size() - 1) == '"') {
                *it = it->substr(1, it->size() - 2);
            } else {
                std::string s = it->substr(1) + " ";
                for (std::vector<std::string>::iterator cit = it + 1; cit != tokens.end(); ) {
                    if (cit->at(cit->size() - 1) == '"') {
                        s += cit->substr(0, cit->size() - 1);
                        cit = tokens.erase(cit);
                        break;
                    } else {
                        s += *cit + " ";
                        cit = tokens.erase(cit);
                    }
                }
                *it = s;
            }
        }
    }

    if(tokens.size() != 5) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown node found" << tokens.size();
    }

    if(tokens.at(0) != "!DOCTYPE") {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown node found";
    }

    type = StringToDocType(tokens.at(1));
    if(type == RobotInterfaceDTD::DocTypeUnknown)
    {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Supported document types are: \"robot\", \"devices\", \"params\"";
    }

    if(tokens.at(2) != "PUBLIC") {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Expected \"PUBLIC\", found" << tokens.at(2);
    }

    identifier = tokens.at(3); // For now just skip checks on the identifier
    uri = tokens.at(4);

    // Extract version numbers from the URI
    if (uri.find(RobotInterfaceDTD::baseUri) != 0) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::size_t start = RobotInterfaceDTD::baseUri.size();
    std::size_t end = uri.find(RobotInterfaceDTD::ext, start);
    if (end == std::string::npos) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::string versionString = uri.substr(start, end - start);
    std::size_t dot = versionString.find('.');
    if (dot == std::string::npos) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Unknown url" << uri;
    }
    std::string majorVersionString = versionString.substr(0, dot);
    std::string minorVersionString = versionString.substr(dot + 1);
    std::istringstream majiss(majorVersionString);
    if ( !(majiss >> majorVersion) ) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Missing version in Url" << uri;
    }
    std::istringstream miniss(minorVersionString);
    if ( !(miniss >> minorVersion) ) {
        SYNTAX_WARNING(unknownNode->Row()) << "Unknown document type. Missing version in Url" << uri;
    }

    // If we got here, this is a valid DTD declaration
    return true;
}

} // namespace


class RobotInterface::XMLReader::Private
{
public:
    Private(XMLReader *parent);
    ~Private();

    RobotInterface::Robot& readRobotFile(const std::string &fileName);
    RobotInterface::Robot& readRobotTag(TiXmlElement *robotElem);

    RobotInterface::DeviceList readDevices(TiXmlElement *devicesElem);
    RobotInterface::Device readDeviceTag(TiXmlElement *deviceElem);
    RobotInterface::DeviceList readDevicesTag(TiXmlElement *devicesElem);
    RobotInterface::DeviceList readDevicesFile(const std::string &fileName);

    RobotInterface::ParamList readParams(TiXmlElement *paramsElem);
    RobotInterface::Param readParamTag(TiXmlElement *paramElem);
    RobotInterface::Param readGroupTag(TiXmlElement *groupElem);
    RobotInterface::ParamList readParamListTag(TiXmlElement *paramListElem);
    RobotInterface::ParamList readSubDeviceTag(TiXmlElement *subDeviceElem);
    RobotInterface::ParamList readParamsTag(TiXmlElement *paramsElem);
    RobotInterface::ParamList readParamsFile(const std::string &fileName);

    RobotInterface::ActionList readActions(TiXmlElement *actionsElem);
    RobotInterface::Action readActionTag(TiXmlElement *actionElem);
    RobotInterface::ActionList readActionsTag(TiXmlElement *actionsElem);
    RobotInterface::ActionList readActionsFile(const std::string &fileName);

    XMLReader * const parent;
    std::string filename;
    std::string path;
    RobotInterfaceDTD dtd;
    Robot robot;

    std::string curr_filename;
    unsigned int minorVersion;
    unsigned int majorVersion;
};


RobotInterface::XMLReader::Private::Private(XMLReader *p) :
    parent(p)
{
}

RobotInterface::XMLReader::Private::~Private()
{
}

RobotInterface::Robot& RobotInterface::XMLReader::Private::readRobotFile(const std::string &fileName)
{
    filename = fileName;
#ifdef WIN32
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif

    curr_filename = fileName;
#ifdef WIN32
    path = filename.substr(0, filename.rfind("\\"));
#else // WIN32
    path = filename.substr(0, filename.rfind("/"));
#endif //WIN32

    TiXmlDocument *doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(dtd.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!dtd.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version robotInterfaceV1.0";
        dtd.setDefault();
        dtd.type = RobotInterfaceDTD::DocTypeRobot;
    }

    if(dtd.type != RobotInterfaceDTD::DocTypeRobot) {
        SYNTAX_WARNING(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeRobot)
                                       << ". Found" << DocTypeToString(dtd.type);
    }

    if(dtd.majorVersion != 1 || dtd.minorVersion != 0) {
        SYNTAX_WARNING(doc->Row()) << "Only robotInterface DTD version 1.0 is supported";
    }

    readRobotTag(doc->RootElement());
    delete doc;

    // yDebug() << robot;

    return robot;
}

RobotInterface::Robot& RobotInterface::XMLReader::Private::readRobotTag(TiXmlElement *robotElem)
{
    if (robotElem->ValueStr().compare("robot") != 0) {
        SYNTAX_ERROR(robotElem->Row()) << "Root element should be \"robot\". Found" << robotElem->ValueStr();
    }

    if (robotElem->QueryStringAttribute("name", &robot.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(robotElem->Row()) << "\"robot\" element should contain the \"name\" attribute";
    }

#if TINYXML_UNSIGNED_INT_BUG
    if (robotElem->QueryUnsignedAttribute("build", &robot.build()) != TIXML_SUCCESS) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(robotElem->Row()) << "\"robot\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
    }
#else
    int tmp;
    if (robotElem->QueryIntAttribute("build", &tmp) != TIXML_SUCCESS || tmp < 0) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(robotElem->Row()) << "\"robot\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
        tmp = 0;
    }
    robot.build() = (unsigned)tmp;
#endif

    if (robotElem->QueryStringAttribute("portprefix", &robot.portprefix()) != TIXML_SUCCESS) {
        SYNTAX_WARNING(robotElem->Row()) << "\"robot\" element should contain the \"portprefix\" attribute. Using \"name\" attribute";
        robot.portprefix() = robot.name();
    }

    // yDebug() << "Found robot [" << robot.name() << "] build [" << robot.build() << "] portprefix [" << robot.portprefix() << "]";

    for (TiXmlElement* childElem = robotElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr().compare("device") == 0 || childElem->ValueStr().compare("devices") == 0) {
            DeviceList childDevices = readDevices(childElem);
            for (DeviceList::const_iterator it = childDevices.begin(); it != childDevices.end(); ++it) {
                robot.devices().push_back(*it);
            }
        } else {
            ParamList childParams = readParams(childElem);
            for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
                robot.params().push_back(*it);
            }
        }
    }

    return robot;
}



RobotInterface::DeviceList RobotInterface::XMLReader::Private::readDevices(TiXmlElement *devicesElem)
{
    const std::string &valueStr = devicesElem->ValueStr();

    if (valueStr.compare("device") != 0 &&
        valueStr.compare("devices") != 0)
    {
        SYNTAX_ERROR(devicesElem->Row()) << "Expected \"device\" or \"devices\". Found" << valueStr;
    }

    if (valueStr.compare("device") == 0) {
        // yDebug() << valueStr;
        DeviceList deviceList;
        deviceList.push_back(readDeviceTag(devicesElem));
        return deviceList;
    }
    // "devices"
    return readDevicesTag(devicesElem);
}

RobotInterface::Device RobotInterface::XMLReader::Private::readDeviceTag(TiXmlElement *deviceElem)
{
    const std::string &valueStr = deviceElem->ValueStr();

    if (valueStr.compare("device") != 0) {
        SYNTAX_ERROR(deviceElem->Row()) << "Expected \"device\". Found" << valueStr;
    }

    Device device;

    if (deviceElem->QueryStringAttribute("name", &device.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << "\"device\" element should contain the \"name\" attribute";
    }

    // yDebug() << "Found device [" << device.name() << "]";

    if (deviceElem->QueryStringAttribute("type", &device.type()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << "\"device\" element should contain the \"type\" attribute";
    }

    device.params().push_back(Param("robotName", robot.portprefix().c_str()));

    for (TiXmlElement* childElem = deviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr().compare("action") == 0 ||
            childElem->ValueStr().compare("actions") == 0) {
            ActionList childActions = readActions(childElem);
            for (ActionList::const_iterator it = childActions.begin(); it != childActions.end(); ++it) {
                device.actions().push_back(*it);
            }
        } else {
            ParamList childParams = readParams(childElem);
            for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
                device.params().push_back(*it);
            }
        }
    }

    // yDebug() << device;
    return device;
}

RobotInterface::DeviceList RobotInterface::XMLReader::Private::readDevicesTag(TiXmlElement *devicesElem)
{
    const std::string &valueStr = devicesElem->ValueStr();

    if (valueStr.compare("devices") != 0) {
        SYNTAX_ERROR(devicesElem->Row()) << "Expected \"devices\". Found" << valueStr;
    }

    std::string filename;
    if (devicesElem->QueryStringAttribute("file", &filename) == TIXML_SUCCESS) {
        // yDebug() << "Found devices file [" << filename << "]";
#ifdef WIN32
        std::replace(filename.begin(), filename.end(), '/', '\\');
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readDevicesFile(filename);
    }

    std::string robotName;
    if (devicesElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(devicesElem->Row()) << "\"devices\" element should contain the \"robot\" attribute";
    }

    if (robotName != robot.name()) {
        SYNTAX_WARNING(devicesElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << robot.name();
    }

    unsigned int build;
#if TINYXML_UNSIGNED_INT_BUG
    if (devicesElem->QueryUnsignedAttribute("build", &build()) != TIXML_SUCCESS) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(devicesElem->Row()) << "\"devices\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
    }
#else
    int tmp;
    if (devicesElem->QueryIntAttribute("build", &tmp) != TIXML_SUCCESS || tmp < 0) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(devicesElem->Row()) << "\"devices\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
        tmp = 0;
    }
    build = (unsigned)tmp;
#endif

    if (build != robot.build()) {
        SYNTAX_WARNING(devicesElem->Row()) << "Import a file for a different robot build. Found" << build << "instead of" << robot.build();
    }

    DeviceList devices;
    for (TiXmlElement* childElem = devicesElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        DeviceList childDevices = readDevices(childElem);
        for (DeviceList::const_iterator it = childDevices.begin(); it != childDevices.end(); ++it) {
            devices.push_back(*it);
        }
    }

    return devices;
}

RobotInterface::DeviceList RobotInterface::XMLReader::Private::readDevicesFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    TiXmlDocument *doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    RobotInterfaceDTD devicesFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(devicesFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!devicesFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version robotInterfaceV1.0";
        devicesFileDTD.setDefault();
        devicesFileDTD.type = RobotInterfaceDTD::DocTypeDevices;
    }

    if (devicesFileDTD.type != RobotInterfaceDTD::DocTypeDevices) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeDevices)
                                       << ". Found" << DocTypeToString(devicesFileDTD.type);
    }

    if (devicesFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different robotInterface DTD version";
    }

    RobotInterface::DeviceList devices = readDevicesTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return devices;
}



RobotInterface::ParamList RobotInterface::XMLReader::Private::readParams(TiXmlElement* paramsElem)
{
    const std::string &valueStr = paramsElem->ValueStr();

    if (valueStr.compare("param") != 0 &&
        valueStr.compare("group") != 0 &&
        valueStr.compare("paramlist") != 0 &&
        valueStr.compare("subdevice") != 0 &&
        valueStr.compare("params") != 0)
    {
        SYNTAX_ERROR(paramsElem->Row()) << "Expected \"param\", \"group\", \"paramlist\","
                << "\"subDevice\", or \"params\". Found" << valueStr;
    }

    if (valueStr.compare("param") == 0) {
        ParamList params;
        params.push_back(readParamTag(paramsElem));
        return params;
    } else if (valueStr.compare("group") == 0) {
        ParamList params;
        params.push_back(readGroupTag(paramsElem));
        return params;
    } else if (valueStr.compare("paramlist") == 0) {
        return readParamListTag(paramsElem);
    } else if (valueStr.compare("subdevice") == 0) {
        return readSubDeviceTag(paramsElem);
    }
    // "params"
    return readParamsTag(paramsElem);
}


RobotInterface::Param RobotInterface::XMLReader::Private::readParamTag(TiXmlElement *paramElem)
{
    if (paramElem->ValueStr().compare("param") != 0) {
        SYNTAX_ERROR(paramElem->Row()) << "Expected \"param\". Found" << paramElem->ValueStr();
    }

    Param param;

    if (paramElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramElem->Row()) << "\"param\" element should contain the \"name\" attribute";
    }

    // yDebug() << "Found param [" << param.name() << "]";

    const char *valueText = paramElem->GetText();
    if (!valueText) {
        SYNTAX_ERROR(paramElem->Row()) << "\"param\" element should have a value [ \"name\" = " << param.name() << "]";
    }
    param.value() = valueText;

    // yDebug() << param;
    return param;
}

RobotInterface::Param RobotInterface::XMLReader::Private::readGroupTag(TiXmlElement* groupElem)
{
    if (groupElem->ValueStr().compare("group") != 0) {
        SYNTAX_ERROR(groupElem->Row()) << "Expected \"group\". Found" << groupElem->ValueStr();
    }

    Param group(true);

    if (groupElem->QueryStringAttribute("name", &group.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(groupElem->Row()) << "\"group\" element should contain the \"name\" attribute";
    }

    // yDebug() << "Found group [" << group.name() << "]";

    ParamList params;
    for (TiXmlElement* childElem = groupElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(*it);
        }
    }
    if (params.empty()) {
        SYNTAX_ERROR(groupElem->Row()) << "\"group\" cannot be empty";
    }

    std::string groupString;
    for (ParamList::iterator it = params.begin(); it != params.end(); ++it) {
        if (!groupString.empty()) {
            groupString += " ";
        }
        groupString += "(" + it->name() + " " + it->value() + ")";
    }

    group.value() = groupString;

    return group;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readParamListTag(TiXmlElement* paramListElem)
{
    if (paramListElem->ValueStr().compare("paramlist") != 0) {
        SYNTAX_ERROR(paramListElem->Row()) << "Expected \"paramlist\". Found" << paramListElem->ValueStr();
    }

    ParamList params;
    Param mainparam;

    if (paramListElem->QueryStringAttribute("name", &mainparam.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramListElem->Row()) << "\"paramlist\" element should contain the \"name\" attribute";
    }

    params.push_back(mainparam);

    // yDebug() << "Found paramlist [" << params.at(0).name() << "]";

    for (TiXmlElement* childElem = paramListElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr().compare("elem") != 0) {
            SYNTAX_ERROR(childElem->Row()) << "Expected \"elem\". Found" << childElem->ValueStr();
        }

        Param childParam;

        if (childElem->QueryStringAttribute("name", &childParam.name()) != TIXML_SUCCESS) {
            SYNTAX_ERROR(childElem->Row()) << "\"elem\" element should contain the \"name\" attribute";
        }

        const char *valueText = childElem->GetText();
        if (!valueText) {
            SYNTAX_ERROR(childElem->Row()) << "\"elem\" element should have a value [ \"name\" = " << childParam.name() << "]";
        }
        childParam.value() = valueText;

        params.push_back(childParam);
    }

    if (params.empty()) {
        SYNTAX_ERROR(paramListElem->Row()) << "\"paramlist\" cannot be empty";
    }

    // +1 skips the first element, that is the main param
    for (ParamList::iterator it = params.begin() + 1; it != params.end(); ++it) {
        Param &param = *it;
        params.at(0).value() += (params.at(0).value().empty() ? "(" : " ") + param.name();
    }
    params.at(0).value() += ")";

    // yDebug() << params;
    return params;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readSubDeviceTag(TiXmlElement *subDeviceElem)
{
    if (subDeviceElem->ValueStr().compare("subdevice") != 0) {
        SYNTAX_ERROR(subDeviceElem->Row()) << "Expected \"subdevice\". Found" << subDeviceElem->ValueStr();
    }

    ParamList params;

//FIXME    Param featIdParam;
    Param subDeviceParam;

//FIXME    featIdParam.name() = "FeatId";
    subDeviceParam.name() = "subdevice";

//FIXME    if (subDeviceElem->QueryStringAttribute("name", &featIdParam.value()) != TIXML_SUCCESS) {
//        SYNTAX_ERROR(subDeviceElem->Row()) << "\"subdevice\" element should contain the \"name\" attribute";
//    }

    if (subDeviceElem->QueryStringAttribute("type", &subDeviceParam.value()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(subDeviceElem->Row()) << "\"subdevice\" element should contain the \"type\" attribute";
    }

//FIXME    params.push_back(featIdParam);
    params.push_back(subDeviceParam);

    // yDebug() << "Found subdevice [" << params.at(0).value() << "]";

    for (TiXmlElement* childElem = subDeviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(Param(it->name(), it->value()));
        }
    }

    // yDebug() << params;
    return params;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readParamsTag(TiXmlElement *paramsElem)
{
    const std::string &valueStr = paramsElem->ValueStr();

    if (valueStr.compare("params") != 0) {
        SYNTAX_ERROR(paramsElem->Row()) << "Expected \"params\". Found" << valueStr;
    }

    std::string filename;
    if (paramsElem->QueryStringAttribute("file", &filename) == TIXML_SUCCESS) {
        // yDebug() << "Found params file [" << filename << "]";
#ifdef WIN32
        std::replace(filename.begin(), filename.end(), '/', '\\');
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readParamsFile(filename);
    }

    std::string robotName;
    if (paramsElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(paramsElem->Row()) << "\"params\" element should contain the \"robot\" attribute";
    }

    if (robotName != robot.name()) {
        SYNTAX_WARNING(paramsElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << robot.name();
    }

    unsigned int build;
#if TINYXML_UNSIGNED_INT_BUG
    if (paramsElem->QueryUnsignedAttribute("build", &build()) != TIXML_SUCCESS) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(paramsElem->Row()) << "\"params\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
    }
#else
    int tmp;
    if (paramsElem->QueryIntAttribute("build", &tmp) != TIXML_SUCCESS || tmp < 0) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(paramsElem->Row()) << "\"params\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
        tmp = 0;
    }
    build = (unsigned)tmp;
#endif

    if (build != robot.build()) {
        SYNTAX_WARNING(paramsElem->Row()) << "Import a file for a different robot build. Found" << build << "instead of" << robot.build();
    }

    ParamList params;
    for (TiXmlElement* childElem = paramsElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(*it);
        }
    }

    return params;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readParamsFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    TiXmlDocument *doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    RobotInterfaceDTD paramsFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(paramsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!paramsFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version robotInterfaceV1.0";
        paramsFileDTD.setDefault();
        paramsFileDTD.type = RobotInterfaceDTD::DocTypeParams;
    }

    if (paramsFileDTD.type != RobotInterfaceDTD::DocTypeParams) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeParams)
                                       << ". Found" << DocTypeToString(paramsFileDTD.type);
    }

    if (paramsFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different robotInterface DTD version";
    }

    RobotInterface::ParamList params = readParamsTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return params;
}

RobotInterface::ActionList RobotInterface::XMLReader::Private::readActions(TiXmlElement *actionsElem)
{
    const std::string &valueStr = actionsElem->ValueStr();

    if (valueStr.compare("action") != 0 &&
        valueStr.compare("actions") != 0)
    {
        SYNTAX_ERROR(actionsElem->Row()) << "Expected \"action\" or \"actions\". Found" << valueStr;
    }

    if (valueStr.compare("action") == 0) {
        ActionList actionList;
        actionList.push_back(readActionTag(actionsElem));
        return actionList;
    }
    // "actions"
    return readActionsTag(actionsElem);
}

RobotInterface::Action RobotInterface::XMLReader::Private::readActionTag(TiXmlElement* actionElem)
{
    if (actionElem->ValueStr().compare("action") != 0) {
        SYNTAX_ERROR(actionElem->Row()) << "Expected \"action\". Found" << actionElem->ValueStr();
    }

    Action action;

    if (actionElem->QueryValueAttribute<ActionPhase>("phase", &action.phase()) != TIXML_SUCCESS || action.phase() == ActionPhaseUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << "\"action\" element should contain the \"phase\" attribute [startup|interrupt{1,2,3}|shutdown]";
    }


    if (actionElem->QueryValueAttribute<ActionType>("type", &action.type()) != TIXML_SUCCESS || action.type() == ActionTypeUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << "\"action\" element should contain the \"type\" attribute [configure|calibrate|attach|abort|detach|park|custom]";
    }

    // yDebug() << "Found action [ ]";

#if TINYXML_UNSIGNED_INT_BUG
    if (actionElem->QueryUnsignedAttribute("level", &action.level()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(actionElem->Row()) << "\"action\" element should contain the \"level\" attribute [unsigned int]";
    }
#else
    int tmp;
    if (actionElem->QueryIntAttribute("level", &tmp) != TIXML_SUCCESS || tmp < 0) {
        SYNTAX_ERROR(actionElem->Row()) << "\"action\" element should contain the \"level\" attribute [unsigned int]";
    }
    action.level() = (unsigned)tmp;
#endif

    for (TiXmlElement* childElem = actionElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            action.params().push_back(*it);
        }
    }

    // yDebug() << action;
    return action;
}

RobotInterface::ActionList RobotInterface::XMLReader::Private::readActionsTag(TiXmlElement *actionsElem)
{
    const std::string &valueStr = actionsElem->ValueStr();

    if (valueStr.compare("actions") != 0) {
        SYNTAX_ERROR(actionsElem->Row()) << "Expected \"actions\". Found" << valueStr;
    }

    std::string filename;
    if (actionsElem->QueryStringAttribute("file", &filename) == TIXML_SUCCESS) {
        // yDebug() << "Found actions file [" << filename << "]";
#ifdef WIN32
        std::replace(filename.begin(), filename.end(), '/', '\\');
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readActionsFile(filename);
    }

    std::string robotName;
    if (actionsElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(actionsElem->Row()) << "\"actions\" element should contain the \"robot\" attribute";
    }

    if (robotName != robot.name()) {
        SYNTAX_WARNING(actionsElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << robot.name();
    }

    unsigned int build;
#if TINYXML_UNSIGNED_INT_BUG
    if (actionsElem->QueryUnsignedAttribute("build", &build()) != TIXML_SUCCESS) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(actionsElem->Row()) << "\"actions\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
    }
#else
    int tmp;
    if (actionsElem->QueryIntAttribute("build", &tmp) != TIXML_SUCCESS || tmp < 0) {
        // No build attribute. Assuming build="0"
        SYNTAX_WARNING(actionsElem->Row()) << "\"actions\" element should contain the \"build\" attribute [unsigned int]. Assuming 0";
        tmp = 0;
    }
    build = (unsigned)tmp;
#endif

    if (build != robot.build()) {
        SYNTAX_WARNING(actionsElem->Row()) << "Import a file for a different robot build. Found" << build << "instead of" << robot.build();
    }

    ActionList actions;
    for (TiXmlElement* childElem = actionsElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ActionList childActions = readActions(childElem);
        for (ActionList::const_iterator it = childActions.begin(); it != childActions.end(); ++it) {
            actions.push_back(*it);
        }
    }

    return actions;
}

RobotInterface::ActionList RobotInterface::XMLReader::Private::readActionsFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    TiXmlDocument *doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

    RobotInterfaceDTD actionsFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(actionsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!actionsFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version robotInterfaceV1.0";
        actionsFileDTD.setDefault();
        actionsFileDTD.type = RobotInterfaceDTD::DocTypeActions;
    }

    if (actionsFileDTD.type != RobotInterfaceDTD::DocTypeActions) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeActions)
                                 << ". Found" << DocTypeToString(actionsFileDTD.type);
    }

    if (actionsFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different robotInterface DTD version";
    }

    RobotInterface::ActionList actions = readActionsTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return actions;
}


RobotInterface::XMLReader::XMLReader() :
    mPriv(new Private(this))
{
}

RobotInterface::XMLReader::~XMLReader()
{
    delete mPriv;
}

RobotInterface::Robot& RobotInterface::XMLReader::getRobot(const std::string& filename)
{
    return mPriv->readRobotFile(filename);
}
