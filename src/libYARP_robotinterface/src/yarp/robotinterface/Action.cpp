/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/Action.h>

#include <yarp/robotinterface/Param.h>

#include <yarp/os/LogStream.h>

#include <string>


class yarp::robotinterface::Action::Private
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

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Action& t)
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

yarp::robotinterface::Action::Action() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::Action::Action(const std::string& phase, const std::string& type, unsigned int level) :
        mPriv(new Private(this))
{
    mPriv->phase = StringToActionPhase(phase);
    mPriv->type = StringToActionType(type);
    mPriv->level = level;
}

yarp::robotinterface::Action::Action(yarp::robotinterface::ActionPhase phase, yarp::robotinterface::ActionType type, unsigned int level) :
        mPriv(new Private(this))
{
    mPriv->phase = phase;
    mPriv->type = type;
    mPriv->level = level;
}

yarp::robotinterface::Action::Action(const yarp::robotinterface::Action& other) :
        mPriv(new Private(this))
{
    mPriv->phase = other.mPriv->phase;
    mPriv->type = other.mPriv->type;
    mPriv->level = other.mPriv->level;
    mPriv->params = other.mPriv->params;
}

yarp::robotinterface::Action& yarp::robotinterface::Action::operator=(const yarp::robotinterface::Action& other)
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

yarp::robotinterface::Action::~Action()
{
    delete mPriv;
}

yarp::robotinterface::ActionPhase& yarp::robotinterface::Action::phase()
{
    return mPriv->phase;
}

yarp::robotinterface::ActionType& yarp::robotinterface::Action::type()
{
    return mPriv->type;
}

unsigned int& yarp::robotinterface::Action::level()
{
    return mPriv->level;
}

yarp::robotinterface::ParamList& yarp::robotinterface::Action::params()
{
    return mPriv->params;
}

yarp::robotinterface::ActionPhase yarp::robotinterface::Action::phase() const
{
    return mPriv->phase;
}

yarp::robotinterface::ActionType yarp::robotinterface::Action::type() const
{
    return mPriv->type;
}

unsigned int yarp::robotinterface::Action::level() const
{
    return mPriv->level;
}

const yarp::robotinterface::ParamList& yarp::robotinterface::Action::params() const
{
    return mPriv->params;
}

bool yarp::robotinterface::Action::hasParam(const std::string& name) const
{
    return yarp::robotinterface::hasParam(mPriv->params, name);
}

std::string yarp::robotinterface::Action::findParam(const std::string& name) const
{
    return yarp::robotinterface::findParam(mPriv->params, name);
}
