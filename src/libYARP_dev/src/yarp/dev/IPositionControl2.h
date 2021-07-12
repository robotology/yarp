/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IPOSITIONCONTROL2_H
#define YARP_DEV_IPOSITIONCONTROL2_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IPOSITIONCONTROL2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IPositionControl2.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

#include <yarp/dev/IPositionControl.h>
namespace yarp {
    namespace dev {
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IPositionControl instead") IPositionControl IPositionControl2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IPositionControlRaw instead") IPositionControlRaw IPositionControl2Raw;
    }
}
#endif

#endif // YARP_DEV_IPOSITIONCONTROL2_H
