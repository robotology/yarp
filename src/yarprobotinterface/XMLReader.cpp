/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "XMLReader.h"
#include "RobotInterface.h"
#include "Debug.h"

#include <tinyxml.h>
#include <string>
#include <vector>
#include <sstream>

#include <yarp/os/Property.h>

#define SYNTAX_ERROR(filename, line) "Syntax error while loading" << filename << "at line" << line << "."


class RobotInterface::XMLReader::Private
{
public:
    Private(XMLReader *parent);
    ~Private();

    RobotInterface::Robot& readFile(const std::string &filename);
    RobotInterface::Robot readRobot(TiXmlElement *robotElem);
    RobotInterface::Device readDevice(TiXmlElement *deviceElem);
    RobotInterface::ParamList readParams(TiXmlElement *paramsElem);
    RobotInterface::Param readParam(TiXmlElement *paramElem);
    RobotInterface::Param readGroup(TiXmlElement *groupElem);
    RobotInterface::ParamList readParamList(TiXmlElement *paramListElem);
    RobotInterface::ParamList readSubDevice(TiXmlElement *subDeviceElem);
    RobotInterface::ParamList readSubFile(TiXmlElement *subFileElem);
    RobotInterface::Action readAction(TiXmlElement *actionElem);

    XMLReader * const parent;
    std::string filename;
    Robot robot;
};


RobotInterface::XMLReader::Private::Private(XMLReader *parent) :
    parent(parent)
{
}

RobotInterface::XMLReader::Private::~Private()
{
}

RobotInterface::Robot& RobotInterface::XMLReader::Private::readFile(const std::string &filename)
{
    this->filename = filename;
    TiXmlDocument *doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        fatal() << SYNTAX_ERROR(filename, doc->ErrorRow()) << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        fatal() << SYNTAX_ERROR(filename, doc->Row()) << "No root element.";
    }

    robot = readRobot(doc->RootElement());
    debug() << "  " << robot;

    delete doc;
    return robot;
}


RobotInterface::Robot RobotInterface::XMLReader::Private::readRobot(TiXmlElement *robotElem)
{
    if (robotElem->ValueStr().compare("robot") != 0) {
        fatal() << SYNTAX_ERROR(filename, robotElem->Row()) << "Root element should be \"robot\", found" << robotElem->Value();
    }

    if (robotElem->QueryStringAttribute("name", &robot.name()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, robotElem->Row()) << "\"robot\" element should contain the \"name\" attribute";
    }

    debug() << "Found robot [" << robot.name() << "]";

    for (TiXmlElement* deviceElem = robotElem->FirstChildElement(); deviceElem != 0; deviceElem = deviceElem->NextSiblingElement()) {
        robot.devices().push_back(readDevice(deviceElem));
    }

    return robot;
}

RobotInterface::Device RobotInterface::XMLReader::Private::readDevice(TiXmlElement *deviceElem)
{
    if (deviceElem->ValueStr().compare("device") != 0) {
        fatal() << SYNTAX_ERROR(filename, deviceElem->Row()) << "Expected \"device\", found" << deviceElem->Value();
    }

    Device device;

    if (deviceElem->QueryStringAttribute("name", &device.name()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, deviceElem->Row()) << "\"device\" element should contain the \"name\" attribute";
    }

    debug() << "    Found device [" << device.name() << "]";

    if (deviceElem->QueryStringAttribute("type", &device.type()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, deviceElem->Row()) << "\"device\" element should contain the \"type\" attribute";
    }

    //TODO Needed?
    device.params().push_back(Param("robotName", robot.name().c_str()));

    for (TiXmlElement* childElem = deviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr().compare("action") == 0) {
            device.actions().push_back(readAction(childElem));
        } else {
            ParamList paramsElem = readParams(childElem);
            for (ParamList::const_iterator it = paramsElem.begin(); it != paramsElem.end(); it++) {
                device.params().push_back(*it);
            }
        }
    }

    debug() << "     " << device;
    return device;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readParams(TiXmlElement* paramsElem)
{
    ParamList params;
    const std::string &valueStr = paramsElem->ValueStr();

    if (valueStr.compare("param") == 0) {
        params.push_back(readParam(paramsElem));
    } else if (valueStr.compare("group") == 0) {
        params.push_back(readGroup(paramsElem));
    } else if (valueStr.compare("paramlist") == 0) {
        ParamList paramList = readParamList(paramsElem);
        for (ParamList::const_iterator it = paramList.begin(); it != paramList.end(); it++) {
            params.push_back(*it);
        }
    } else if (valueStr.compare("subdevice") == 0) {
        ParamList subDevice = readSubDevice(paramsElem);
        for (ParamList::const_iterator it = subDevice.begin(); it != subDevice.end(); it++) {
            params.push_back(*it);
        }
    } else if (valueStr.compare("file") == 0) {
        ParamList subFile = readSubFile(paramsElem);
        for (ParamList::const_iterator it = subFile.begin(); it != subFile.end(); it++) {
            params.push_back(*it);
        }
    } else {
        fatal() << SYNTAX_ERROR(filename, paramsElem->Row()) << "\"device\" element contain an unknown tag" << valueStr;
    }

    return params;
}

RobotInterface::Param RobotInterface::XMLReader::Private::readParam(TiXmlElement *paramElem)
{
    if (paramElem->ValueStr().compare("param") != 0) {
        fatal() << SYNTAX_ERROR(filename, paramElem->Row()) << "Expected \"param\", found" << paramElem->Value();
    }

    Param param;

    if (paramElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, paramElem->Row()) << "\"param\" element should contain the \"name\" attribute";
    }

    debug() << "        Found param [" << param.name() << "]";

    const char *valueText = paramElem->GetText();
    if (!valueText) {
        fatal() << SYNTAX_ERROR(filename, paramElem->Row()) << "\"param\" element should have a value [ \"name\" = " << param.name() << "]";
    }
    param.value() = valueText;

    debug() << "         " << param;
    return param;
}

RobotInterface::Param RobotInterface::XMLReader::Private::readGroup(TiXmlElement* groupElem)
{
    if (groupElem->ValueStr().compare("group") != 0) {
        fatal() << SYNTAX_ERROR(filename, groupElem->Row()) << "Expected \"group\", found" << groupElem->Value();
    }

    Param group;

    if (groupElem->QueryStringAttribute("name", &group.name()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, groupElem->Row()) << "\"group\" element should contain the \"name\" attribute";
    }

    debug() << "        Found group [" << group.name() << "]";

    ParamList groupParamList;
    for (TiXmlElement* childElem = groupElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList paramsElem = readParams(childElem);
        for (ParamList::const_iterator it = paramsElem.begin(); it != paramsElem.end(); it++) {
            groupParamList.push_back(*it);
        }
    }
    if (groupParamList.empty()) {
        fatal() << SYNTAX_ERROR(filename, groupElem->Row()) << "\"group\" cannot be empty";
    }

    std::string groupString;
    for (ParamList::iterator it = groupParamList.begin(); it != groupParamList.end(); it++) {
        if (!groupString.empty()) {
            groupString += " ";
        }
        groupString += "(" + it->name() + " " + it->value() + ")";
    }

    group.value() = groupString;

    return group;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readParamList(TiXmlElement* paramListElem)
{
    if (paramListElem->ValueStr().compare("paramlist") != 0) {
        fatal() << SYNTAX_ERROR(filename, paramListElem->Row()) << "Expected \"paramlist\", found" << paramListElem->Value();
    }

    ParamList paramList;
    Param mainparam;

    if (paramListElem->QueryStringAttribute("name", &mainparam.name()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, paramListElem->Row()) << "\"paramlist\" element should contain the \"name\" attribute";
    }

    paramList.push_back(mainparam);

    debug() << "        Found paramlist [" << paramList.at(0).name() << "]";

    for (TiXmlElement* childElem = paramListElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (childElem->ValueStr().compare("elem") != 0) {
            fatal() << SYNTAX_ERROR(filename, childElem->Row()) << "Expected \"elem\", found" << childElem->Value();
        }

        Param param;

        if (childElem->QueryStringAttribute("name", &param.name()) != TIXML_SUCCESS) {
            fatal() << SYNTAX_ERROR(filename, childElem->Row()) << "\"elem\" element should contain the \"name\" attribute";
        }

        const char *valueText = childElem->GetText();
        if (!valueText) {
            fatal() << SYNTAX_ERROR(filename, childElem->Row()) << "\"elem\" element should have a value [ \"name\" = " << param.name() << "]";
        }
        param.value() = valueText;

        paramList.push_back(param);
    }

    if (paramList.empty()) {
        fatal() << SYNTAX_ERROR(filename, paramListElem->Row()) << "\"paramlist\" cannot be empty";
    }

    // +1 skips the first element, that is the main param
    for (ParamList::iterator it = paramList.begin() + 1; it != paramList.end(); it++) {
        Param &param = *it;
        paramList.at(0).value() += (paramList.at(0).value().empty() ? "(" : " ") + param.name();
    }
    paramList.at(0).value() += ")";

    debug() << "         " << paramList;
    return paramList;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readSubDevice(TiXmlElement *subDeviceElem)
{
    if (subDeviceElem->ValueStr().compare("subdevice") != 0) {
        fatal() << SYNTAX_ERROR(filename, subDeviceElem->Row()) << "Expected \"subdevice\", found" << subDeviceElem->Value();
    }

    ParamList subDevice;

//FIXME    Param featIdParam;
    Param subDeviceParam;

//FIXME    featIdParam.name() = "FeatId";
    subDeviceParam.name() = "subdevice";

//FIXME    if (subDeviceElem->QueryStringAttribute("name", &featIdParam.value()) != TIXML_SUCCESS) {
//        fatal() << SYNTAX_ERROR(filename, subDeviceElem->Row()) << "\"subdevice\" element should contain the \"name\" attribute";
//    }

    if (subDeviceElem->QueryStringAttribute("type", &subDeviceParam.value()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR(filename, subDeviceElem->Row()) << "\"subdevice\" element should contain the \"type\" attribute";
    }

//FIXME    subDevice.push_back(featIdParam);
    subDevice.push_back(subDeviceParam);

    debug() << "        Found subdevice [" << subDevice.at(0).value() << "]";

    for (TiXmlElement* childElem = subDeviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        ParamList paramsElem = readParams(childElem);
        for (ParamList::const_iterator it = paramsElem.begin(); it != paramsElem.end(); it++) {
            subDevice.push_back(Param(it->name(), it->value()));
        }
    }

    debug() << "         " << subDevice;
    return subDevice;
}

RobotInterface::ParamList RobotInterface::XMLReader::Private::readSubFile(TiXmlElement* subFileElem)
{
    if (subFileElem->ValueStr().compare("file") != 0) {
        fatal() << SYNTAX_ERROR(filename, subFileElem->Row()) << "Expected \"file\", found" << subFileElem->Value();
    }

    ParamList paramList;

    warning() << "\"file\" tag not yet handled";

    return paramList;

}

RobotInterface::Action RobotInterface::XMLReader::Private::readAction(TiXmlElement* actionElem)
{
    if (actionElem->ValueStr().compare("action") != 0) {
        fatal() << SYNTAX_ERROR(filename, actionElem->Row()) << "Expected \"action\", found" << actionElem->Value();
    }

    Action action;

    if (actionElem->QueryValueAttribute<ActionPhase>("phase", &action.phase()) != TIXML_SUCCESS || action.phase() == ActionPhaseUnknown) {
        fatal() << SYNTAX_ERROR(filename, actionElem->Row()) << "\"action\" element should contain the \"phase\" attribute [startup|interrupt|shutdown]";
    }


    if (actionElem->QueryValueAttribute<ActionType>("type", &action.type()) != TIXML_SUCCESS || action.type() == ActionTypeUnknown) {
        fatal() << SYNTAX_ERROR(filename, actionElem->Row()) << "\"action\" element should contain the \"type\" attribute [FIXME: list of accepted values]"; // FIXME
    }

    debug() << "        Found action [ ]";

#if 0
    // BUG in TinyXML, see
    // https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
    // When this bug is fixed upstream we can enable this
    if (actionElem->QueryUnsignedAttribute("level", &action.level()) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"level\" attribute";
    }
#else
    int tmp;
    if (actionElem->QueryIntAttribute("level", &tmp) != TIXML_SUCCESS || tmp < 0) {
        fatal() << SYNTAX_ERROR(filename, actionElem->Row()) << "\"param\" element should contain the \"level\" attribute [unsigned int]";
    }
    action.level() = (unsigned)tmp;
#endif


    debug() << "         " << action;
    return action;
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
    return mPriv->readFile(filename);
}
