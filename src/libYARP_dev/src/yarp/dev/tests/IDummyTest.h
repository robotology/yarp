/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IDUMMYTEST_H
#define IDUMMYTEST_H

#include <yarp/dev/api.h>

namespace yarp::dev::tests
{
    inline bool YARP_dev_API exec_dummy_test()
    {
        bool b = true;
        return b;
    }
}

#endif
