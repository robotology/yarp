/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVELOCITYCONTROL2_H
#define YARP_DEV_IVELOCITYCONTROL2_H

#include <yarp/dev/IVelocityControl.h>
namespace yarp {
    namespace dev {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IVelocityControl instead") IVelocityControl IVelocityControl2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IVelocityControlRaw instead") IVelocityControlRaw IVelocityControl2Raw;
#endif
    }
}
#endif // YARP_DEV_IVELOCITYCONTROL2_H
