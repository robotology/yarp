/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDVOCABS_H
#define YARP_DEV_CONTROLBOARDVOCABS_H

#include <yarp/os/Vocab.h>

/* Vocabs representing the above interfaces */
constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONE  = yarp::os::createVocab32('d','o','n');
constexpr yarp::conf::vocab32_t VOCAB_MOTION_DONES = yarp::os::createVocab32('d','o','n','s');
constexpr yarp::conf::vocab32_t VOCAB_TIMESTAMP    = yarp::os::createVocab32('t', 's', 't', 'a');

// interface IPositionControl sets
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MODE  = yarp::os::createVocab32('p','o','s','d');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVE  = yarp::os::createVocab32('p','o','s');
constexpr yarp::conf::vocab32_t VOCAB_POSITION_MOVES = yarp::os::createVocab32('p','o','s','s');
constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVE  = yarp::os::createVocab32('r','e','l');

constexpr yarp::conf::vocab32_t VOCAB_RELATIVE_MOVES    = yarp::os::createVocab32('r','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEED         = yarp::os::createVocab32('v','e','l');
constexpr yarp::conf::vocab32_t VOCAB_REF_SPEEDS        = yarp::os::createVocab32('v','e','l','s');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATION  = yarp::os::createVocab32('a','c','c');
constexpr yarp::conf::vocab32_t VOCAB_REF_ACCELERATIONS = yarp::os::createVocab32('a','c','c','s');
constexpr yarp::conf::vocab32_t VOCAB_STOP              = yarp::os::createVocab32('s','t','o');
constexpr yarp::conf::vocab32_t VOCAB_STOPS             = yarp::os::createVocab32('s','t','o','s');

// interface IVelocityControl sets
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MODE  = yarp::os::createVocab32('v','e','l','d');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVE  = yarp::os::createVocab32('v','m','o');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_MOVES = yarp::os::createVocab32('v','m','o','s');

// protocol version
constexpr yarp::conf::vocab32_t VOCAB_PROTOCOL_VERSION = yarp::os::createVocab32('p', 'r', 'o', 't');

#endif // YARP_DEV_CONTROLBOARDVOCABS_H
