/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev.impl

struct VectorOfDouble {
  1: list<double> content;
} (
  yarp.name = "yarp::sig::VectorOf<double>"
  yarp.includefile="yarp/sig/Vector.h"
)

struct VectorOfInt {
  1: list<i32> content;
} (
  yarp.name = "yarp::sig::VectorOf<int>"
  yarp.includefile="yarp/sig/Vector.h"
)

struct jointData
{
  1: VectorOfDouble jointPosition;
  2: bool jointPosition_isValid;
  3: VectorOfDouble jointVelocity;
  4: bool jointVelocity_isValid;
  5: VectorOfDouble jointAcceleration;
  6: bool jointAcceleration_isValid;
  7: VectorOfDouble motorPosition;
  8: bool motorPosition_isValid;
  9: VectorOfDouble motorVelocity;
  10: bool motorVelocity_isValid;
  11: VectorOfDouble motorAcceleration;
  12: bool motorAcceleration_isValid;
  13: VectorOfDouble torque;
  14: bool torque_isValid;
  15: VectorOfDouble pwmDutycycle;
  16: bool pwmDutycycle_isValid;
  17: VectorOfDouble current;
  18: bool current_isValid;
  19: VectorOfInt controlMode;
  20: bool controlMode_isValid;
  21: VectorOfInt interactionMode;
  22: bool interactionMode_isValid;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
