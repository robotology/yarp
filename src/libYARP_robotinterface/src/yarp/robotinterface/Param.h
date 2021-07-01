/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_PARAM_H
#define YARP_ROBOTINTERFACE_PARAM_H

#include <yarp/robotinterface/Types.h>

namespace yarp {

namespace os {
class Property;
} // namespace os

namespace robotinterface {

class YARP_robotinterface_API Param
{
public:
    explicit Param(bool isGroup = false);
    Param(const std::string& name, const std::string& value, bool isGroup = false);
    Param(const Param& other);
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
    Private* const mPriv;
}; // class Param

} // namespace robotinterface
} // namespace yarp

namespace std {
YARP_robotinterface_API std::ostream& operator<<(std::ostream& oss, const yarp::robotinterface::Param& t);
}
YARP_robotinterface_API yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Param& t);


#endif // YARP_ROBOTINTERFACE_PARAM_H
