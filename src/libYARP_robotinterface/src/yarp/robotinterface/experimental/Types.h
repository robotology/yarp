/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTINTERFACE_EXPERIMENTAL_TYPES_H
#define YARP_ROBOTINTERFACE_EXPERIMENTAL_TYPES_H

#include <yarp/conf/system.h>
#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_YARP_ROBOTINTERFACE_EXPERIMENTAL_TYPES_H_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/dev/experimental/Types.h> file is deprecated in favour of <yarp/dev/Types.h>")
#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.5.0

#include <yarp/robotinterface/Types.h>

namespace yarp {
namespace robotinterface {
namespace experimental {

YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::ParamList") yarp::robotinterface::ParamList ParamList;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::ActionList") yarp::robotinterface::ActionList ActionList;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::DeviceList") yarp::robotinterface::DeviceList DeviceList;
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::ThreadList") yarp::robotinterface::ThreadList ThreadList;

// YARP_DEPRECATED_MSG("Use yarp::robotinterface::hasParam")
using robotinterface::hasParam;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::findParam")
using robotinterface::findParam;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::hasGroup")
using robotinterface::hasGroup;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::findGroup")
using robotinterface::findGroup;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::mergeDuplicateGroups")
using robotinterface::mergeDuplicateGroups;

// YARP_DEPRECATED_MSG("Use yarp::robotinterface::ActionPhase")
YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::ActionPhase") yarp::robotinterface::ActionPhase ActionPhase;
using robotinterface::ActionPhaseUnknown;
using robotinterface::ActionPhaseStartup;
using robotinterface::ActionPhaseRun;
using robotinterface::ActionPhaseInterrupt1;
using robotinterface::ActionPhaseInterrupt2;
using robotinterface::ActionPhaseInterrupt3;
using robotinterface::ActionPhaseShutdown;
using robotinterface::ActionPhaseReserved;

// YARP_DEPRECATED_MSG("Use yarp::robotinterface::StringToActionPhase")
using robotinterface::StringToActionPhase;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::ActionPhaseToString")
using robotinterface::ActionPhaseToString;


YARP_DEPRECATED_TYPEDEF_MSG("Use yarp::robotinterface::ActionType") yarp::robotinterface::ActionType ActionType;
using robotinterface::ActionTypeUnknown;
using robotinterface::ActionTypeConfigure;
using robotinterface::ActionTypeCalibrate;
using robotinterface::ActionTypeAttach;
using robotinterface::ActionTypeAbort;
using robotinterface::ActionTypeDetach;
using robotinterface::ActionTypePark;
using robotinterface::ActionTypeCustom;

// YARP_DEPRECATED_MSG("Use yarp::robotinterface::StringToActionType")
using robotinterface::StringToActionType;
// YARP_DEPRECATED_MSG("Use yarp::robotinterface::ActionTypeToString")
using robotinterface::ActionTypeToString;

} // namespace experimental
} // namespace robotinterface
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_ROBOTINTERFACE_EXPERIMENTAL_TYPES_H
