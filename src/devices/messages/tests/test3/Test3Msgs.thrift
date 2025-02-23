/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

include "../test1/Test1Msgs.thrift"

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

// ---------------------------------------------------------------

service Test3Msgs
{
  yReturnValue              setMethod3a(1:i32 param3a);
  Test1Msgs.yReturnValue    setMethod3b(1:i32 param3b);
}
