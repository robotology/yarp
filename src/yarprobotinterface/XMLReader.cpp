/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "XMLReader.h"
#include "Debug.h"

#include <tinyxml.h>
#include <string>
#include <vector>
#include <sstream>

#define SYNTAX_ERROR "Syntax error while loading" << filename << ". "


namespace {

template <typename T>
inline std::ostringstream& operator<<(std::ostringstream &oss, const std::vector<T> &t)
{
    for (typename std::vector<T>::const_iterator it = t.begin(); it != t.end(); it++) {
        const T &p = *it;
        oss << p;
    }
    return oss;
}

template <typename T>
inline RobotInterface::Debug& operator<<(RobotInterface::Debug &dbg, const std::vector<T> &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}


//BEGIN Param

struct Param
{
    std::string name;
    std::string value;
};
typedef std::vector<Param> ParamList;

inline std::ostringstream& operator<<(std::ostringstream &oss, const Param &t)
{
    oss << "(\"" << t.name << "\" = \"" << t.value << "\")";
    return oss;
}

inline RobotInterface::Debug& operator<<(RobotInterface::Debug &dbg, const Param &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

//END Param

//BEGIN Action

enum ActionPhase
{
    ActionPhaseUnknown = 0,
    ActionPhaseStartup,
    ActionPhaseShutdown,

    ActionPhaseReserved = 0xFF
};

ActionPhase StringToActionPhase(const std::string &phase)
{
    if (!phase.compare("startup")) {
        return ActionPhaseStartup;
    } else if (!phase.compare("shutdown")) {
        return ActionPhaseShutdown;
    }
    return ActionPhaseReserved;
}

std::string ActionPhaseToString(ActionPhase actionphase)
{
    switch (actionphase) {
    case ActionPhaseStartup:
        return "startup";
    case ActionPhaseShutdown:
        return "shutdown";
    case ActionPhaseUnknown:
    default:
        return std::string();
    }
}

// This is required to use TiXmlElement::QueryValueAttribute<ActionPhase>
inline void operator>>(const std::stringstream &sstream, ActionPhase &actionphase) {
    actionphase = StringToActionPhase(sstream.str());
}

enum ActionType
{
    ActionTypeUnknown = 0,

    ActionTypeConfigure,
    ActionTypeCalibrate,
    ActionTypeAttach,
    ActionTypeDetach,
    ActionTypePark,

    ActionTypeCustom = 0xFF
};

ActionType StringToActionType(const std::string &type)
{
    if (!type.compare("configure")) {
        return ActionTypeConfigure;
    } else if (!type.compare("calibrate")) {
        return ActionTypeCalibrate;
    } else if (!type.compare("attach")) {
        return ActionTypeAttach;
    } else if (!type.compare("detach")) {
        return ActionTypeDetach;
    } else if (!type.compare("park")) {
        return ActionTypePark;
    } else if (!type.compare("custom")) {
        return ActionTypeCustom;
    } else {
        return ActionTypeUnknown;
    }
}

std::string ActionTypeToString(ActionType actiontype)
{
    switch (actiontype) {
    case ActionTypeConfigure:
        return "configure";
    case ActionTypeCalibrate:
        return "calibrate";
    case ActionTypeAttach:
        return "attach";
    case ActionTypeDetach:
        return "detach";
    case ActionTypePark:
        return "park";
    case ActionTypeCustom:
        return "custom";
    case ActionTypeUnknown:
    default:
        return std::string();
    }
}

// This is required to use TiXmlElement::QueryValueAttribute<ActionType>
inline void operator>>(const std::stringstream &sstream, ActionType &actiontype) {
    actiontype = StringToActionType(sstream.str());
}

struct Action
{
    ActionPhase phase;
    ActionType type;
    unsigned int level;
};
typedef std::vector<Action> ActionList;

inline std::ostringstream& operator<<(std::ostringstream &oss, const Action &t)
{
    oss << "(\"" << ActionPhaseToString(t.phase) << ":" << ActionTypeToString(t.type) << ":" << t.level << "\")";
    return oss;
}

inline RobotInterface::Debug& operator<<(RobotInterface::Debug &dbg, const Action &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

//END Action

//BEGIN Device
struct Device
{
    std::string name;
    std::string type;
    ParamList params;
    ActionList actions;
};

inline std::ostringstream& operator<<(std::ostringstream &oss, const Device &t)
{
    oss << "(name = \"" << t.name << "\", type = \"" << t.type << "\"";
    if (!t.params.empty()) {
        oss << ", params = [";
        oss << t.params;
        oss << "]";
    }
    if (!t.actions.empty()) {
        oss << ", actions = [";
        oss << t.actions;
        oss << "]";
    }
    oss << ")";
    return oss;
}

inline RobotInterface::Debug& operator<<(RobotInterface::Debug &dbg, const Device &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

//END Device

//BEGIN Robot
struct Robot
{
    std::string name;
    std::vector<Device> devices;
};

inline RobotInterface::Debug& operator<<(RobotInterface::Debug &dbg, const Robot &t)
{
    std::ostringstream oss;
    oss << "(name = \"" << t.name << "\"";
    if (!t.devices.empty()) {
        oss << ", devices = [";
        oss << t.devices;
        oss << "]";
    }
    oss << ")";
    dbg << oss.str();
    return dbg;
}

//END Robot

} // namespace


class RobotInterface::XMLReader::Private
{
public:
    Private(XMLReader *parent);
    ~Private();

    void readFile();
    Robot readRobot(TiXmlElement *robotElem);
    Device readDevice(TiXmlElement *deviceElem);
    Param readParam(TiXmlElement *paramElem);
    void readParamList(TiXmlElement *paramListElem);
    Action readAction(TiXmlElement *actionElem);

    XMLReader * const parent;

    std::string filename;
    TiXmlDocument *doc;

    Robot robot;
};


RobotInterface::XMLReader::Private::Private(XMLReader *parent) :
    parent(parent),
    doc(NULL)
{

}

RobotInterface::XMLReader::Private::~Private()
{
    if (doc)
        delete doc;
}

void RobotInterface::XMLReader::Private::readFile()
{
    doc = new TiXmlDocument(filename.c_str());
    if (!doc->LoadFile()) {
        fatal() << SYNTAX_ERROR << "at line" << doc->ErrorRow() << ":" << doc->ErrorDesc();
    }

    if (!doc->RootElement()) {
        fatal() << SYNTAX_ERROR << "No root element.";
    }

    robot = readRobot(doc->RootElement());
    debug() << "  " << robot;
}


Robot RobotInterface::XMLReader::Private::readRobot(TiXmlElement *robotElem)
{
    if (robotElem->ValueStr().compare("robot") != 0) {
        fatal() << SYNTAX_ERROR << "Root element should be \"robot\", found" << robotElem->Value();
    }

    const char *nameAttr = robotElem->Attribute("name");
    if (!nameAttr) {
        fatal() << SYNTAX_ERROR << "\"robot\" element should contain the \"name\" attribute";
    }

    robot.name = nameAttr;
    debug() << "Found robot [" << robot.name << "]";

    for (TiXmlElement* deviceElem = robotElem->FirstChildElement(); deviceElem != 0; deviceElem = deviceElem->NextSiblingElement()) {
        robot.devices.push_back(readDevice(deviceElem));
    }

    return robot;
}

Device RobotInterface::XMLReader::Private::readDevice(TiXmlElement *deviceElem)
{

    if (deviceElem->ValueStr().compare("device") != 0) {
        fatal() << SYNTAX_ERROR << "Expected \"device\", found" << deviceElem->Value();
    }

    Device device;

    const char *nameAttr = deviceElem->Attribute("name");
    if (!nameAttr) {
        fatal() << SYNTAX_ERROR << "\"device\" element should contain the \"name\" attribute";
    }

    device.name = nameAttr;
    debug() << "    Found device [" << device.name << "]";

    const char *typeAttr = deviceElem->Attribute("type");
    if (!typeAttr) {
        fatal() << SYNTAX_ERROR << "\"device\" element should contain the \"type\" attribute";
    }
    else {
        device.type = typeAttr;
    }

    for (TiXmlElement* childElem = deviceElem->FirstChildElement(); childElem != 0; childElem = childElem->NextSiblingElement()) {
        if (!childElem->ValueStr().compare("param")) {
            device.params.push_back(readParam(childElem));
        } else if (!childElem->ValueStr().compare("paramlist")) {
            warning() << "\"paramlist\" not yet handled";
        } else if (!childElem->ValueStr().compare("subdevice")) {
            warning() << "\"subdevice\" not yet handled";
        } else if (!childElem->ValueStr().compare("action")) {
            device.actions.push_back(readAction(childElem));
        } else {
            fatal() << SYNTAX_ERROR << "\"device\" element contain an unknown tag" << childElem->ValueStr();
        }
    }

    debug() << "     " << device;
    return device;
}

Param RobotInterface::XMLReader::Private::readParam(TiXmlElement *paramElem)
{
    if (paramElem->ValueStr().compare("param") != 0) {
        fatal() << SYNTAX_ERROR << "Expected \"param\", found" << paramElem->Value();
    }

    Param param;

    const char *nameAttr = paramElem->Attribute("name");
    if (!nameAttr) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"name\" attribute";
    }

    param.name = nameAttr;
    debug() << "        Found param [" << param.name << "]";

    const char *valueText = paramElem->GetText();
    if (!nameAttr) {
        fatal() << SYNTAX_ERROR << "\"param\" element should have a value";
    }
    param.value = valueText;

    debug() << "          " << param;
    return param;
}

Action RobotInterface::XMLReader::Private::readAction(TiXmlElement* actionElem)
{
    if (actionElem->ValueStr().compare("action") != 0) {
        fatal() << SYNTAX_ERROR << "Expected \"action\", found" << actionElem->Value();
    }

    Action action;

    if (actionElem->QueryValueAttribute<ActionPhase>("phase", &action.phase) != TIXML_SUCCESS || action.phase == ActionPhaseUnknown) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"phase\" attribute [FIXME: list of accepted values]"; // FIXME
    }

    if (actionElem->QueryValueAttribute<ActionType>("type", &action.type) != TIXML_SUCCESS || action.type == ActionTypeUnknown) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"type\" attribute [startup|shutdown]";
    }

#if 0
    // BUG in TinyXML, see
    // https://sourceforge.net/tracker/?func=detail&aid=3567726&group_id=13559&atid=113559
    // When this bug is fixed upstream we can enable this
    if (actionElem->QueryUnsignedAttribute("level", &action.level) != TIXML_SUCCESS) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"level\" attribute";
    }
#else
    int tmp;
    if (actionElem->QueryIntAttribute("level", &tmp) != TIXML_SUCCESS || tmp < 0) {
        fatal() << SYNTAX_ERROR << "\"param\" element should contain the \"level\" attribute [unsigned int]";
    }
    action.level = (unsigned)tmp;
#endif

    return action;
}

RobotInterface::XMLReader::XMLReader(const std::string &filename) :
    mPriv(new Private(this))
{
    mPriv->filename = filename;
    mPriv->readFile();
}

RobotInterface::XMLReader::~XMLReader()
{
    delete mPriv;
}
