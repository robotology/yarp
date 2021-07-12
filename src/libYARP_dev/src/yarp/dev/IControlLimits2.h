/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ICONTROLLIMITS2_H
#define YARP_DEV_ICONTROLLIMITS2_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_ICONTROLLIMITS2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/IControlLimits2.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

#include <yarp/dev/IControlLimits.h>

namespace yarp {
    namespace dev {
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlLimits instead") IControlLimits IControlLimits2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlLimitsRaw instead") IControlLimitsRaw IControlLimits2Raw;
    }
}

#endif

#endif // YARP_DEV_ICONTROLLIMITS2_H
