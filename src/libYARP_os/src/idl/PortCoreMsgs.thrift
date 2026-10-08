/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

// Administrative commands handled by PortCore::adminBlock().
service PortCoreMsgs
{
    // Attach a portmonitor plug-in to the port's output
    // prop: the portmonitor configuration, e.g. "(context yarpviz) (file portrate)"
    bool attachPortmonitorOut(1: string prop);

    // Attach a portmonitor plug-in to the port's input
    // prop: the portmonitor configuration, e.g. "(context yarpviz) (file portrate)"
    bool attachPortmonitorIn(1: string prop);

    // Detach the portmonitor plug-in from the port's output
    bool detachPortmonitorOut();

    // Detach the portmonitor plug-in from the port's input
    bool detachPortmonitorIn();
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
