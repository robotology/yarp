/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTREADERBUFFERBASECREATOR_H
#define YARP_OS_IMPL_PORTREADERBUFFERBASECREATOR_H

#include <yarp/os/api.h>
#include <yarp/os/PortReader.h>

namespace yarp::os::impl {


class YARP_os_API PortReaderBufferBaseCreator
{
public:
    virtual ~PortReaderBufferBaseCreator();

    virtual yarp::os::PortReader* create() const = 0;
};

} // namespace yarp::os::impl

#endif // YARP_OS_IMPL_PORTREADERBUFFERBASECREATOR_H
