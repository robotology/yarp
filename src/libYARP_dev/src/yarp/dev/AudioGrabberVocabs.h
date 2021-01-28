/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_AUDIOGRABBERVOCABS_H
#define YARP_DEV_AUDIOGRABBERVOCABS_H

#include <yarp/os/Vocab.h>

constexpr yarp::conf::vocab32_t VOCAB_AUDIO_INTERFACE = yarp::os::createVocab('s', 'n', 'd');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_START     = yarp::os::createVocab('r', 'u', 'n');
constexpr yarp::conf::vocab32_t VOCAB_AUDIO_STOP      = yarp::os::createVocab('s', 't', 'o', 'p');

#endif // YARP_DEV_AUDIOGRABBERVOCABS_H
