/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/impl/XMLReaderFileV1.h>

#include <yarp/robotinterface/experimental/Action.h>
#include <yarp/robotinterface/experimental/Device.h>
#include <yarp/robotinterface/experimental/Param.h>
#include <yarp/robotinterface/experimental/Robot.h>
#include <yarp/robotinterface/experimental/Types.h>
#include <yarp/robotinterface/experimental/XMLReader.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <tinyxml.h>
#include <utility>
#include <vector>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."

// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0

class yarp::robotinterface::impl::XMLReaderFileV1::Private
{
public:
    explicit Private(XMLReaderFileV1* parent);
    virtual ~Private();

    yarp::robotinterface::experimental::XMLReaderResult readRobotFromFile(const std::string& fileName);
    yarp::robotinterface::experimental::XMLReaderResult readRobotFromString(const std::string& xmlString);
    yarp::robotinterface::experimental::XMLReaderResult readRobotTag(TiXmlElement* robotElem);

    yarp::robotinterface::experimental::DeviceList readDevices(TiXmlElement* devicesElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::Device readDeviceTag(TiXmlElement* deviceElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::DeviceList readDevicesTag(TiXmlElement* devicesElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::DeviceList readDevicesFile(const std::string& fileName, yarp::robotinterface::experimental::XMLReaderResult& result);

    yarp::robotinterface::experimental::ParamList readParams(TiXmlElement* paramsElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::Param readParamTag(TiXmlElement* paramElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::Param readGroupTag(TiXmlElement* groupElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ParamList readParamListTag(TiXmlElement* paramListElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ParamList readSubDeviceTag(TiXmlElement* subDeviceElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ParamList readParamsTag(TiXmlElement* paramsElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ParamList readParamsFile(const std::string& fileName, yarp::robotinterface::experimental::XMLReaderResult& result);

    yarp::robotinterface::experimental::ActionList readActions(TiXmlElement* actionsElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::Action readActionTag(TiXmlElement* actionElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ActionList readActionsTag(TiXmlElement* actionsElem, yarp::robotinterface::experimental::XMLReaderResult& result);
    yarp::robotinterface::experimental::ActionList readActionsFile(const std::string& fileName, yarp::robotinterface::experimental::XMLReaderResult& result);

    XMLReaderFileV1* const parent;
    std::string filename;
    std::string path;
#ifdef USE_DTD
    RobotInterfaceDTD dtd;
#endif

    bool verbose_output;
    std::string curr_filename;
    unsigned int minorVersion;
    unsigned int majorVersion;
};


yarp::robotinterface::impl::XMLReaderFileV1::Private::Private(XMLReaderFileV1* p) :
        parent(p),
        minorVersion(0),
        majorVersion(0)
{
    verbose_output = false;
}

yarp::robotinterface::impl::XMLReaderFileV1::Private::~Private() = default;

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV1::Private::readRobotFromFile(const std::string& fileName)
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
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

#ifdef USE_DTD
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if (dtd.parse(childNode->ToUnknown(), curr_filename)) {
                break;
            }
        }
    }

    if (!dtd.valid()) {
        SYNTAX_WARNING(doc->Row()) << "No DTD found. Assuming version yarprobotinterfaceV1.0";
        dtd.setDefault();
        dtd.type = RobotInterfaceDTD::DocTypeRobot;
    }

    if (dtd.type != RobotInterfaceDTD::DocTypeRobot) {
        SYNTAX_WARNING(doc->Row()) << "Expected document of type" << DocTypeToString(RobotInterfaceDTD::DocTypeRobot)
                                   << ". Found" << DocTypeToString(dtd.type);
    }

    if (dtd.majorVersion != 1 || dtd.minorVersion != 0) {
        SYNTAX_WARNING(doc->Row()) << "Only yarprobotinterface DTD version 1.0 is supported";
    }
#endif

    yarp::robotinterface::experimental::XMLReaderResult result = readRobotTag(doc->RootElement());
    delete doc;

    // yDebug() << robot;

    return result;
}

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV1::Private::readRobotFromString(const std::string& xmlString)
{
    curr_filename = " XML runtime string ";
    auto* doc = new TiXmlDocument();
    if (!doc->Parse(xmlString.c_str())) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    yarp::robotinterface::experimental::XMLReaderResult result = readRobotTag(doc->RootElement());
    delete doc;

    return result;
}


yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV1::Private::readRobotTag(TiXmlElement* robotElem)
{
    yarp::robotinterface::experimental::XMLReaderResult result;
    result.parsingIsSuccessful = true;

    if (robotElem->ValueStr() != "robot") {
        SYNTAX_ERROR(robotElem->Row()) << "Root element should be \"robot\". Found" << robotElem->ValueStr();
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
    }

    if (robotElem->QueryStringAttribute("name", &result.robot.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(robotElem->Row()) << R"("robot" element should contain the "name" attribute)";
        return yarp::robotinterface::experimental::XMLReaderResult::ParsingFailed();
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
    result.robot.build() = static_cast<unsigned>(tmp);
#endif

    if (robotElem->QueryStringAttribute("portprefix", &result.robot.portprefix()) != TIXML_SUCCESS) {
        SYNTAX_WARNING(robotElem->Row()) << R"("robot" element should contain the "portprefix" attribute. Using "name" attribute)";
        result.robot.portprefix() = result.robot.name();
    }

    // yDebug() << "Found robot [" << robot.name() << "] build [" << robot.build() << "] portprefix [" << robot.portprefix() << "]";

    for (TiXmlElement* childElem = robotElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() == "device" || childElem->ValueStr() == "devices") {
            for (const auto& childDevice : readDevices(childElem, result)) {
                result.robot.devices().push_back(childDevice);
            }
        } else {
            for (const auto& childParam : readParams(childElem, result)) {
                result.robot.params().push_back(childParam);
            }
        }
    }

    return result;
}


yarp::robotinterface::experimental::DeviceList yarp::robotinterface::impl::XMLReaderFileV1::Private::readDevices(TiXmlElement* devicesElem,
                                                                                                                 yarp::robotinterface::experimental::XMLReaderResult& result)
{
    const std::string& valueStr = devicesElem->ValueStr();

    if (valueStr == "device") {
        // yDebug() << valueStr;
        yarp::robotinterface::experimental::DeviceList deviceList;
        deviceList.push_back(readDeviceTag(devicesElem, result));
        return deviceList;
    }

    if (valueStr == "devices") {
        // "devices"
        return readDevicesTag(devicesElem, result);
    }

    SYNTAX_ERROR(devicesElem->Row()) << R"(Expected "device" or "devices". Found)" << valueStr;
    result.parsingIsSuccessful = false;
    return yarp::robotinterface::experimental::DeviceList();
}

yarp::robotinterface::experimental::Device yarp::robotinterface::impl::XMLReaderFileV1::Private::readDeviceTag(TiXmlElement* deviceElem,
                                                                                                               yarp::robotinterface::experimental::XMLReaderResult& result)
{
    const std::string& valueStr = deviceElem->ValueStr();

    if (valueStr != "device") {
        SYNTAX_ERROR(deviceElem->Row()) << "Expected \"device\". Found" << valueStr;
        result.parsingIsSuccessful = false;
    }

    yarp::robotinterface::experimental::Device device;

    if (deviceElem->QueryStringAttribute("name", &device.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
    }

    // yDebug() << "Found device [" << device.name() << "]";

    if (deviceElem->QueryStringAttribute("type", &device.type()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "type" attribute)";
        result.parsingIsSuccessful = false;
    }

    device.params().push_back(yarp::robotinterface::experimental::Param("robotName", result.robot.portprefix()));

    for (TiXmlElement* childElem = deviceElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() == "action" || childElem->ValueStr() == "actions") {
            for (const auto& childAction : readActions(childElem, result)) {
                device.actions().push_back(childAction);
            }
        } else {
            for (const auto& childParam : readParams(childElem, result)) {
                device.params().push_back(childParam);
            }
        }
    }

    // yDebug() << device;
    return device;
}

yarp::robotinterface::experimental::DeviceList yarp::robotinterface::impl::XMLReaderFileV1::Private::readDevicesTag(TiXmlElement* devicesElem,
                                                                                                                    yarp::robotinterface::experimental::XMLReaderResult& result)
{
    std::string filename;
    if (devicesElem->QueryStringAttribute("file", &filename) == TIXML_SUCCESS) {
        // yDebug() << "Found devices file [" << filename << "]";
#ifdef WIN32
        std::replace(filename.begin(), filename.end(), '/', '\\');
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readDevicesFile(filename, result);
    }

    yarp::robotinterface::experimental::DeviceList devices;
    for (TiXmlElement* childElem = devicesElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childDevice : readDevices(childElem, result)) {
            devices.push_back(childDevice);
        }
    }

    return devices;
}

yarp::robotinterface::experimental::DeviceList yarp::robotinterface::impl::XMLReaderFileV1::Private::readDevicesFile(const std::string& fileName,
                                                                                                                     yarp::robotinterface::experimental::XMLReaderResult& result)
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
            if (devicesFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
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

    yarp::robotinterface::experimental::DeviceList devices = readDevicesTag(doc->RootElement(), result);
    delete doc;
    curr_filename = old_filename;
    return devices;
}


yarp::robotinterface::experimental::ParamList yarp::robotinterface::impl::XMLReaderFileV1::Private::readParams(TiXmlElement* paramsElem,
                                                                                                               yarp::robotinterface::experimental::XMLReaderResult& result)
{
    const std::string& valueStr = paramsElem->ValueStr();

    if (valueStr == "param") {
        yarp::robotinterface::experimental::ParamList params;
        params.push_back(readParamTag(paramsElem, result));
        return params;
    }
    if (valueStr == "group") {
        yarp::robotinterface::experimental::ParamList params;
        params.push_back(readGroupTag(paramsElem, result));
        return params;
    }
    if (valueStr == "paramlist") {
        return readParamListTag(paramsElem, result);
    }
    if (valueStr == "subdevice") {
        return readSubDeviceTag(paramsElem, result);
    }
    if (valueStr == "params") {
        return readParamsTag(paramsElem, result);
    }
    SYNTAX_ERROR(paramsElem->Row()) << R"(Expected "param", "group", "paramlist", "subdevice", or "params". Found)" << valueStr;
    return yarp::robotinterface::experimental::ParamList();
}


yarp::robotinterface::experimental::Param yarp::robotinterface::impl::XMLReaderFileV1::Private::readParamTag(TiXmlElement* paramElem,
                                                                                                             yarp::robotinterface::experimental::XMLReaderResult& result)
{
    if (paramElem->ValueStr() != "param") {
        SYNTAX_ERROR(paramElem->Row()) << "Expected \"param\". Found" << paramElem->ValueStr();
        result.parsingIsSuccessful = false;
    }

    yarp::robotinterface::experimental::Param param;

    if (paramElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
    }

    // yDebug() << "Found param [" << param.name() << "]";

    const char* valueText = paramElem->GetText();
    if (!valueText) {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should have a value [ "name" = )" << param.name() << "]";
    } else {
        param.value() = valueText;
    }

    // yDebug() << param;
    return param;
}

yarp::robotinterface::experimental::Param yarp::robotinterface::impl::XMLReaderFileV1::Private::readGroupTag(TiXmlElement* groupElem,
                                                                                                             yarp::robotinterface::experimental::XMLReaderResult& result)
{
    if (groupElem->ValueStr() != "group") {
        SYNTAX_ERROR(groupElem->Row()) << "Expected \"group\". Found" << groupElem->ValueStr();
        result.parsingIsSuccessful = false;
    }

    yarp::robotinterface::experimental::Param group(true);

    if (groupElem->QueryStringAttribute("name", &group.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(groupElem->Row()) << R"("group" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
    }

    // yDebug() << "Found group [" << group.name() << "]";

    yarp::robotinterface::experimental::ParamList params;
    for (TiXmlElement* childElem = groupElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childParam : readParams(childElem, result)) {
            params.push_back(childParam);
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

yarp::robotinterface::experimental::ParamList yarp::robotinterface::impl::XMLReaderFileV1::Private::readParamListTag(TiXmlElement* paramListElem,
                                                                                                                     yarp::robotinterface::experimental::XMLReaderResult& result)
{
    if (paramListElem->ValueStr() != "paramlist") {
        SYNTAX_ERROR(paramListElem->Row()) << "Expected \"paramlist\". Found" << paramListElem->ValueStr();
        result.parsingIsSuccessful = false;
    }

    yarp::robotinterface::experimental::ParamList params;
    yarp::robotinterface::experimental::Param mainparam;

    if (paramListElem->QueryStringAttribute("name", &mainparam.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramListElem->Row()) << R"("paramlist" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
    }

    params.push_back(mainparam);

    // yDebug() << "Found paramlist [" << params.at(0).name() << "]";

    for (TiXmlElement* childElem = paramListElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() != "elem") {
            SYNTAX_ERROR(childElem->Row()) << "Expected \"elem\". Found" << childElem->ValueStr();
        }

        yarp::robotinterface::experimental::Param childParam;

        if (childElem->QueryStringAttribute("name", &childParam.name()) != TIXML_SUCCESS) {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should contain the "name" attribute)";
            result.parsingIsSuccessful = false;
        }

        const char* valueText = childElem->GetText();
        if (!valueText) {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should have a value [ "name" = )" << childParam.name() << "]";
            result.parsingIsSuccessful = false;
        } else {
            childParam.value() = valueText;
        }

        params.push_back(childParam);
    }

    if (params.empty()) {
        SYNTAX_ERROR(paramListElem->Row()) << "\"paramlist\" cannot be empty";
        result.parsingIsSuccessful = false;
    }

    // +1 skips the first element, that is the main param
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        yarp::robotinterface::experimental::Param& param = *it;
        params.at(0).value() += (params.at(0).value().empty() ? "(" : " ") + param.name();
    }
    params.at(0).value() += ")";

    // yDebug() << params;
    return params;
}

yarp::robotinterface::experimental::ParamList yarp::robotinterface::impl::XMLReaderFileV1::Private::readSubDeviceTag(TiXmlElement* subDeviceElem,
                                                                                                                     yarp::robotinterface::experimental::XMLReaderResult& result)
{
    if (subDeviceElem->ValueStr() != "subdevice") {
        SYNTAX_ERROR(subDeviceElem->Row()) << "Expected \"subdevice\". Found" << subDeviceElem->ValueStr();
        result.parsingIsSuccessful = false;
    }

    yarp::robotinterface::experimental::ParamList params;

    //FIXME    Param featIdParam;
    yarp::robotinterface::experimental::Param subDeviceParam;

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
        for (const auto& childParam : readParams(childElem, result)) {
            params.push_back(yarp::robotinterface::experimental::Param(childParam.name(), childParam.value()));
        }
    }

    // yDebug() << params;
    return params;
}

yarp::robotinterface::experimental::ParamList yarp::robotinterface::impl::XMLReaderFileV1::Private::readParamsTag(TiXmlElement* paramsElem,
                                                                                                                  yarp::robotinterface::experimental::XMLReaderResult& result)
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
        return readParamsFile(filename, result);
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
    yarp::robotinterface::experimental::ParamList params;
    for (TiXmlElement* childElem = paramsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto & childParam : readParams(childElem, result)) {
            params.push_back(childParam);
        }
    }

    return params;
}

yarp::robotinterface::experimental::ParamList yarp::robotinterface::impl::XMLReaderFileV1::Private::readParamsFile(const std::string& fileName,
                                                                                                                   yarp::robotinterface::experimental::XMLReaderResult& result)
{
    std::string old_filename = curr_filename;
    curr_filename = fileName;

    yDebug() << "Reading file" << fileName.c_str();
    auto* doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::experimental::ParamList();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::experimental::ParamList();
    }

#ifdef USE_DTD
    RobotInterfaceDTD paramsFileDTD;
    for (TiXmlNode* childNode = doc->FirstChild(); childNode != 0; childNode = childNode->NextSibling()) {
        if (childNode->Type() == TiXmlNode::TINYXML_UNKNOWN) {
            if (paramsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
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
    yarp::robotinterface::experimental::ParamList params = readParamsTag(doc->RootElement(), result);
    delete doc;
    curr_filename = old_filename;
    return params;
}

yarp::robotinterface::experimental::ActionList yarp::robotinterface::impl::XMLReaderFileV1::Private::readActions(TiXmlElement* actionsElem,
                                                                                                                 yarp::robotinterface::experimental::XMLReaderResult& result)
{
    const std::string& valueStr = actionsElem->ValueStr();

    if (valueStr != "action" && valueStr != "actions") {
        SYNTAX_ERROR(actionsElem->Row()) << R"(Expected "action" or "actions". Found)" << valueStr;
    }

    if (valueStr == "action") {
        yarp::robotinterface::experimental::ActionList actionList;
        actionList.push_back(readActionTag(actionsElem, result));
        return actionList;
    }
    // "actions"
    return readActionsTag(actionsElem, result);
}

yarp::robotinterface::experimental::Action yarp::robotinterface::impl::XMLReaderFileV1::Private::readActionTag(TiXmlElement* actionElem,
                                                                                                               yarp::robotinterface::experimental::XMLReaderResult& result)
{
    if (actionElem->ValueStr() != "action") {
        SYNTAX_ERROR(actionElem->Row()) << "Expected \"action\". Found" << actionElem->ValueStr();
    }

    yarp::robotinterface::experimental::Action action;

    if (actionElem->QueryValueAttribute<yarp::robotinterface::experimental::ActionPhase>("phase", &action.phase()) != TIXML_SUCCESS || action.phase() == yarp::robotinterface::experimental::ActionPhaseUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "phase" attribute [startup|interrupt{1,2,3}|shutdown])";
    }


    if (actionElem->QueryValueAttribute<yarp::robotinterface::experimental::ActionType>("type", &action.type()) != TIXML_SUCCESS || action.type() == yarp::robotinterface::experimental::ActionTypeUnknown) {
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
    action.level() = static_cast<unsigned>(tmp);
#endif

    for (TiXmlElement* childElem = actionElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childParam : readParams(childElem, result)) {
            action.params().push_back(childParam);
        }
    }

    // yDebug() << action;
    return action;
}

yarp::robotinterface::experimental::ActionList yarp::robotinterface::impl::XMLReaderFileV1::Private::readActionsTag(TiXmlElement* actionsElem,
                                                                                                                    yarp::robotinterface::experimental::XMLReaderResult& result)
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
        return readActionsFile(filename, result);
    }

    std::string robotName;
    if (actionsElem->QueryStringAttribute("robot", &robotName) != TIXML_SUCCESS) {
        SYNTAX_WARNING(actionsElem->Row()) << R"("actions" element should contain the "robot" attribute)";
    }

    if (robotName != result.robot.name()) {
        SYNTAX_WARNING(actionsElem->Row()) << "Trying to import a file for the wrong robot. Found" << robotName << "instead of" << result.robot.name();
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
    build = static_cast<unsigned>(tmp);
#endif

    if (build != result.robot.build()) {
        SYNTAX_WARNING(actionsElem->Row()) << "Import a file for a different robot build. Found" << build << "instead of" << result.robot.build();
    }

    yarp::robotinterface::experimental::ActionList actions;
    for (TiXmlElement* childElem = actionsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childAction : readActions(childElem, result)) {
            actions.push_back(childAction);
        }
    }

    return actions;
}

yarp::robotinterface::experimental::ActionList yarp::robotinterface::impl::XMLReaderFileV1::Private::readActionsFile(const std::string& fileName,
                                                                                                                     yarp::robotinterface::experimental::XMLReaderResult& result)
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
            if (actionsFileDTD.parse(childNode->ToUnknown(), curr_filename)) {
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
    yarp::robotinterface::experimental::ActionList actions = readActionsTag(doc->RootElement(), result);
    delete doc;
    curr_filename = old_filename;
    return actions;
}

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV1::getRobotFromFile(const std::string& filename,
                                                                                                                  const yarp::os::Searchable& /*config*/,
                                                                                                                  bool verb)
{
    mPriv->verbose_output = verb;
    return mPriv->readRobotFromFile(filename);
}

yarp::robotinterface::experimental::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV1::getRobotFromString(const std::string& xmlString,
                                                                                                                    const yarp::os::Searchable& /*config*/,
                                                                                                                    bool verb)
{
    mPriv->verbose_output = verb;
    return mPriv->readRobotFromString(xmlString);
}

yarp::robotinterface::impl::XMLReaderFileV1::XMLReaderFileV1() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::impl::XMLReaderFileV1::~XMLReaderFileV1()
{
    delete mPriv;
}
