/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_PIDENUMS_H
#define YARP_DEV_PIDENUMS_H

#include <yarp/os/Vocab.h>
#include <yarp/conf/system.h>
#include <yarp/dev/api.h>

namespace yarp::dev {
enum class YARP_dev_API PidControlTypeEnum
{
    VOCAB_PIDTYPE_POSITION   = yarp::os::createVocab32('p', 'o', 's','1'),
    VOCAB_PIDTYPE_POSITION_1 = yarp::os::createVocab32('p', 'o', 's','1'),
    VOCAB_PIDTYPE_POSITION_2 = yarp::os::createVocab32('p', 'o', 's','2'),
    VOCAB_PIDTYPE_POSITION_3 = yarp::os::createVocab32('p', 'o', 's','3'),
    VOCAB_PIDTYPE_VELOCITY   = yarp::os::createVocab32('v', 'e', 'l','1'),
    VOCAB_PIDTYPE_VELOCITY_1 = yarp::os::createVocab32('v', 'e', 'l','1'),
    VOCAB_PIDTYPE_VELOCITY_2 = yarp::os::createVocab32('v', 'e', 'l','2'),
    VOCAB_PIDTYPE_VELOCITY_3 = yarp::os::createVocab32('v', 'e', 'l','3'),
    VOCAB_PIDTYPE_TORQUE     = yarp::os::createVocab32('t', 'r', 'q','1'),
    VOCAB_PIDTYPE_TORQUE_1   = yarp::os::createVocab32('t', 'r', 'q','1'),
    VOCAB_PIDTYPE_TORQUE_2   = yarp::os::createVocab32('t', 'r', 'q','2'),
    VOCAB_PIDTYPE_TORQUE_3   = yarp::os::createVocab32('t', 'r', 'q','3'),
    VOCAB_PIDTYPE_CURRENT    = yarp::os::createVocab32('c', 'u', 'r','1'),
    VOCAB_PIDTYPE_CURRENT_1  = yarp::os::createVocab32('c', 'u', 'r','1'),
    VOCAB_PIDTYPE_CURRENT_2  = yarp::os::createVocab32('c', 'u', 'r','2'),
    VOCAB_PIDTYPE_CURRENT_3  = yarp::os::createVocab32('c', 'u', 'r','3')
};

inline size_t PidControlTypeEnum2Index(PidControlTypeEnum type)
{
    int32_t vocab = static_cast<yarp::conf::vocab32_t>(type);
    char lastChar = static_cast<char>(vocab & 0xFF);
    if (lastChar >= '0' && lastChar <= '9')
        return lastChar - '1'; // returns 0 for '1', 1 for '2', etc.
    return 0;
}

YARP_WARNING_PUSH
YARP_DISABLE_CLASS_ENUM_API_WARNING
enum class YARP_dev_API PidFeedbackUnitsEnum
{
    RAW_MACHINE_UNITS = 0,
    METRIC = 1,
};

enum class YARP_dev_API PidOutputUnitsEnum
{
    RAW_MACHINE_UNITS = 0,
    DUTYCYCLE_PWM_PERCENT = 1,
    POSITION_METRIC = 2,
    VELOCITY_METRIC = 3,
    TORQUE_METRIC = 4,
    CURRENT_METRIC = 5
};
YARP_WARNING_POP
} // namespace yarp::dev

#endif // YARP_DEV_PIDENUMS_H
