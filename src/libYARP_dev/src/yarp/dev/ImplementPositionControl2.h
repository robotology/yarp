/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IPOSITIONCONTROL2IMPL_H
#define YARP_DEV_IPOSITIONCONTROL2IMPL_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IMPLEMENTPOSITIONCONTROL2_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/ImplementPositionControl2.h> file is deprecated")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

#include <yarp/dev/ImplementPositionControl.h>

namespace yarp{
    namespace dev {
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::ImplementPositionControl instead") ImplementPositionControl ImplementPositionControl2;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::dev::StubImplPositionControlRaw instead") StubImplPositionControlRaw StubImplPositionControl2Raw;
    }
}

#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_IPOSITIONCONTROL2IMPL_H
