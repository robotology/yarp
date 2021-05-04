/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_DEVICE_H
#define YARP_ROBOTINTERFACE_DEVICE_H

#include <yarp/robotinterface/experimental/Action.h>
#include <yarp/robotinterface/experimental/Param.h>
#include <yarp/robotinterface/experimental/Types.h>

namespace yarp {

namespace dev {
class PolyDriver;
class PolyDriverDescriptor;
class PolyDriverList;
} // namespace dev

namespace robotinterface {
namespace experimental {

class YARP_robotinterface_API Device
{
public:
    explicit Device();
    Device(const std::string& name,
           const std::string& type,
           const ParamList& params = ParamList(),
           const ActionList& actions = ActionList());
    Device(const Device& other);
    Device& operator=(const Device& other);

    virtual ~Device();

    std::string& name();
    std::string& type();
    ParamList& params();
    ActionList& actions();

    const std::string& name() const;
    const std::string& type() const;
    const ParamList& params() const;
    const ActionList& actions() const;

    bool hasParam(const std::string& name) const;
    std::string findParam(const std::string& name) const;

    bool open();
    bool close();

    yarp::dev::PolyDriver* driver() const;

    // thread handling methods
    void registerThread(yarp::os::Thread* thread) const;
    void joinThreads() const;
    void stopThreads() const;

    // configure action
    bool configure(const Device& target, const ParamList& params) const;

    // calibrate one device
    bool calibrate(const yarp::dev::PolyDriverDescriptor& target) const;

    // attach a list of drivers to this wrapper
    bool attach(const yarp::dev::PolyDriverList& drivers) const;

    // abort action
    bool abort() const;

    // detach all drivers attached to this wrapper
    bool detach() const;

    // park
    bool park(const yarp::dev::PolyDriverDescriptor& target) const;

    // custom action
    bool custom(const ParamList& params) const;


private:
    class Private;
    Private* const mPriv;
}; // class Device

} // namespace experimental
} // namespace robotinterface
} // namespace yarp

YARP_robotinterface_API yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::experimental::Device& t);


#endif // YARP_ROBOTINTERFACE_DEVICE_H
