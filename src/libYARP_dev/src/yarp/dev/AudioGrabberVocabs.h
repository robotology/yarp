/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_AUDIOGRABBERVOCABS_H
#define YARP_DEV_AUDIOGRABBERVOCABS_H

#include <yarp/os/Vocab.h>

constexpr yarp::conf::vocab32_t VOCAB_AUDIO_INTERFACE = yarp::os::createVocab32('s', 'n', 'd');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_START     = yarp::os::createVocab32('r', 'u', 'n');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_STOP      = yarp::os::createVocab32('s', 't', 'o', 'p');

#endif // YARP_DEV_AUDIOGRABBERVOCABS_H
