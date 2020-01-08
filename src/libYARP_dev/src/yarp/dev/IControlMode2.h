/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ICONTROLMODE2_H
#define YARP_DEV_ICONTROLMODE2_H

#include <yarp/dev/IControlMode.h>


namespace yarp {
    namespace dev {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlModeRaw instead") IControlModeRaw IControlMode2Raw;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::IControlMode instead") IControlMode IControlMode2;
#endif
    }
}
#endif // YARP_DEV_ICONTROLMODE2_H
