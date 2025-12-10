/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

const i16  protocol_version = 1

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

//-------------------------------------------------

service FakeTestMsgs
{
    yReturnValue doSomethingRPC ();
    yReturnValue doSomethingUnimplementedRPC ();
}
