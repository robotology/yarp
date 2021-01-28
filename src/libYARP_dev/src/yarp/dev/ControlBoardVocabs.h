/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDVOCABS_H
#define YARP_DEV_CONTROLBOARDVOCABS_H

#include <yarp/os/Vocab.h>

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

#endif // YARP_DEV_CONTROLBOARDVOCABS_H
