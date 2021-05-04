/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/experimental/Action.h>

#include <yarp/robotinterface/experimental/Param.h>

#include <yarp/os/LogStream.h>

#include <string>


class yarp::robotinterface::experimental::Action::Private
{
public:
    Private(Action* /*parent*/) :
            phase(ActionPhaseUnknown),
            type(ActionTypeUnknown),
            level(0)
    {
    }

    ActionPhase phase;
    ActionType type;
    unsigned int level;
    ParamList params;
};

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::experimental::Action& t)
{
    dbg << "(\"" << ActionPhaseToString(t.phase()) << ":" << ActionTypeToString(t.type()) << ":" << t.level() << "\"";
    if (!t.params().empty()) {
        dbg << ", params = [";
        dbg << t.params();
        dbg << "]";
    }
    dbg << ")";
    return dbg;
}

yarp::robotinterface::experimental::Action::Action() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::experimental::Action::Action(const std::string& phase, const std::string& type, unsigned int level) :
        mPriv(new Private(this))
{
    mPriv->phase = StringToActionPhase(phase);
    mPriv->type = StringToActionType(type);
    mPriv->level = level;
}

yarp::robotinterface::experimental::Action::Action(yarp::robotinterface::experimental::ActionPhase phase, yarp::robotinterface::experimental::ActionType type, unsigned int level) :
        mPriv(new Private(this))
{
    mPriv->phase = phase;
    mPriv->type = type;
    mPriv->level = level;
}

yarp::robotinterface::experimental::Action::Action(const yarp::robotinterface::experimental::Action& other) :
        mPriv(new Private(this))
{
    mPriv->phase = other.mPriv->phase;
    mPriv->type = other.mPriv->type;
    mPriv->level = other.mPriv->level;
    mPriv->params = other.mPriv->params;
}

yarp::robotinterface::experimental::Action& yarp::robotinterface::experimental::Action::operator=(const yarp::robotinterface::experimental::Action& other)
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

yarp::robotinterface::experimental::Action::~Action()
{
    delete mPriv;
}

yarp::robotinterface::experimental::ActionPhase& yarp::robotinterface::experimental::Action::phase()
{
    return mPriv->phase;
}

yarp::robotinterface::experimental::ActionType& yarp::robotinterface::experimental::Action::type()
{
    return mPriv->type;
}

unsigned int& yarp::robotinterface::experimental::Action::level()
{
    return mPriv->level;
}

yarp::robotinterface::experimental::ParamList& yarp::robotinterface::experimental::Action::params()
{
    return mPriv->params;
}

yarp::robotinterface::experimental::ActionPhase yarp::robotinterface::experimental::Action::phase() const
{
    return mPriv->phase;
}

yarp::robotinterface::experimental::ActionType yarp::robotinterface::experimental::Action::type() const
{
    return mPriv->type;
}

unsigned int yarp::robotinterface::experimental::Action::level() const
{
    return mPriv->level;
}

const yarp::robotinterface::experimental::ParamList& yarp::robotinterface::experimental::Action::params() const
{
    return mPriv->params;
}

bool yarp::robotinterface::experimental::Action::hasParam(const std::string& name) const
{
    return yarp::robotinterface::experimental::hasParam(mPriv->params, name);
}

std::string yarp::robotinterface::experimental::Action::findParam(const std::string& name) const
{
    return yarp::robotinterface::experimental::findParam(mPriv->params, name);
}
