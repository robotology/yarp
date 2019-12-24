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

#ifndef YARP_YARPROBOTINTERFACE_PARAM_H
#define YARP_YARPROBOTINTERFACE_PARAM_H

#include "Types.h"

namespace yarp { namespace os { class Property; } }

namespace yarp {
namespace robotinterface {

class YARP_robotinterface_API Param
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

} // namespace robotinterface
} // namespace yarp

namespace std { std::ostream& operator<<(std::ostream &oss, const yarp::robotinterface::Param &t); }
yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Param &t);


#endif // YARP_YARPROBOTINTERFACE_PARAM_H
