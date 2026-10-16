/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

struct return_listConnections
{
    1: bool         ret;            // false if the request could not be processed
    2: list<string> connections;    // names of the connected ports
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_getConnectionInfo
{
    1: bool   ret;              // false if the request could not be processed
    2: bool   found;            // false if there is no such connection
    3: string from_port;        // source port of the connection
    4: string to_port;          // destination port of the connection
    5: string carrier;          // carrier used by the connection
    6: bool   connectionless;   // true if the carrier is connectionless
    7: bool   push;             // false if the carrier is reversed (i.e. pull)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

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

    // List the names of the ports connected to the port's input
    return_listConnections listConnectionsIn();

    // List the names of the ports connected to the port's output
    return_listConnections listConnectionsOut();

    // Give details about the input connection coming from a given port
    // portname: the source port of the connection
    return_getConnectionInfo getConnectionInfoIn(1: string portname);

    // Give details about the output connection going to a given port
    // portname: the destination port of the connection
    return_getConnectionInfo getConnectionInfoOut(1: string portname);
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
