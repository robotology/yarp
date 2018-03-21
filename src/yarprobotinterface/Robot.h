/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_YARPROBOTINTERFACE_ROBOT_H
#define YARP_YARPROBOTINTERFACE_ROBOT_H

#include "Types.h"

namespace RobotInterface
{

class Robot
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
    bool enterPhase(RobotInterface::ActionPhase phase);
    RobotInterface::ActionPhase currentPhase() const;
    int currentLevel() const;

private:
    class Private;
    Private * const mPriv;
}; // class Robot

} // namespace RobotInterface

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Robot &t);
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Robot &t);


#endif // YARP_YARPROBOTINTERFACE_ROBOT_H
