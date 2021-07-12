/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDINTERFACES_H
#define YARP_DEV_CONTROLBOARDINTERFACES_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>

#include <yarp/dev/ICalibrator.h>
#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/dev/CalibratorVocabs.h>

/*! \file ControlBoardInterfaces.h define control board standard interfaces*/

#include <yarp/dev/IEncoders.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPositionDirect.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IMotorEncoders.h>
#include <yarp/dev/IMotor.h>
#include <yarp/dev/IRemoteVariables.h>
#include <yarp/dev/IControlDebug.h>
#include <yarp/dev/IControlCalibration.h>
#include <yarp/dev/IAmplifierControl.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IControlLimits.h>
#include <yarp/dev/IControlMode.h>

#include <yarp/dev/ControlBoardVocabs.h>

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_ICONTROLMODE2_H_ON_PURPOSE
#include <yarp/dev/IControlMode2.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_ICONTROLMODE2_H_ON_PURPOSE

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IPOSITIONCONTROL2_H_ON_PURPOSE
#include <yarp/dev/IPositionControl2.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IPOSITIONCONTROL2_H_ON_PURPOSE

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IVELOCITYCONTROL2_H_ON_PURPOSE
#include <yarp/dev/IVelocityControl2.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_IVELOCITYCONTROL2_H_ON_PURPOSE

#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
