/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Translation.h>

#include <yarp/os/LogComponent.h>

namespace {
YARP_LOG_COMPONENT(TRANSLATION, "yarp.math.Translation")
}

void yarp::math::Translation::set(double x, double y, double z)
{
    t_x = x;
    t_y = y;
    t_z = z;
}
