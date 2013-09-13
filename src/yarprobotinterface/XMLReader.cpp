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

#include <yarp/os/Property.h>

#define SYNTAX_ERROR(line) yFatal() << "Syntax error while loading" << curr_filename << "at line" << line << "."
#define SYNTAX_WARNING(line) yWarning() << "Syntax error while loading" << curr_filename << "at line" << line << "."


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
    Robot robot;

    std::string curr_filename;
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

    readRobotTag(doc->RootElement());
    delete doc;

    yDebug() << robot;

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

#if 0
    // BUG in TinyXML, see
    // https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
    // When this bug is fixed upstream we can enable this
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
        SYNTAX_ERROR(robotElem->Row()) << "\"robot\" element should contain the \"portprefix\" attribute";
    }

    yDebug() << "Found robot [" << robot.name() << "] build [" << robot.build() << "] portprefix [" << robot.portprefix() << "]";

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
        yDebug() << valueStr;
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

    yDebug() << "Found device [" << device.name() << "]";

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

    yDebug() << device;
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
        yDebug() << "Found devices file [" << filename << "]";
#ifdef WIN32
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readDevicesFile(filename);
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

    yDebug() << "Found param [" << param.name() << "]";

    const char *valueText = paramElem->GetText();
    if (!valueText) {
        SYNTAX_ERROR(paramElem->Row()) << "\"param\" element should have a value [ \"name\" = " << param.name() << "]";
    }
    param.value() = valueText;

    yDebug() << param;
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

    yDebug() << "Found group [" << group.name() << "]";

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

    yDebug() << "Found paramlist [" << params.at(0).name() << "]";

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

    yDebug() << params;
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

    yDebug() << "Found subdevice [" << params.at(0).value() << "]";

    for (TiXmlElement* childElem = subDeviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList childParams = readParams(childElem);
        for (ParamList::const_iterator it = childParams.begin(); it != childParams.end(); ++it) {
            params.push_back(Param(it->name(), it->value()));
        }
    }

    yDebug() << params;
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
        yDebug() << "Found params file [" << filename << "]";
#ifdef WIN32
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readParamsFile(filename);
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

    yDebug() << "Found action [ ]";

#if 0
    // BUG in TinyXML, see
    // https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
    // When this bug is fixed upstream we can enable this
    if (actionElem->QueryUnsignedAttribute("level", &action.level()) != TIXML_SUCCESS) {
        yFatal() << SYNTAX_ERROR << "\"action\" element should contain the \"level\" attribute";
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

    yDebug() << action;
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
        yDebug() << "Found actions file [" << filename << "]";
#ifdef WIN32
        filename = path + "\\" + filename;
#else // WIN32
        filename = path + "/" + filename;
#endif //WIN32
        return readActionsFile(filename);
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
