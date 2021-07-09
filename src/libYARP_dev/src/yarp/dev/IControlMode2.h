/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLMODE2_H
#define YARP_DEV_ICONTROLMODE2_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_ICONTROLMODE2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IControlMode2.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

#include <yarp/dev/IControlMode.h>

namespace yarp {
    namespace dev {
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlModeRaw instead") IControlModeRaw IControlMode2Raw;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlMode instead") IControlMode IControlMode2;
    }
}

#endif

#endif // YARP_DEV_ICONTROLMODE2_H
