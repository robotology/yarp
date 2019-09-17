/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDINTERFACES_H
#define YARP_DEV_CONTROLBOARDINTERFACES_H

#include <yarp/os/Vocab.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardPid.h>

#include <yarp/dev/ICalibrator.h>
#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/dev/CalibratorVocabs.h>

/*! \file ControlBoardInterfaces.h define control board standard interfaces*/

#include <yarp/dev/IEncoders.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IVelocityControl2.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IPositionControl2.h>
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

/* Vocabs representing the above interfaces */
constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONE  = yarp::os::createVocab('d','o','n');
constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONES = yarp::os::createVocab('d','o','n','s');
constexpr yarp::conf::vocab32_t VOCAB_TIMESTAMP    = yarp::os::createVocab('t', 's', 't', 'a');

// interface IPositionControl sets
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MODE  = yarp::os::createVocab('p','o','s','d');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVE  = yarp::os::createVocab('p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVES = yarp::os::createVocab('p','o','s','s');
constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVE  = yarp::os::createVocab('r','e','l');

constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVES    = yarp::os::createVocab('r','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEED         = yarp::os::createVocab('v','e','l');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEEDS        = yarp::os::createVocab('v','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATION  = yarp::os::createVocab('a','c','c');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATIONS = yarp::os::createVocab('a','c','c','s');
constexpr yarp::conf::vocab32_t VOCAB_STOP              = yarp::os::createVocab('s','t','o');
constexpr yarp::conf::vocab32_t VOCAB_STOPS             = yarp::os::createVocab('s','t','o','s');

// interface IVelocityControl sets
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MODE  = yarp::os::createVocab('v','e','l','d');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVE  = yarp::os::createVocab('v','m','o');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVES = yarp::os::createVocab('v','m','o','s');

// protocol version
constexpr yarp::conf::vocab32_t VOCAB_PROTOCOL_VERSION = yarp::os::createVocab('p', 'r', 'o', 't');

#endif // YARP_DEV_CONTROLBOARDINTERFACES_H
