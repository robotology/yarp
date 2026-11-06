/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

/**
 * Commands accepted by the port opened by yarp::os::Terminee, and sent by
 * yarp::os::Terminator::terminateByName().
 * They can also be sent with "yarp rpc /portname/quit".
 */
service TerminatorMsgs
{
    /**
     * Ask the process which owns the port to start a graceful quit procedure.
     * @return true if the request has been accepted
     */
    bool quit();
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
