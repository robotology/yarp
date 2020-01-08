/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IVELOCITYCONTROL2IMPL_H
#define YARP_DEV_IVELOCITYCONTROL2IMPL_H


#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/os/Log.h>

namespace yarp {
    namespace dev {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::ImplementVelocityControl instead") ImplementVelocityControl ImplementVelocityControl2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::StubImplVelocityControlRaw instead") StubImplVelocityControlRaw StubImplVelocityControl2Raw;
#endif
    }
}
#endif // YARP_DEV_IVELOCITYCONTROL2IMPL_H
