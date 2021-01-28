/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTREADERBUFFERBASECREATOR_H
#define YARP_OS_PORTREADERBUFFERBASECREATOR_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

class PortReader;

class YARP_os_API PortReaderBufferBaseCreator
{
public:
    virtual ~PortReaderBufferBaseCreator();

    virtual yarp::os::PortReader* create() const = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTREADERBUFFERBASECREATOR_H
