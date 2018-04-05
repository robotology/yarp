/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_PIDENUMS_H
#define YARP_DEV_PIDENUMS_H

#include <yarp/os/Vocab.h>

namespace yarp
{
    namespace dev
    {
        enum YARP_dev_API PidControlTypeEnum
        {
            VOCAB_PIDTYPE_POSITION = VOCAB3('p', 'o', 's'),
            VOCAB_PIDTYPE_VELOCITY = VOCAB3('v', 'e', 'l'),
            VOCAB_PIDTYPE_TORQUE   = VOCAB3('t', 'r', 'q'),
            VOCAB_PIDTYPE_CURRENT  = VOCAB3(99/*'c'*/, 'u', 'r') // SWIG bug
        };

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

    }
}

#endif // YARP_DEV_PIDENUMS_H
