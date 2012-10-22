/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Action.h"
#include "Param.h"

#include <debugStream/Debug.h>

#include <string>



class RobotInterface::Action::Private
{
public:
    Private(Action * /*parent*/) {}

    ActionPhase phase;
    ActionType type;
    unsigned int level;
    ParamList params;
};

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Action &t)
{
    oss << "(\"" << ActionPhaseToString(t.phase()) << ":" << ActionTypeToString(t.type()) << ":" << t.level() << "\"";
    if (!t.params().empty()) {
        oss << ", params = [";
        oss << t.params();
        oss << "]";
    }
    oss << ")";
    return oss;
}


DebugStream::Debug operator<<(DebugStream::Debug dbg, const RobotInterface::Action &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Action::Action() :
    mPriv(new Private(this))
{
}

RobotInterface::Action::Action(const std::string& phase, const std::string& type, unsigned int level) :
    mPriv(new Private(this))
{
    mPriv->phase = StringToActionPhase(phase);
    mPriv->type = StringToActionType(type);
    mPriv->level = level;
}

RobotInterface::Action::Action(RobotInterface::ActionPhase phase, RobotInterface::ActionType type, unsigned int level) :
    mPriv(new Private(this))
{
    mPriv->phase = phase;
    mPriv->type = type;
    mPriv->level = level;
}

RobotInterface::Action::Action(const RobotInterface::Action& other) :
    mPriv(new Private(this))
{
    mPriv->phase = other.mPriv->phase;
    mPriv->type = other.mPriv->type;
    mPriv->level = other.mPriv->level;
    mPriv->params = other.mPriv->params;
}

RobotInterface::Action& RobotInterface::Action::operator=(const RobotInterface::Action& other)
{
    if (&other != this) {
        mPriv->phase = other.mPriv->phase;
        mPriv->type = other.mPriv->type;
        mPriv->level = other.mPriv->level;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;
    }

    return *this;
}

RobotInterface::Action::~Action()
{
    delete mPriv;
}

RobotInterface::ActionPhase& RobotInterface::Action::phase()
{
    return mPriv->phase;
}

RobotInterface::ActionType& RobotInterface::Action::type()
{
    return mPriv->type;
}

unsigned int& RobotInterface::Action::level()
{
    return mPriv->level;
}

RobotInterface::ParamList& RobotInterface::Action::params()
{
    return mPriv->params;
}

RobotInterface::ActionPhase RobotInterface::Action::phase() const
{
    return mPriv->phase;
}

RobotInterface::ActionType RobotInterface::Action::type() const
{
    return mPriv->type;
}

unsigned int RobotInterface::Action::level() const
{
    return mPriv->level;
}

const RobotInterface::ParamList& RobotInterface::Action::params() const
{
    return mPriv->params;
}

bool RobotInterface::Action::hasParam(const std::string& name) const
{
    return RobotInterface::hasParam(mPriv->params, name);
}

std::string RobotInterface::Action::findParam(const std::string& name) const
{
    return RobotInterface::findParam(mPriv->params, name);
}
