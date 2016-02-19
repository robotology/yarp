/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_YARPROBOTINTERFACE_ACTION_H
#define YARP_YARPROBOTINTERFACE_ACTION_H

#include "Types.h"

namespace RobotInterface
{

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

    bool hasParam(const std::string &name) const;
    std::string findParam(const std::string &name) const;

    virtual ~Action();

private:
    class Private;
    Private * const mPriv;
}; // class Action

} // namespace RobotInterface


namespace std { std::ostream& operator<<(std::ostream &oss, const RobotInterface::Action &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Action &t);


#endif // YARP_YARPROBOTINTERFACE_ACTION_H
