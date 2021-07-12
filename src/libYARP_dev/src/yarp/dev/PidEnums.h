/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_PIDENUMS_H
#define YARP_DEV_PIDENUMS_H

#include <yarp/os/Vocab.h>
#include <yarp/conf/system.h>
#include <yarp/dev/api.h>

namespace yarp
{
    namespace dev
    {
        enum YARP_dev_API PidControlTypeEnum
        {
            VOCAB_PIDTYPE_POSITION = yarp::os::createVocab32('p', 'o', 's'),
            VOCAB_PIDTYPE_VELOCITY = yarp::os::createVocab32('v', 'e', 'l'),
            VOCAB_PIDTYPE_TORQUE   = yarp::os::createVocab32('t', 'r', 'q'),
            VOCAB_PIDTYPE_CURRENT  = yarp::os::createVocab32('c', 'u', 'r')
        };

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
    }
}

#endif // YARP_DEV_PIDENUMS_H
