/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_YARPROBOTINTERFACE_ACTION_H
#define YARP_YARPROBOTINTERFACE_ACTION_H

#include "Types.h"

namespace yarp {
namespace robotinterface {

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

} // namespace robotinterface
} // namespace yarp


namespace std { std::ostream& operator<<(std::ostream &oss, const yarp::robotinterface::Action &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Action &t);


#endif // YARP_YARPROBOTINTERFACE_ACTION_H
