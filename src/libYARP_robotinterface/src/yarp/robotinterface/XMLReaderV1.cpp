/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include "XMLReader.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"
#include "Robot.h"
#include "Types.h"
#include "impl/XMLReaderFileVx.h"

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


// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0

class yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1
{
public:
    explicit privateXMLReaderFileV1(XMLReaderFileV1 *parent);
    virtual ~privateXMLReaderFileV1();

    yarp::robotinterface::Robot& readRobotFile(const std::string &fileName);
    yarp::robotinterface::Robot& readRobotTag(TiXmlElement *robotElem);

    yarp::robotinterface::DeviceList readDevices(TiXmlElement *devicesElem);
    yarp::robotinterface::Device readDeviceTag(TiXmlElement *deviceElem);
    yarp::robotinterface::DeviceList readDevicesTag(TiXmlElement *devicesElem);
    yarp::robotinterface::DeviceList readDevicesFile(const std::string &fileName);

    yarp::robotinterface::ParamList readParams(TiXmlElement *paramsElem);
    yarp::robotinterface::Param readParamTag(TiXmlElement *paramElem);
    yarp::robotinterface::Param readGroupTag(TiXmlElement *groupElem);
    yarp::robotinterface::ParamList readParamListTag(TiXmlElement *paramListElem);
    yarp::robotinterface::ParamList readSubDeviceTag(TiXmlElement *subDeviceElem);
    yarp::robotinterface::ParamList readParamsTag(TiXmlElement *paramsElem);
    yarp::robotinterface::ParamList readParamsFile(const std::string &fileName);

    yarp::robotinterface::ActionList readActions(TiXmlElement *actionsElem);
    yarp::robotinterface::Action readActionTag(TiXmlElement *actionElem);
    yarp::robotinterface::ActionList readActionsTag(TiXmlElement *actionsElem);
    yarp::robotinterface::ActionList readActionsFile(const std::string &fileName);

    XMLReaderFileV1 * const parent;
    std::string filename;
    std::string path;
#ifdef USE_DTD
    RobotInterfaceDTD dtd;
#endif

    Robot robot;
    bool verbose_output;
    std::string curr_filename;
    unsigned int minorVersion;
    unsigned int majorVersion;
};


yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::privateXMLReaderFileV1(XMLReaderFileV1 *p) :
        parent(p),
        minorVersion(0),
        majorVersion(0)
{
    verbose_output = false;
}

yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::~privateXMLReaderFileV1() = default;

yarp::robotinterface::Robot& yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readRobotFile(const std::string &fileName)
{
    filename = fileName;
#ifdef WIN32
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif

    curr_filename = fileName;
#ifdef WIN32
    path = filename.substr(0, filename.rfind("\\"));
#else // WIN32
    path = filename.substr(0, filename.rfind('/'));
#endif //WIN32

    yDebug() << "Reading file" << filename.c_str();
    auto* doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

#ifdef USE_DTD
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(dtd.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!dtd.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        dtd.setDefault();
        dtd.type = RobotInterfaceDTD::DocTypeRobot;
    }

    if(dtd.type != RobotInterfaceDTD::DocTypeRobot) {
        SYNTAX_WARNING(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeRobot)
                                       << ". Found" << DocTypeToString(dtd.type);
    }

    if(dtd.majorVersion != 1 || dtd.minorVersion != 0) {
        SYNTAX_WARNING(doc->Row()) << "Only yarprobotinterface DTD version 1.0 is supported";
    }
#endif

    readRobotTag(doc->RootElement());
    delete doc;

    // yDebug() << robot;

    return robot;
}

yarp::robotinterface::Robot& yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readRobotTag(TiXmlElement *robotElem)
{
    if (robotElem->ValueStr() != "robot") {
        SYNTAX_ERROR(robotElem->Row()) << "Root element should be \"robot\". Found" << robotElem->ValueStr();
    }

    if (robotElem->QueryStringAttribute("name", &robot.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(robotElem->Row()) << R"("robot" element should contain the "name" attribute)";
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
        SYNTAX_WARNING(robotElem->Row()) << R"("robot" element should contain the "build" attribute [unsigned int]. Assuming 0)";
        tmp = 0;
    }
    robot.build() = (unsigned)tmp;
#endif

    if (robotElem->QueryStringAttribute("portprefix", &robot.portprefix()) != TIXML_SUCCESS) {
        SYNTAX_WARNING(robotElem->Row()) << R"("robot" element should contain the "portprefix" attribute. Using "name" attribute)";
        robot.portprefix() = robot.name();
    }

    // yDebug() << "Found robot [" << robot.name() << "] build [" << robot.build() << "] portprefix [" << robot.portprefix() << "]";

    for (TiXmlElement* childElem = robotElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() == "device" || childElem->ValueStr() == "devices") {
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



yarp::robotinterface::DeviceList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readDevices(TiXmlElement *devicesElem)
{
    const std::string &valueStr = devicesElem->ValueStr();

    if (valueStr == "device") {
        // yDebug() << valueStr;
        DeviceList deviceList;
        deviceList.push_back(readDeviceTag(devicesElem));
        return deviceList;
    }
    else if (valueStr == "devices") {
        // "devices"
        return readDevicesTag(devicesElem);
    }
    else
    {
        SYNTAX_ERROR(devicesElem->Row()) << R"(Expected "device" or "devices". Found)" << valueStr;
    }
    return DeviceList();
}

yarp::robotinterface::Device yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readDeviceTag(TiXmlElement *deviceElem)
{
    const std::string &valueStr = deviceElem->ValueStr();

    if (valueStr != "device") {
        SYNTAX_ERROR(deviceElem->Row()) << "Expected \"device\". Found" << valueStr;
    }

    Device device;

    if (deviceElem->QueryStringAttribute("name", &device.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "name" attribute)";
    }

    // yDebug() << "Found device [" << device.name() << "]";

    if (deviceElem->QueryStringAttribute("type", &device.type()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "type" attribute)";
    }

    device.params().push_back(Param("robotName", robot.portprefix()));

    for (TiXmlElement* childElem = deviceElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() == "action" ||
            childElem->ValueStr() == "actions") {
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

yarp::robotinterface::DeviceList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readDevicesTag(TiXmlElement *devicesElem)
{
    //const std::string &valueStr = devicesElem->ValueStr();

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

    /*
    if (valueStr.compare("devices") != 0) {
        SYNTAX_ERROR(devicesElem->Row()) << "Expected \"devices\". Found" << valueStr;
    }
    */
    /*
    std::string robotName;
    if (devicesElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(devicesElem->Row()) << "\"devices\" element should contain the \"robot\" attribute";
    }

    if (robotName != robot.name()) {
        SYNTAX_WARNING(devicesElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << robot.name();
    }
    */
    /*
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
    */
    DeviceList devices;
    for (TiXmlElement* childElem = devicesElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        DeviceList childDevices = readDevices(childElem);
        for (DeviceList::const_iterator it = childDevices.begin(); it != childDevices.end(); ++it) {
            devices.push_back(*it);
        }
    }

    return devices;
}

yarp::robotinterface::DeviceList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readDevicesFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    yDebug() << "Reading file" << fileName.c_str();
    auto* doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

#ifdef USE_DTD
    RobotInterfaceDTD devicesFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(devicesFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!devicesFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        devicesFileDTD.setDefault();
        devicesFileDTD.type = RobotInterfaceDTD::DocTypeDevices;
    }

    if (devicesFileDTD.type != RobotInterfaceDTD::DocTypeDevices) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeDevices)
                                       << ". Found" << DocTypeToString(devicesFileDTD.type);
    }

    if (devicesFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different yarprobotinterface DTD version";
    }
#endif

    yarp::robotinterface::DeviceList devices = readDevicesTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return devices;
}



yarp::robotinterface::ParamList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readParams(TiXmlElement* paramsElem)
{
    const std::string &valueStr = paramsElem->ValueStr();

    if (valueStr == "param") {
        ParamList params;
        params.push_back(readParamTag(paramsElem));
        return params;
    } else if (valueStr == "group") {
        ParamList params;
        params.push_back(readGroupTag(paramsElem));
        return params;
    } else if (valueStr == "paramlist") {
        return readParamListTag(paramsElem);
    } else if (valueStr == "subdevice") {
        return readSubDeviceTag(paramsElem);
    } else if (valueStr == "params") {
        return readParamsTag(paramsElem);
    }
    else
    {
        SYNTAX_ERROR(paramsElem->Row()) << R"(Expected "param", "group", "paramlist", "subdevice", or "params". Found)" << valueStr;
    }
    return ParamList();
}


yarp::robotinterface::Param yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readParamTag(TiXmlElement *paramElem)
{
    if (paramElem->ValueStr() != "param") {
        SYNTAX_ERROR(paramElem->Row()) << "Expected \"param\". Found" << paramElem->ValueStr();
    }

    Param param;

    if (paramElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should contain the "name" attribute)";
    }

    // yDebug() << "Found param [" << param.name() << "]";

    const char *valueText = paramElem->GetText();
    if (!valueText)
    {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should have a value [ "name" = )" << param.name() << "]";
    }
    else
    {
        param.value() = valueText;
    }

    // yDebug() << param;
    return param;
}

yarp::robotinterface::Param yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readGroupTag(TiXmlElement* groupElem)
{
    if (groupElem->ValueStr() != "group") {
        SYNTAX_ERROR(groupElem->Row()) << "Expected \"group\". Found" << groupElem->ValueStr();
    }

    Param group(true);

    if (groupElem->QueryStringAttribute("name", &group.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(groupElem->Row()) << R"("group" element should contain the "name" attribute)";
    }

    // yDebug() << "Found group [" << group.name() << "]";

    ParamList params;
    for (TiXmlElement* childElem = groupElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(*it);
        }
    }
    if (params.empty()) {
        SYNTAX_ERROR(groupElem->Row()) << "\"group\" cannot be empty";
    }

    std::string groupString;
    for (auto& param : params) {
        if (!groupString.empty()) {
            groupString += " ";
        }
        groupString += "(" + param.name() + " " + param.value() + ")";
    }

    group.value() = groupString;

    return group;
}

yarp::robotinterface::ParamList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readParamListTag(TiXmlElement* paramListElem)
{
    if (paramListElem->ValueStr() != "paramlist") {
        SYNTAX_ERROR(paramListElem->Row()) << "Expected \"paramlist\". Found" << paramListElem->ValueStr();
    }

    ParamList params;
    Param mainparam;

    if (paramListElem->QueryStringAttribute("name", &mainparam.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramListElem->Row()) << R"("paramlist" element should contain the "name" attribute)";
    }

    params.push_back(mainparam);

    // yDebug() << "Found paramlist [" << params.at(0).name() << "]";

    for (TiXmlElement* childElem = paramListElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() != "elem") {
            SYNTAX_ERROR(childElem->Row()) << "Expected \"elem\". Found" << childElem->ValueStr();
        }

        Param childParam;

        if (childElem->QueryStringAttribute("name", &childParam.name()) != TIXML_SUCCESS) {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should contain the "name" attribute)";
        }

        const char *valueText = childElem->GetText();
        if (!valueText)
        {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should have a value [ "name" = )" << childParam.name() << "]";
        }
        else
        {
            childParam.value() = valueText;
        }

        params.push_back(childParam);
    }

    if (params.empty()) {
        SYNTAX_ERROR(paramListElem->Row()) << "\"paramlist\" cannot be empty";
    }

    // +1 skips the first element, that is the main param
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        Param &param = *it;
        params.at(0).value() += (params.at(0).value().empty() ? "(" : " ") + param.name();
    }
    params.at(0).value() += ")";

    // yDebug() << params;
    return params;
}

yarp::robotinterface::ParamList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readSubDeviceTag(TiXmlElement *subDeviceElem)
{
    if (subDeviceElem->ValueStr() != "subdevice") {
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
        SYNTAX_ERROR(subDeviceElem->Row()) << R"("subdevice" element should contain the "type" attribute)";
    }

//FIXME    params.push_back(featIdParam);
    params.push_back(subDeviceParam);

    // yDebug() << "Found subdevice [" << params.at(0).value() << "]";

    for (TiXmlElement* childElem = subDeviceElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(Param(it->name(), it->value()));
        }
    }

    // yDebug() << params;
    return params;
}

yarp::robotinterface::ParamList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readParamsTag(TiXmlElement *paramsElem)
{
    //const std::string &valueStr = paramsElem->ValueStr();

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

    /*std::string robotName;
    if (paramsElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(paramsElem->Row()) << "\"params\" element should contain the \"robot\" attribute";
    }

    if (robotName != robot.name()) {
        SYNTAX_WARNING(paramsElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << robot.name();
    }*/

    /*
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
    */
    ParamList params;
    for (TiXmlElement* childElem = paramsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(*it);
        }
    }

    return params;
}

yarp::robotinterface::ParamList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readParamsFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    yDebug() << "Reading file" << fileName.c_str();
    auto* doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

#ifdef USE_DTD
    RobotInterfaceDTD paramsFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(paramsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!paramsFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        paramsFileDTD.setDefault();
        paramsFileDTD.type = RobotInterfaceDTD::DocTypeParams;
    }

    if (paramsFileDTD.type != RobotInterfaceDTD::DocTypeParams) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeParams)
                                       << ". Found" << DocTypeToString(paramsFileDTD.type);
    }

    if (paramsFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different yarprobotinterface DTD version";
    }

#endif
    yarp::robotinterface::ParamList params = readParamsTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return params;
}

yarp::robotinterface::ActionList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readActions(TiXmlElement *actionsElem)
{
    const std::string &valueStr = actionsElem->ValueStr();

    if (valueStr != "action" &&
        valueStr != "actions")
    {
        SYNTAX_ERROR(actionsElem->Row()) << R"(Expected "action" or "actions". Found)" << valueStr;
    }

    if (valueStr == "action") {
        ActionList actionList;
        actionList.push_back(readActionTag(actionsElem));
        return actionList;
    }
    // "actions"
    return readActionsTag(actionsElem);
}

yarp::robotinterface::Action yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readActionTag(TiXmlElement* actionElem)
{
    if (actionElem->ValueStr() != "action") {
        SYNTAX_ERROR(actionElem->Row()) << "Expected \"action\". Found" << actionElem->ValueStr();
    }

    Action action;

    if (actionElem->QueryValueAttribute<ActionPhase>("phase", &action.phase()) != TIXML_SUCCESS || action.phase() == ActionPhaseUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "phase" attribute [startup|interrupt{1,2,3}|shutdown])";
    }


    if (actionElem->QueryValueAttribute<ActionType>("type", &action.type()) != TIXML_SUCCESS || action.type() == ActionTypeUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "type" attribute [configure|calibrate|attach|abort|detach|park|custom])";
    }

    // yDebug() << "Found action [ ]";

#if TINYXML_UNSIGNED_INT_BUG
    if (actionElem->QueryUnsignedAttribute("level", &action.level()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(actionElem->Row()) << "\"action\" element should contain the \"level\" attribute [unsigned int]";
    }
#else
    int tmp;
    if (actionElem->QueryIntAttribute("level", &tmp) != TIXML_SUCCESS || tmp < 0) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "level" attribute [unsigned int])";
    }
    action.level() = (unsigned)tmp;
#endif

    for (TiXmlElement* childElem = actionElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            action.params().push_back(*it);
        }
    }

    // yDebug() << action;
    return action;
}

yarp::robotinterface::ActionList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readActionsTag(TiXmlElement *actionsElem)
{
    //const std::string &valueStr = actionsElem->ValueStr();

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
        SYNTAX_WARNING(actionsElem->Row()) << R"("actions" element should contain the "robot" attribute)";
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
        SYNTAX_WARNING(actionsElem->Row()) << R"("actions" element should contain the "build" attribute [unsigned int]. Assuming 0)";
        tmp = 0;
    }
    build = (unsigned)tmp;
#endif

    if (build != robot.build()) {
        SYNTAX_WARNING(actionsElem->Row()) << "Import a file for a different robot build. Found" << build << "instead of" << robot.build();
    }

    ActionList actions;
    for (TiXmlElement* childElem = actionsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        ActionList childActions = readActions(childElem);
        for (ActionList::const_iterator it = childActions.begin(); it != childActions.end(); ++it) {
            actions.push_back(*it);
        }
    }

    return actions;
}

yarp::robotinterface::ActionList yarp::robotinterface::XMLReaderFileV1::privateXMLReaderFileV1::readActionsFile(const std::string &fileName)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    yDebug() << "Reading file" << fileName.c_str();
    auto* doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
    }

#ifdef USE_DTD
    RobotInterfaceDTD actionsFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if(actionsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!actionsFileDTD.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        actionsFileDTD.setDefault();
        actionsFileDTD.type = RobotInterfaceDTD::DocTypeActions;
    }

    if (actionsFileDTD.type != RobotInterfaceDTD::DocTypeActions) {
        SYNTAX_ERROR(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeActions)
                                 << ". Found" << DocTypeToString(actionsFileDTD.type);
    }

    if (actionsFileDTD.majorVersion != dtd.majorVersion) {
        SYNTAX_ERROR(doc->Row()) << "Trying to import a file with a different yarprobotinterface DTD version";
    }

#endif
    yarp::robotinterface::ActionList actions = readActionsTag(doc->RootElement());
    delete doc;
    curr_filename = old_filename;
    return actions;
}

yarp::robotinterface::Robot& yarp::robotinterface::XMLReaderFileV1::getRobotFile(const std::string& filename, bool verb)
{
    mPriv->verbose_output = verb;
    return mPriv->readRobotFile(filename);
}

yarp::robotinterface::XMLReaderFileV1::XMLReaderFileV1() : mPriv(new privateXMLReaderFileV1(this))
{

}

yarp::robotinterface::XMLReaderFileV1::~XMLReaderFileV1()
{
    if (mPriv)
    {
        delete mPriv;
    }
}
