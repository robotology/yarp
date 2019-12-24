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

#ifndef YARP_YARPROBOTINTERFACE_DEVICE_H
#define YARP_YARPROBOTINTERFACE_DEVICE_H

#include "Types.h"



namespace yarp { namespace dev { class PolyDriver; } }
namespace yarp { namespace dev { class PolyDriverList; } }

namespace yarp {
namespace robotinterface {

class YARP_robotinterface_API Device
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

    bool hasParam(const std::string &name) const;
    std::string findParam(const std::string &name) const;

    bool open();
    bool close();

    yarp::dev::PolyDriver *driver() const;

    // thread handling methods
    void registerThread(yarp::os::Thread *thread) const;
    void joinThreads() const;
    void stopThreads() const;

    // configure action
    bool configure(const Device &target, const ParamList& params) const;

    // calibrate one device
    bool calibrate(const Device &target) const;

    // attach a list of drivers to this wrapper
    bool attach(const yarp::dev::PolyDriverList &drivers) const;

    // abort action
    bool abort() const;

    // detach all drivers attached to this wrapper
    bool detach() const;

    // park
    bool park(const Device &target) const;

    // custom action
    bool custom(const ParamList &params) const;


private:
    class Private;
    Private * const mPriv;
}; // class Device

} // namespace robotinterface
} // namespace yarp

namespace std { std::ostream& operator<<(std::ostream &oss, const yarp::robotinterface::Device &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Device &t);


#endif // YARP_YARPROBOTINTERFACE_DEVICE_H
