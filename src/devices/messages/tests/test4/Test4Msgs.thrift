/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

include "Test4IncMsgs.thrift"

// ---------------------------------------------------------------

service Test4Msgs
{
  Test4IncMsgs.customreturn   setMethod4(1:i32 param4);
}
