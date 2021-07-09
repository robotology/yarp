/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_RUN_H
#define YARP_RUN_RUN_H

#include <yarp/conf/system.h>

YARP_COMPILER_DEPRECATED_WARNING(<yarp/os/Run.h> header is deprecated. Use <yarp/run/Run.h> instead. If your build fails you need to link the YARP_run library.)

#include <yarp/run/Run.h>


namespace yarp {
namespace os {
    YARP_DEPRECATED_MSG("yarp::os::Run is deprecated. Use yarp::run::Run instead")
    typedef yarp::run::Run Run;
} // namespace os
} // namespace yarp


#endif // YARP_RUN_RUN_H
