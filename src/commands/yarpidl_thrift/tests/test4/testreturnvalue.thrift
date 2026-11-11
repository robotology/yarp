/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fixture used to test the value returned by the generated client methods
// when the communication fails: every yarp::dev::ReturnValue must be set to
// return_value_error_nws_nwc_communication_error.

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

// Struct with a single ReturnValue field
struct return_getValue {
  1: yReturnValue ret;
  2: i32 value;
}

// Struct with more than one ReturnValue field
struct return_getTwoValues {
  1: yReturnValue ret;
  2: yReturnValue ret2;
  3: string text;
}

service TestReturnValue
{
     // Case 1: yarp::dev::ReturnValue returned directly
     yReturnValue setValueRPC(1: i32 value);

     // Case 2: struct containing yarp::dev::ReturnValue fields
     return_getValue getValueRPC();
     return_getTwoValues getTwoValuesRPC();

     // Case 3: no yarp::dev::ReturnValue involved (default value returned)
     bool getBoolRPC();
}
