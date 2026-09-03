/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IJoypadController.h>

using namespace yarp::dev;

IJoypadController::~IJoypadController() = default;

ReturnValue IJoypadController::getAllAxes(std::vector<double>& values)
{
    size_t num_of_axes = 0;
    ReturnValue ret = this->getAxisCount(num_of_axes);
    if (!ret)
    {
        return ret;
    }
    for (size_t i = 0; i < num_of_axes; i++)
    {
        double value = 0.0;
        ret = this->getAxis(i, value);
        values.push_back(value);
    }

    return ReturnValue::ReturnValue_ok;
}
