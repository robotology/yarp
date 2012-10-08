/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_ROBOTINTERFACE_H
#define ROBOTINTERFACE_ROBOTINTERFACE_H

#include <iosfwd>
#include <vector>

namespace yarp
{
namespace os
{
class Property;
}
}

namespace RobotInterface
{
class Debug;

//BEGIN Param

class Param
{
public:
    explicit Param();
    Param(const std::string &name, const std::string &value);
    Param(const Param &other);
    Param& operator=(const Param& other);

    virtual ~Param();

    std::string& name();
    std::string& value();

    const std::string& name() const;
    const std::string& value() const;

    yarp::os::Property toProperty() const;

private:
    class Private;
    Private * const mPriv;
};

typedef std::vector<Param> ParamList;

//END Param


//BEGIN Action

enum ActionPhase
{
    ActionPhaseUnknown = 0,
    ActionPhaseStartup,
    ActionPhaseShutdown,

    ActionPhaseReserved = 0xFF
};

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

class Action
{
public:
    explicit Action();
    Action(ActionPhase phase, ActionType type, unsigned int level);
    Action(const std::string &phase, const std::string &type, unsigned int level);
    Action(const Action &other);
    Action& operator=(const Action &other);

    ActionPhase& phase();
    ActionType& type();
    unsigned int& level();
    ParamList& params();

    ActionPhase phase() const;
    ActionType type() const;
    unsigned int level() const;
    const ParamList& params() const;

    virtual ~Action();

private:
    class Private;
    Private * const mPriv;
};

typedef std::vector<Action> ActionList;

//END Action


//BEGIN Device
class Device
{
public:
    explicit Device();
    Device(const std::string &name,
           const std::string &type,
           const ParamList &params = ParamList(),
           const ActionList &actions = ActionList());
    Device(const Device &other);
    Device& operator=(const Device &other);

    virtual ~Device();

    std::string& name();
    std::string& type();
    ParamList& params();
    ActionList& actions();

    const std::string& name() const;
    const std::string& type() const;
    const ParamList& params() const;
    const ActionList& actions() const;

    yarp::os::Property paramsAsProperty() const;

private:
    class Private;
    Private * const mPriv;
};

typedef std::vector<Device> DeviceList;

//END Device


//BEGIN Robot
class Robot
{
public:
    explicit Robot();
    explicit Robot(const std::string &name, const DeviceList &devices = DeviceList());
    Robot(const Robot &other);
    Robot& operator=(const Robot &other);

    virtual ~Robot();

    std::string& name();
    ParamList& params();
    DeviceList& devices();
    Device& device(const std::string &name);

    const std::string& name() const;
    const ParamList& params() const;
    const DeviceList& devices() const;
    const Device& device(const std::string &name) const;

private:
    class Private;
    Private * const mPriv;
};


//END Robot

#if 0
class RobotInterface
{
public:
    explicit RobotInterface();
    virtual ~RobotInterface();

private:
    class Private;
    Private * const mPriv;
}; // class RobotInterface
#endif

} // namespace RobotInterface

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Param &t);
std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Action &t);
std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Device &t);
std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Robot &t);

RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Param &t);
RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Action &t);
RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Device &t);
RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Robot &t);

// These are required to use TiXmlElement::QueryValueAttribute<T>
void operator>>(const std::stringstream &sstream, RobotInterface::ActionPhase &actionphase);
void operator>>(const std::stringstream &sstream, RobotInterface::ActionType &actiontype);


#endif // ROBOTINTERFACE_ROBOTINTERFACE_H
