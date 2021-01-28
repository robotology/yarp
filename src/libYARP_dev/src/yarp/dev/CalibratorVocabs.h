/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CALIBRATORVOCABS_H
#define YARP_DEV_CALIBRATORVOCABS_H

#include <yarp/os/Vocab.h>

constexpr yarp::conf::vocab32_t VOCAB_REMOTE_CALIBRATOR_INTERFACE           = yarp::os::createVocab('r','e','c','a');
constexpr yarp::conf::vocab32_t VOCAB_IS_CALIBRATOR_PRESENT                 = yarp::os::createVocab('i','s','c','a');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_SINGLE_JOINT                = yarp::os::createVocab('c','a','l');
constexpr yarp::conf::vocab32_t VOCAB_CALIBRATE_WHOLE_PART                  = yarp::os::createVocab('c','a','l','s');
constexpr yarp::conf::vocab32_t VOCAB_HOMING_SINGLE_JOINT                   = yarp::os::createVocab('h','o','m');
constexpr yarp::conf::vocab32_t VOCAB_HOMING_WHOLE_PART                     = yarp::os::createVocab('h','o','m','s');
constexpr yarp::conf::vocab32_t VOCAB_PARK_SINGLE_JOINT                     = yarp::os::createVocab('p','a','r');
constexpr yarp::conf::vocab32_t VOCAB_PARK_WHOLE_PART                       = yarp::os::createVocab('p','a','r','s');
constexpr yarp::conf::vocab32_t VOCAB_QUIT_CALIBRATE                        = yarp::os::createVocab('q','u','c','a');
constexpr yarp::conf::vocab32_t VOCAB_QUIT_PARK                             = yarp::os::createVocab('q','u','p','a');

#endif // YARP_DEV_CALIBRATORVOCABS_H
