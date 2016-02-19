/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_YARPROBOTINTERFACE_DEVICE_H
#define YARP_YARPROBOTINTERFACE_DEVICE_H

#include "Types.h"

namespace yarp { namespace dev { class PolyDriver; } }
namespace yarp { namespace dev { class PolyDriverList; } }

namespace RobotInterface
{

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

} // RobotInterface

namespace std { std::ostream& operator<<(std::ostream &oss, const RobotInterface::Device &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Device &t);


#endif // YARP_YARPROBOTINTERFACE_DEVICE_H
