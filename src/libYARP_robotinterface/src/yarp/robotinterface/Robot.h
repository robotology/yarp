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

#ifndef YARP_ROBOTINTERFACE_ROBOT_H
#define YARP_ROBOTINTERFACE_ROBOT_H

#include "Types.h"

#include <yarp/robotinterface/api.h>

namespace yarp {
namespace robotinterface {

class YARP_robotinterface_API Robot
{
public:
    explicit Robot();
    explicit Robot(const std::string &name, const DeviceList &devices = DeviceList());
    Robot(const Robot &other);
    Robot& operator=(const Robot &other);

    virtual ~Robot();

    std::string& name();
    unsigned int& build();
    std::string& portprefix();

    void setVerbose(bool verbose);
    void setAllowDeprecatedDevices(bool allowDeprecatedDevices);

    ParamList& params();
    DeviceList& devices();
    Device& device(const std::string &name);

    const std::string& name() const;
    const unsigned int& build() const;
    const std::string& portprefix() const;
    const ParamList& params() const;
    const DeviceList& devices() const;
    const Device& device(const std::string &name) const;

    bool hasParam(const std::string &name) const;
    std::string findParam(const std::string &name) const;

    void interrupt();
    bool enterPhase(yarp::robotinterface::ActionPhase phase);
    yarp::robotinterface::ActionPhase currentPhase() const;
    int currentLevel() const;

private:
    class Private;
    Private * const mPriv;
}; // class Robot

} // namespace robotinterface
} // namespace yarp

std::ostringstream& operator<<(std::ostringstream &oss, const yarp::robotinterface::Robot &t);
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Robot &t);


#endif // YARP_ROBOTINTERFACE_ROBOT_H
