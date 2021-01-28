/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_GENERIC_VOCABS_H
#define YARP_DEV_GENERIC_VOCABS_H

#include <yarp/os/Vocab.h>
// Common
constexpr yarp::conf::vocab32_t VOCAB_SET        = yarp::os::createVocab('s','e','t');
constexpr yarp::conf::vocab32_t VOCAB_GET        = yarp::os::createVocab('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IS         = yarp::os::createVocab('i','s');
constexpr yarp::conf::vocab32_t VOCAB_OK         = yarp::os::createVocab('o','k');
constexpr yarp::conf::vocab32_t VOCAB_FAILED     = yarp::os::createVocab('f','a','i','l');
constexpr yarp::conf::vocab32_t VOCAB_ERR        = yarp::os::createVocab('e','r','r');
constexpr yarp::conf::vocab32_t VOCAB_ERRS       = yarp::os::createVocab('e','r','r','s');
constexpr yarp::conf::vocab32_t VOCAB_HELP       = yarp::os::createVocab('h','e','l','p');
constexpr yarp::conf::vocab32_t VOCAB_QUIT       = yarp::os::createVocab('q','u','i','t');
constexpr yarp::conf::vocab32_t VOCAB_NOT        = yarp::os::createVocab('n','o','t');
constexpr yarp::conf::vocab32_t VOCAB_REMOVE     = yarp::os::createVocab('r','m');
constexpr yarp::conf::vocab32_t VOCAB_OFFSET     = yarp::os::createVocab('o', 'f', 'f');
constexpr yarp::conf::vocab32_t VOCAB_REF        = yarp::os::createVocab('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_REFS       = yarp::os::createVocab('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_REFG       = yarp::os::createVocab('r','e','f','g');
constexpr yarp::conf::vocab32_t VOCAB_LIM        = yarp::os::createVocab('l','i','m');
constexpr yarp::conf::vocab32_t VOCAB_LIMS       = yarp::os::createVocab('l','i','m','s');
constexpr yarp::conf::vocab32_t VOCAB_RESET      = yarp::os::createVocab('r','e','s');
constexpr yarp::conf::vocab32_t VOCAB_DISABLE    = yarp::os::createVocab('d','i','s');
constexpr yarp::conf::vocab32_t VOCAB_ENABLE     = yarp::os::createVocab('e','n','a');
constexpr yarp::conf::vocab32_t VOCAB_OUTPUT     = yarp::os::createVocab('o','u','t');
constexpr yarp::conf::vocab32_t VOCAB_OUTPUTS    = yarp::os::createVocab('o','u','t','s');
constexpr yarp::conf::vocab32_t VOCAB_REFERENCE  = yarp::os::createVocab('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_REFERENCES = yarp::os::createVocab('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_AXES       = yarp::os::createVocab('a','x','e','s');
constexpr yarp::conf::vocab32_t VOCAB_COUNT      = yarp::os::createVocab('c','n','t');
constexpr yarp::conf::vocab32_t VOCAB_VALUE      = yarp::os::createVocab('v','a','l');

// Image, matrix etc
constexpr yarp::conf::vocab32_t VOCAB_WIDTH      = yarp::os::createVocab('w');
constexpr yarp::conf::vocab32_t VOCAB_HEIGHT     = yarp::os::createVocab('h');

#endif // YARP_DEV_GENERIC_VOCABS_H
