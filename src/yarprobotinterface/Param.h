/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_YARPROBOTINTERFACE_PARAM_H
#define YARP_YARPROBOTINTERFACE_PARAM_H

#include "Types.h"

namespace yarp { namespace os { class Property; } }

namespace RobotInterface
{

class Param
{
public:
    explicit Param(bool isGroup = false);
    Param(const std::string &name, const std::string &value, bool isGroup = false);
    Param(const Param &other);
    Param& operator=(const Param& other);

    virtual ~Param();

    std::string& name();
    std::string& value();

    const std::string& name() const;
    const std::string& value() const;

    bool isGroup() const;

    yarp::os::Property toProperty() const;

private:
    class Private;
    Private * const mPriv;
}; // class Param

} // RobotInterface

namespace std { std::ostream& operator<<(std::ostream &oss, const RobotInterface::Param &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Param &t);


#endif // YARP_YARPROBOTINTERFACE_PARAM_H
