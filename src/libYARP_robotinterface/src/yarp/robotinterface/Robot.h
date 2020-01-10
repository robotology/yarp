/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_ROBOT_H
#define YARP_ROBOTINTERFACE_ROBOT_H

#include <yarp/robotinterface/Types.h>
#include <yarp/robotinterface/Device.h>

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

YARP_robotinterface_API std::ostringstream& operator<<(std::ostringstream &oss, const yarp::robotinterface::Robot &t);
YARP_robotinterface_API yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Robot &t);


#endif // YARP_ROBOTINTERFACE_ROBOT_H
