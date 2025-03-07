/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

include "../test1/Test1Msgs.thrift"

// ---------------------------------------------------------------

service Test2Msgs
{
  Test1Msgs.yReturnValue    setMethod2(1:i32 param2);
}
