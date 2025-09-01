/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct PidDataUnits
{  /** proportional gain */
  1: string units_kp;
  /** derivative gain */
  2: string units_kd;
  /** integrative gain */
  3: string units_ki;

  /** saturation threshold for the integrator */
  4: string units_max_int;
  /** scale for the pid output */
  5: string units_scale;
  /** max output */
  6: string units_max_output;

  /** pwm offset added to the pid output */
  7: string units_offset;
  /** up stiction offset added to the pid output */
  8: string units_stiction_up_val;
  /** down stiction offset added to the pid output */
  9: string units_stiction_down_val;
  /** feedforward gain */
  10: string units_kff;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct PidDataDescription
{
  /** human-readable description of the pid controller. */
  1: string pid_description;
  /** human-readable description of the input data of pid */
  2: string input_data_description;
  /** human-readable description of the output data of pid */
  3: string output_data_description;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct PidDataExtraInfo
{
   1:PidDataDescription description;
   2:PidDataUnits       units;
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)

struct PidDataGains
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
} (
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
