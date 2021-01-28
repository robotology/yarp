/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NAMESTORE_H
#define YARP_OS_NAMESTORE_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

namespace yarp {
namespace os {

/**
 * Abstract interface for a database of port names.
 */
class YARP_os_API NameStore
{
public:
    virtual ~NameStore();
    virtual Contact query(const std::string& name) = 0;
    virtual bool announce(const std::string& name, int activity) = 0;
    virtual bool process(PortWriter& in,
                         PortReader& out,
                         const Contact& source) = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_NAMESTORE_H
