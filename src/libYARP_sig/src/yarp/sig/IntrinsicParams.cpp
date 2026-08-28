/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/Vector.h>

using namespace yarp::sig;

IntrinsicParams::IntrinsicParams()
{
    rectificationMatrix3X3 = {1.0, 0.0, 0.0,
                              0.0, 1.0, 0.0,
                              0.0, 0.0, 1.0};
}
