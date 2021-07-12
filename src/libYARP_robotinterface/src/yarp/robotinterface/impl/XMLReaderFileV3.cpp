/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/impl/XMLReaderFileV3.h>

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

#define SYNTAX_ERROR(line) yError() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Invalid syntax while loading" << curr_filename << "at line" << line << "."

// BUG in TinyXML, see
// https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
// When this bug is fixed upstream we can enable this
#define TINYXML_UNSIGNED_INT_BUG 0

class yarp::robotinterface::impl::XMLReaderFileV3::Private
{
public:
    explicit Private(XMLReaderFileV3* parent);
    virtual ~Private();

    yarp::robotinterface::XMLReaderResult readRobotFromFile(const std::string& fileName);
    yarp::robotinterface::XMLReaderResult readRobotFromString(const std::string& xmlString);
    yarp::robotinterface::XMLReaderResult readRobotTag(TiXmlElement* robotElem);

    yarp::robotinterface::DeviceList readDevices(TiXmlElement* devicesElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::Device readDeviceTag(TiXmlElement* deviceElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::DeviceList readDevicesTag(TiXmlElement* devicesElem, yarp::robotinterface::XMLReaderResult& result);

    yarp::robotinterface::ParamList readParams(TiXmlElement* paramsElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::Param readParamTag(TiXmlElement* paramElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::Param readGroupTag(TiXmlElement* groupElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::ParamList readParamListTag(TiXmlElement* paramListElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::ParamList readSubDeviceTag(TiXmlElement* subDeviceElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::ParamList readParamsTag(TiXmlElement* paramsElem, yarp::robotinterface::XMLReaderResult& result);

    yarp::robotinterface::ActionList readActions(TiXmlElement* actionsElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::Action readActionTag(TiXmlElement* actionElem, yarp::robotinterface::XMLReaderResult& result);
    yarp::robotinterface::ActionList readActionsTag(TiXmlElement* actionsElem, yarp::robotinterface::XMLReaderResult& result);

    bool PerformInclusions(TiXmlNode* pParent, const std::string& parent_fileName, const std::string& current_path);

    XMLReaderFileV3* const parent;

#ifdef USE_DTD
    RobotInterfaceDTD dtd;
#endif

    bool verbose_output;
    const yarp::os::Searchable* config;
    std::string curr_filename;
    unsigned int minorVersion;
    unsigned int majorVersion;
};


yarp::robotinterface::impl::XMLReaderFileV3::Private::Private(XMLReaderFileV3* p) :
        parent(p),
        minorVersion(0),
        majorVersion(0)
{
    verbose_output = false;
}

yarp::robotinterface::impl::XMLReaderFileV3::Private::~Private() = default;

yarp::robotinterface::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV3::Private::readRobotFromFile(const std::string& fileName)
{
    yDebug() << "Reading file" << fileName.c_str();
    auto* doc = new TiXmlDocument(fileName.c_str());
    if (!doc->LoadFile()) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
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

    std::string current_path;
    current_path = fileName.substr(0, fileName.find_last_of("\\/"));
    std::string current_filename;
    std::string log_filename;
    current_filename = fileName.substr(fileName.find_last_of("\\/") + 1);
    log_filename = current_filename.substr(0, current_filename.find(".xml"));
    log_filename += "_preprocessor_log.xml";
    double start_time = yarp::os::Time::now();
    PerformInclusions(doc->RootElement(), current_filename, current_path);
    double end_time = yarp::os::Time::now();
    std::string full_log_withpath = current_path + std::string("\\") + log_filename;
    std::replace(full_log_withpath.begin(), full_log_withpath.end(), '\\', '/');
    yDebug() << "Preprocessor complete in: " << end_time - start_time << "s";
    if (verbose_output) {
        yDebug() << "Preprocessor output stored in: " << full_log_withpath;
        doc->SaveFile(full_log_withpath);
    }
    yarp::robotinterface::XMLReaderResult result = readRobotTag(doc->RootElement());
    delete doc;

    // yDebug() << robot;

    return result;
}

yarp::robotinterface::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV3::Private::readRobotFromString(const std::string& xmlString)
{
    curr_filename = " XML runtime string ";
    auto* doc = new TiXmlDocument();
    if (!doc->Parse(xmlString.c_str())) {
        SYNTAX_ERROR(doc->ErrorRow()) << doc->ErrorDesc();
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
    }

    if (!doc->RootElement()) {
        SYNTAX_ERROR(doc->Row()) << "No root element.";
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
    }

    yarp::robotinterface::XMLReaderResult result = readRobotTag(doc->RootElement());
    delete doc;

    return result;
}

bool yarp::robotinterface::impl::XMLReaderFileV3::Private::PerformInclusions(TiXmlNode* pParent, const std::string& parent_fileName, const std::string& current_path)
{
loop_start: //goto label
    for (TiXmlElement* childElem = pParent->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
#ifdef DEBUG_PARSER
        std::string a;
        if (childElem->FirstAttribute())
            a = childElem->FirstAttribute()->Value();
        yDebug() << "Parsing" << childElem->Value() << a;
#endif
        std::string elemString = childElem->ValueStr();
        if (elemString == "file") {
            yFatal() << "'file' attribute is forbidden in yarprobotinterface DTD format 3.0. Error found in " << parent_fileName;
            return false;
        }

        if (elemString == "xi:include") {
            std::string href_filename;
            std::string included_filename;
            std::string included_path;
            if (childElem->QueryStringAttribute("href", &href_filename) == TIXML_SUCCESS) {
                included_path = std::string(current_path).append("\\").append(href_filename.substr(0, href_filename.find_last_of("\\/")));
                included_filename = href_filename.substr(href_filename.find_last_of("\\/") + 1);
                std::string full_path_file = std::string(included_path).append("\\").append(included_filename);
                TiXmlDocument included_file;

                std::replace(full_path_file.begin(), full_path_file.end(), '\\', '/');
                if (included_file.LoadFile(full_path_file)) {
                    PerformInclusions(included_file.RootElement(), included_filename, included_path);
                    //included_file.RootElement()->SetAttribute("xml:base", href_filename); //not yet implemented
                    included_file.RootElement()->RemoveAttribute("xmlns:xi");
                    if (pParent->ReplaceChild(childElem, *included_file.FirstChildElement())) {
                        //the replace operation invalidates the iterator, hence we need to restart the parsing of this level
                        goto loop_start;
                    } else {
                        //fatal error
                        yFatal() << "Failed to include: " << included_filename << " in: " << parent_fileName;
                        return false;
                    }
                } else {
                    //fatal error
                    yError() << included_file.ErrorDesc() << " file" << full_path_file << "included by " << parent_fileName << "at line" << childElem->Row();
                    yFatal() << "In file:" << included_filename << " included by: " << parent_fileName << " at line: " << childElem->Row();
                    return false;
                }
            } else {
                //fatal error
                yFatal() << "Syntax error in: " << parent_fileName << " while searching for href attribute";
                return false;
            }
        }
        PerformInclusions(childElem, parent_fileName, current_path);
    }
    return true;
}

yarp::robotinterface::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV3::Private::readRobotTag(TiXmlElement* robotElem)
{
    yarp::robotinterface::XMLReaderResult result;
    result.parsingIsSuccessful = true;

    if (robotElem->ValueStr() != "robot") {
        SYNTAX_ERROR(robotElem->Row()) << "Root element should be \"robot\". Found" << robotElem->ValueStr();
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
    }

    if (robotElem->QueryStringAttribute("name", &result.robot.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(robotElem->Row()) << R"("robot" element should contain the "name" attribute)";
        return yarp::robotinterface::XMLReaderResult::ParsingFailed();
    }

#if TINYXML_UNSIGNED_INT_BUG
    if (robotElem->QueryUnsignedAttribute("build", &result.robot.build()) != TIXML_SUCCESS) {
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
        std::string elemString = childElem->ValueStr();
        if (elemString == "device" || elemString == "devices") {
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


yarp::robotinterface::DeviceList yarp::robotinterface::impl::XMLReaderFileV3::Private::readDevices(TiXmlElement* devicesElem,
                                                                                                                 yarp::robotinterface::XMLReaderResult& result)
{
    const std::string& valueStr = devicesElem->ValueStr();

    if (valueStr == "device") {
        // yDebug() << valueStr;
        yarp::robotinterface::DeviceList deviceList;
        deviceList.push_back(readDeviceTag(devicesElem, result));
        return deviceList;
    }
    if (valueStr == "devices") {
        // "devices"
        return readDevicesTag(devicesElem, result);
    }

    SYNTAX_ERROR(devicesElem->Row()) << R"(Expected "device" or "devices". Found)" << valueStr;
    result.parsingIsSuccessful = false;
    return yarp::robotinterface::DeviceList();
}

yarp::robotinterface::Device yarp::robotinterface::impl::XMLReaderFileV3::Private::readDeviceTag(TiXmlElement* deviceElem,
                                                                                                               yarp::robotinterface::XMLReaderResult& result)
{
    const std::string& valueStr = deviceElem->ValueStr();

    if (valueStr != "device") {
        SYNTAX_ERROR(deviceElem->Row()) << "Expected \"device\". Found" << valueStr;
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Device();
    }

    yarp::robotinterface::Device device;

    if (deviceElem->QueryStringAttribute("name", &device.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
        return device;
    }

    // yDebug() << "Found device [" << device.name() << "]";

    if (deviceElem->QueryStringAttribute("type", &device.type()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(deviceElem->Row()) << R"("device" element should contain the "type" attribute)";
        result.parsingIsSuccessful = false;
        return device;
    }

    device.params().push_back(yarp::robotinterface::Param("robotName", result.robot.portprefix()));

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

yarp::robotinterface::DeviceList yarp::robotinterface::impl::XMLReaderFileV3::Private::readDevicesTag(TiXmlElement* devicesElem,
                                                                                                                    yarp::robotinterface::XMLReaderResult& result)
{
    //const std::string &valueStr = devicesElem->ValueStr();

    yarp::robotinterface::DeviceList devices;
    for (TiXmlElement* childElem = devicesElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childDevice : readDevices(childElem, result)) {
            devices.push_back(childDevice);
        }
    }

    return devices;
}

yarp::robotinterface::ParamList yarp::robotinterface::impl::XMLReaderFileV3::Private::readParams(TiXmlElement* paramsElem,
                                                                                                               yarp::robotinterface::XMLReaderResult& result)
{
    const std::string& valueStr = paramsElem->ValueStr();

    if (valueStr == "param") {
        yarp::robotinterface::ParamList params;
        params.push_back(readParamTag(paramsElem, result));
        return params;
    }
    if (valueStr == "group") {
        yarp::robotinterface::ParamList params;
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
    result.parsingIsSuccessful = false;
    return yarp::robotinterface::ParamList();
}


yarp::robotinterface::Param yarp::robotinterface::impl::XMLReaderFileV3::Private::readParamTag(TiXmlElement* paramElem,
                                                                                                             yarp::robotinterface::XMLReaderResult& result)
{
    if (paramElem->ValueStr() != "param") {
        SYNTAX_ERROR(paramElem->Row()) << "Expected \"param\". Found" << paramElem->ValueStr();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
    }

    yarp::robotinterface::Param param;

    if (paramElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
    }

    // yDebug() << "Found param [" << param.name() << "]";

    const char* valueText = paramElem->GetText();
    if (!valueText) {
        SYNTAX_ERROR(paramElem->Row()) << R"("param" element should have a value [ "name" = )" << param.name() << "]";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
    }

    std::string extern_name;
    if (paramElem->QueryStringAttribute("extern-name", &extern_name) == TIXML_SUCCESS && config && config->check(extern_name)) {
        // FIXME Check DTD >= 3.1
        param.value() = config->find(extern_name).asString();
    } else {
        param.value() = valueText;
    }

    // yDebug() << param;
    return param;
}

yarp::robotinterface::Param yarp::robotinterface::impl::XMLReaderFileV3::Private::readGroupTag(TiXmlElement* groupElem,
                                                                                                             yarp::robotinterface::XMLReaderResult& result)
{
    if (groupElem->ValueStr() != "group") {
        SYNTAX_ERROR(groupElem->Row()) << "Expected \"group\". Found" << groupElem->ValueStr();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
    }

    yarp::robotinterface::Param group(true);

    if (groupElem->QueryStringAttribute("name", &group.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(groupElem->Row()) << R"("group" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
    }

    // yDebug() << "Found group [" << group.name() << "]";

    yarp::robotinterface::ParamList params;
    for (TiXmlElement* childElem = groupElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childParam : readParams(childElem, result)) {
            params.push_back(childParam);
        }
    }
    if (params.empty()) {
        SYNTAX_ERROR(groupElem->Row()) << "\"group\" cannot be empty";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Param();
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

yarp::robotinterface::ParamList yarp::robotinterface::impl::XMLReaderFileV3::Private::readParamListTag(TiXmlElement* paramListElem,
                                                                                                                     yarp::robotinterface::XMLReaderResult& result)
{
    if (paramListElem->ValueStr() != "paramlist") {
        SYNTAX_ERROR(paramListElem->Row()) << "Expected \"paramlist\". Found" << paramListElem->ValueStr();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::ParamList();
    }

    yarp::robotinterface::ParamList params;
    yarp::robotinterface::Param mainparam;

    if (paramListElem->QueryStringAttribute("name", &mainparam.name()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(paramListElem->Row()) << R"("paramlist" element should contain the "name" attribute)";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::ParamList();
    }

    params.push_back(mainparam);

    // yDebug() << "Found paramlist [" << params.at(0).name() << "]";

    for (TiXmlElement* childElem = paramListElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr() != "elem") {
            SYNTAX_ERROR(childElem->Row()) << "Expected \"elem\". Found" << childElem->ValueStr();
            result.parsingIsSuccessful = false;
            return yarp::robotinterface::ParamList();
        }

        yarp::robotinterface::Param childParam;

        if (childElem->QueryStringAttribute("name", &childParam.name()) != TIXML_SUCCESS) {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should contain the "name" attribute)";
            result.parsingIsSuccessful = false;
            return yarp::robotinterface::ParamList();
        }

        const char* valueText = childElem->GetText();
        if (!valueText) {
            SYNTAX_ERROR(childElem->Row()) << R"("elem" element should have a value [ "name" = )" << childParam.name() << "]";
            result.parsingIsSuccessful = false;
            return yarp::robotinterface::ParamList();
        }
        childParam.value() = valueText;

        params.push_back(childParam);
    }

    if (params.empty()) {
        SYNTAX_ERROR(paramListElem->Row()) << "\"paramlist\" cannot be empty";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::ParamList();
    }

    // +1 skips the first element, that is the main param
    for (auto it = params.begin() + 1; it != params.end(); ++it) {
        yarp::robotinterface::Param& param = *it;
        params.at(0).value() += (params.at(0).value().empty() ? "(" : " ") + param.name();
    }
    params.at(0).value() += ")";

    // yDebug() << params;
    return params;
}

yarp::robotinterface::ParamList yarp::robotinterface::impl::XMLReaderFileV3::Private::readSubDeviceTag(TiXmlElement* subDeviceElem,
                                                                                                                     yarp::robotinterface::XMLReaderResult& result)
{
    if (subDeviceElem->ValueStr() != "subdevice") {
        SYNTAX_ERROR(subDeviceElem->Row()) << "Expected \"subdevice\". Found" << subDeviceElem->ValueStr();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::ParamList();
    }

    yarp::robotinterface::ParamList params;

    //FIXME    yarp::robotinterface::Param featIdParam;
    yarp::robotinterface::Param subDeviceParam;

    //FIXME    featIdParam.name() = "FeatId";
    subDeviceParam.name() = "subdevice";

    //FIXME    if (subDeviceElem->QueryStringAttribute("name", &featIdParam.value()) != TIXML_SUCCESS) {
    //        SYNTAX_ERROR(subDeviceElem->Row()) << "\"subdevice\" element should contain the \"name\" attribute";
    //    }

    if (subDeviceElem->QueryStringAttribute("type", &subDeviceParam.value()) != TIXML_SUCCESS) {
        SYNTAX_ERROR(subDeviceElem->Row()) << R"("subdevice" element should contain the "type" attribute)";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::ParamList();
    }

    //FIXME    params.push_back(featIdParam);
    params.push_back(subDeviceParam);

    // yDebug() << "Found subdevice [" << params.at(0).value() << "]";

    for (TiXmlElement* childElem = subDeviceElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto& childParam : readParams(childElem, result)) {
            params.push_back(yarp::robotinterface::Param(childParam.name(), childParam.value()));
        }
    }

    // yDebug() << params;
    return params;
}

yarp::robotinterface::ParamList yarp::robotinterface::impl::XMLReaderFileV3::Private::readParamsTag(TiXmlElement* paramsElem,
                                                                                                                  yarp::robotinterface::XMLReaderResult& result)
{
    //const std::string &valueStr = paramsElem->ValueStr();

    yarp::robotinterface::ParamList params;
    for (TiXmlElement* childElem = paramsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto & childParam : readParams(childElem, result)) {
            params.push_back(childParam);
        }
    }

    return params;
}

yarp::robotinterface::ActionList yarp::robotinterface::impl::XMLReaderFileV3::Private::readActions(TiXmlElement* actionsElem,
                                                                                                                 yarp::robotinterface::XMLReaderResult& result)
{
    const std::string& valueStr = actionsElem->ValueStr();

    if (valueStr != "action" && valueStr != "actions") {
        SYNTAX_ERROR(actionsElem->Row()) << R"(Expected "action" or "actions". Found)" << valueStr;
    }

    if (valueStr == "action") {
        yarp::robotinterface::ActionList actionList;
        actionList.push_back(readActionTag(actionsElem, result));
        return actionList;
    }
    // "actions"
    return readActionsTag(actionsElem, result);
}

yarp::robotinterface::Action yarp::robotinterface::impl::XMLReaderFileV3::Private::readActionTag(TiXmlElement* actionElem,
                                                                                                               yarp::robotinterface::XMLReaderResult& result)
{
    if (actionElem->ValueStr() != "action") {
        SYNTAX_ERROR(actionElem->Row()) << "Expected \"action\". Found" << actionElem->ValueStr();
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Action();
    }

    yarp::robotinterface::Action action;

    if (actionElem->QueryValueAttribute<yarp::robotinterface::ActionPhase>("phase", &action.phase()) != TIXML_SUCCESS || action.phase() == yarp::robotinterface::ActionPhaseUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "phase" attribute [startup|interrupt{1,2,3}|shutdown])";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Action();
    }


    if (actionElem->QueryValueAttribute<yarp::robotinterface::ActionType>("type", &action.type()) != TIXML_SUCCESS || action.type() == yarp::robotinterface::ActionTypeUnknown) {
        SYNTAX_ERROR(actionElem->Row()) << R"("action" element should contain the "type" attribute [configure|calibrate|attach|abort|detach|park|custom])";
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Action();
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
        result.parsingIsSuccessful = false;
        return yarp::robotinterface::Action();
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

yarp::robotinterface::ActionList yarp::robotinterface::impl::XMLReaderFileV3::Private::readActionsTag(TiXmlElement* actionsElem,
                                                                                                                    yarp::robotinterface::XMLReaderResult& result)
{
    //const std::string &valueStr = actionsElem->ValueStr();

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

    yarp::robotinterface::ActionList actions;
    for (TiXmlElement* childElem = actionsElem->FirstChildElement(); childElem != nullptr; childElem = childElem->NextSiblingElement()) {
        for (const auto & childAction : readActions(childElem, result)) {
            actions.push_back(childAction);
        }
    }

    return actions;
}


yarp::robotinterface::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV3::getRobotFromFile(const std::string& filename,
                                                                                                                  const yarp::os::Searchable& config,
                                                                                                                  bool verb)
{
    mPriv->config = &config;
    mPriv->verbose_output = verb;
    auto ret = mPriv->readRobotFromFile(filename);
    mPriv->config = nullptr;
    return ret;
}

yarp::robotinterface::XMLReaderResult yarp::robotinterface::impl::XMLReaderFileV3::getRobotFromString(const std::string& xmlString,
                                                                                                                    const yarp::os::Searchable& config,
                                                                                                                    bool verb)
{
    mPriv->config = &config;
    mPriv->verbose_output = verb;
    auto ret = mPriv->readRobotFromString(xmlString);
    mPriv->config = nullptr;
    return ret;
}


yarp::robotinterface::impl::XMLReaderFileV3::XMLReaderFileV3() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::impl::XMLReaderFileV3::~XMLReaderFileV3()
{
    delete mPriv;
}
