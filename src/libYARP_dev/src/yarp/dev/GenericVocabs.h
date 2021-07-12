/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_GENERIC_VOCABS_H
#define YARP_DEV_GENERIC_VOCABS_H

#include <yarp/os/Vocab.h>
// Common
constexpr yarp::conf::vocab32_t VOCAB_SET        = yarp::os::createVocab32('s','e','t');
constexpr yarp::conf::vocab32_t VOCAB_GET        = yarp::os::createVocab32('g','e','t');
constexpr yarp::conf::vocab32_t VOCAB_IS         = yarp::os::createVocab32('i','s');
constexpr yarp::conf::vocab32_t VOCAB_OK         = yarp::os::createVocab32('o','k');
constexpr yarp::conf::vocab32_t VOCAB_FAILED     = yarp::os::createVocab32('f','a','i','l');
constexpr yarp::conf::vocab32_t VOCAB_ERR        = yarp::os::createVocab32('e','r','r');
constexpr yarp::conf::vocab32_t VOCAB_ERRS       = yarp::os::createVocab32('e','r','r','s');
constexpr yarp::conf::vocab32_t VOCAB_HELP       = yarp::os::createVocab32('h','e','l','p');
constexpr yarp::conf::vocab32_t VOCAB_QUIT       = yarp::os::createVocab32('q','u','i','t');
constexpr yarp::conf::vocab32_t VOCAB_NOT        = yarp::os::createVocab32('n','o','t');
constexpr yarp::conf::vocab32_t VOCAB_REMOVE     = yarp::os::createVocab32('r','m');
constexpr yarp::conf::vocab32_t VOCAB_OFFSET     = yarp::os::createVocab32('o', 'f', 'f');
constexpr yarp::conf::vocab32_t VOCAB_REF        = yarp::os::createVocab32('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_REFS       = yarp::os::createVocab32('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_REFG       = yarp::os::createVocab32('r','e','f','g');
constexpr yarp::conf::vocab32_t VOCAB_LIM        = yarp::os::createVocab32('l','i','m');
constexpr yarp::conf::vocab32_t VOCAB_LIMS       = yarp::os::createVocab32('l','i','m','s');
constexpr yarp::conf::vocab32_t VOCAB_RESET      = yarp::os::createVocab32('r','e','s');
constexpr yarp::conf::vocab32_t VOCAB_DISABLE    = yarp::os::createVocab32('d','i','s');
constexpr yarp::conf::vocab32_t VOCAB_ENABLE     = yarp::os::createVocab32('e','n','a');
constexpr yarp::conf::vocab32_t VOCAB_OUTPUT     = yarp::os::createVocab32('o','u','t');
constexpr yarp::conf::vocab32_t VOCAB_OUTPUTS    = yarp::os::createVocab32('o','u','t','s');
constexpr yarp::conf::vocab32_t VOCAB_REFERENCE  = yarp::os::createVocab32('r','e','f');
constexpr yarp::conf::vocab32_t VOCAB_REFERENCES = yarp::os::createVocab32('r','e','f','s');
constexpr yarp::conf::vocab32_t VOCAB_AXES       = yarp::os::createVocab32('a','x','e','s');
constexpr yarp::conf::vocab32_t VOCAB_COUNT      = yarp::os::createVocab32('c','n','t');
constexpr yarp::conf::vocab32_t VOCAB_VALUE      = yarp::os::createVocab32('v','a','l');

// Image, matrix etc
constexpr yarp::conf::vocab32_t VOCAB_WIDTH      = yarp::os::createVocab32('w');
constexpr yarp::conf::vocab32_t VOCAB_HEIGHT     = yarp::os::createVocab32('h');

#endif // YARP_DEV_GENERIC_VOCABS_H
