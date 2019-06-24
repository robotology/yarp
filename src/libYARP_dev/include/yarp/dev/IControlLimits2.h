/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLLIMITS2_H
#define YARP_DEV_ICONTROLLIMITS2_H

#include <yarp/dev/IControlLimits.h>

namespace yarp {
    namespace dev {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlLimits instead") IControlLimits IControlLimits2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlLimitsRaw instead") IControlLimitsRaw IControlLimits2Raw;
#endif
    }
}

#endif // YARP_DEV_ICONTROLLIMITS2_H
