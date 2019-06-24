/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
 }
