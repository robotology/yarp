/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDINTERFACESIMPL_H
#define YARP_DEV_CONTROLBOARDINTERFACESIMPL_H

#include <yarp/dev/ControlBoardInterfaces.h>

#include <yarp/dev/ImplementEncodersTimed.h>
#include <yarp/dev/ImplementMotorEncoders.h>
#include <yarp/dev/ImplementMotor.h>
#include <yarp/dev/ImplementRemoteVariables.h>
#include <yarp/dev/ImplementControlMode.h>
#include <yarp/dev/ImplementControlMode2.h>
#include <yarp/dev/ImplementTorqueControl.h>
#include <yarp/dev/ImplementCurrentControl.h>
#include <yarp/dev/ImplementPWMControl.h>
#include <yarp/dev/ImplementImpedanceControl.h>
#include <yarp/dev/ImplementAxisInfo.h>
#include <yarp/dev/ImplementControlCalibration.h>
#include <yarp/dev/IPidControlImpl.h>
#include <yarp/dev/IPositionControl2Impl.h>
#include <yarp/dev/IVelocityControl2Impl.h>
#include <yarp/dev/IControlLimitsImpl.h>
#include <yarp/dev/IPositionDirectImpl.h>
#include <yarp/dev/IInteractionModeImpl.h>

// old interface implementation file
#include <yarp/dev/ImplementControlBoardInterfaces.h>

#endif // YARP_DEV_CONTROLBOARDINTERFACESIMPL_H
