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

struct return_getAdminProtocolVersion
{
    1: bool ret;                // false if the request could not be processed
    2: i32  version_major;
    3: i32  version_minor;
    4: i32  version_patch;
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_addConnection
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // true if the connection has been added
    3: string message;      // a human readable description of the result
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_delConnection
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // true if the connection has been removed
    3: string message;      // a human readable description of the result
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_setPortmonitorParams
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // false if no portmonitor is attached
    3: string message;      // a human readable description of the error (if any)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_getPortmonitorParams
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // false if no portmonitor is attached
    3: string params;       // the parameters, in yarp::os::Property format, e.g. "(key1 value1) (key2 value2)"
    4: string message;      // a human readable description of the error (if any)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_setConnectionParams
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // false if there is no such connection
    3: string message;      // a human readable description of the result
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

struct return_getConnectionParams
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // false if there is no such connection
    3: string params;       // the parameters, in yarp::os::Property format, e.g. "(key1 value1) (key2 value2)"
    4: string message;      // a human readable description of the error (if any)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

// Administrative commands handled by PortCore::adminBlock().
service PortCoreMsgs
{
    // Add an output connection to the port
    // portname: the destination port of the connection
    // carrier: the carrier of the connection (optional)
    return_addConnection addConnection(1: string portname, 2: string carrier = "");

    // Remove any input or output connection involving the given port
    // portname: the port at the other end of the connection(s)
    return_delConnection delConnection(1: string portname);

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

    // Set the parameters of the portmonitor plug-in attached to the port's output
    // params: the parameters, in yarp::os::Property format, e.g. "(log_save 1)"
    return_setPortmonitorParams setPortmonitorParamsOut(1: string params);

    // Set the parameters of the portmonitor plug-in attached to the port's input
    // params: the parameters, in yarp::os::Property format, e.g. "(log_save 1)"
    return_setPortmonitorParams setPortmonitorParamsIn(1: string params);

    // Get the parameters of the portmonitor plug-in attached to the port's output
    return_getPortmonitorParams getPortmonitorParamsOut();

    // Get the parameters of the portmonitor plug-in attached to the port's input
    return_getPortmonitorParams getPortmonitorParamsIn();

    // Set the carrier parameters of the output connection going to a given port
    // portname: the destination port of the connection
    // params: the parameters, in yarp::os::Property format
    return_setConnectionParams setConnectionParamsOut(1: string portname, 2: string params);

    // Set the carrier parameters of the input connection coming from a given port
    // portname: the source port of the connection
    // params: the parameters, in yarp::os::Property format
    return_setConnectionParams setConnectionParamsIn(1: string portname, 2: string params);

    // Get the carrier parameters of the output connection going to a given port
    // portname: the destination port of the connection
    return_getConnectionParams getConnectionParamsOut(1: string portname);

    // Get the carrier parameters of the input connection coming from a given port
    // portname: the source port of the connection
    return_getConnectionParams getConnectionParamsIn(1: string portname);

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

    // Report the version of the administrative protocol of the port.
    // It is distinct from the YARP library version.
    return_getAdminProtocolVersion getAdminProtocolVersion();
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
