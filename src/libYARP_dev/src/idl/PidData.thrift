/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct PidData
{
  /** proportional gain */
  1: double kp;
  /** derivative gain */
  2: double kd;
  /** integrative gain */
  3: double ki;

  /** saturation threshold for the integrator */
  4: double max_int;
  /** scale for the pid output */
  5: double scale;
  /** max output */
  6: double max_output;

  /** pwm offset added to the pid output */
  7: double offset;
  /** up stiction offset added to the pid output */
  8: double stiction_up_val;
  /** down stiction offset added to the pid output */
  9: double stiction_down_val;
  /** feedforward gain */
  10: double kff;

  /** name/description of the pid */
  11: string name;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
