/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_ROBOT_H
#define ROBOTINTERFACE_ROBOT_H

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

private:
    class Private;
    Private * const mPriv;
}; // class Robot

} // namespace RobotInterface

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Robot &t);
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Robot &t);


#endif // ROBOTINTERFACE_ROBOT_H
