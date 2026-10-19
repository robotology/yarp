/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.os

include "SystemInfoData.thrift"
include "ThreadInfoData.thrift"
include "PortInfoData.thrift"

/**
 * Reply of PortCoreMsgs::listConnectionsIn() and PortCoreMsgs::listConnectionsOut().
 */
struct return_listConnections
{
    1: bool         ret;            // false if the request could not be processed
    2: list<string> connections;    // names of the connected ports
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

/**
 * Reply of PortCoreMsgs::getConnectionInfoIn() and PortCoreMsgs::getConnectionInfoOut().
 */
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

/**
 * Reply of PortCoreMsgs::getAdminProtocolVersion().
 */
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

/**
 * Reply of PortCoreMsgs::addConnection().
 */
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

/**
 * Reply of PortCoreMsgs::delConnection().
 */
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

/**
 * Reply of PortCoreMsgs::setPortmonitorParamsIn() and PortCoreMsgs::setPortmonitorParamsOut().
 */
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

/**
 * Reply of PortCoreMsgs::getPortmonitorParamsIn() and PortCoreMsgs::getPortmonitorParamsOut().
 */
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

/**
 * Reply of PortCoreMsgs::setConnectionParamsIn() and PortCoreMsgs::setConnectionParamsOut().
 */
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

/**
 * Reply of PortCoreMsgs::getConnectionParamsIn() and PortCoreMsgs::getConnectionParamsOut().
 */
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

/**
 * Reply of PortCoreMsgs::getPortInfo().
 */
struct return_getPortInfo
{
    1: bool                            ret;        // false if the request could not be processed
    2: SystemInfoData.ProcessInfoData  process;    // the process which owns the port
    3: SystemInfoData.PlatformInfoData platform;   // the platform on which the process is running
    4: ThreadInfoData.ThreadInfoData   thread;     // the main thread of the port
    5: PortInfoData.PortInfoData       port;       // the port itself
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

/**
 * Reply of PortCoreMsgs::getConnectionQosIn() and PortCoreMsgs::getConnectionQosOut().
 */
struct return_getConnectionQos
{
    1: bool                          ret;      // false if the request could not be processed
    2: bool                          found;    // false if there is no such connection
    3: ThreadInfoData.ThreadInfoData thread;   // the thread which handles the connection (tid, priority, policy)
    4: i32                           qos_tos;  // the type of service of the packets of the connection (-1 if not available)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

/**
 * Reply of PortCoreMsgs::setConnectionQosIn() and PortCoreMsgs::setConnectionQosOut().
 */
struct return_setConnectionQos
{
    1: bool   ret;          // false if the request could not be processed
    2: bool   success;      // true if the qos of the connection has been set
    3: string message;      // a human readable description of the error (if any)
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)

/**
 * Administrative commands of a YARP port, handled by PortCore::adminBlock().
 * They can be sent with "yarp admin rpc /portname", or from code using
 * yarp::os::impl::PortCoreClient.
 */
service PortCoreMsgs
{
    /**
     * Add an output connection from this port to another port.
     * @param portname the destination port of the connection
     * @param carrier the carrier of the connection, e.g. tcp, udp, fast_tcp (optional, the default carrier is used if empty)
     * @return success: true if the connection has been added; message: a description of the result
     */
    return_addConnection addConnection(1: string portname, 2: string carrier = "");

    /**
     * Remove all the input and output connections between this port and another port.
     * @param portname the port at the other end of the connection(s)
     * @return success: true if at least one connection has been removed; message: a description of the result
     */
    return_delConnection delConnection(1: string portname);

    /**
     * Attach a portmonitor plug-in to the output of this port (it replaces the one previously attached, if any).
     * @param prop the portmonitor configuration, in yarp::os::Property format, e.g. "(type dll) (file test_numbers1)" or "(context yarpviz) (file portrate)"
     * @return true if the portmonitor has been attached
     */
    bool attachPortmonitorOut(1: string prop);

    /**
     * Attach a portmonitor plug-in to the input of this port (it replaces the one previously attached, if any).
     * @param prop the portmonitor configuration, in yarp::os::Property format, e.g. "(type dll) (file test_numbers1)" or "(context yarpviz) (file portrate)"
     * @return true if the portmonitor has been attached
     */
    bool attachPortmonitorIn(1: string prop);

    /**
     * Detach the portmonitor plug-in from the output of this port.
     * @return true (also if no portmonitor was attached)
     */
    bool detachPortmonitorOut();

    /**
     * Detach the portmonitor plug-in from the input of this port.
     * @return true (also if no portmonitor was attached)
     */
    bool detachPortmonitorIn();

    /**
     * Set the parameters of the portmonitor plug-in attached to the output of this port.
     * @param params the parameters, in yarp::os::Property format, e.g. "(log_save 1)"; their meaning depends on the portmonitor
     * @return success: false if no portmonitor is attached; message: a description of the error (if any)
     */
    return_setPortmonitorParams setPortmonitorParamsOut(1: string params);

    /**
     * Set the parameters of the portmonitor plug-in attached to the input of this port.
     * @param params the parameters, in yarp::os::Property format, e.g. "(log_save 1)"; their meaning depends on the portmonitor
     * @return success: false if no portmonitor is attached; message: a description of the error (if any)
     */
    return_setPortmonitorParams setPortmonitorParamsIn(1: string params);

    /**
     * Get the parameters of the portmonitor plug-in attached to the output of this port.
     * @return success: false if no portmonitor is attached; params: the parameters, in yarp::os::Property format; message: a description of the error (if any)
     */
    return_getPortmonitorParams getPortmonitorParamsOut();

    /**
     * Get the parameters of the portmonitor plug-in attached to the input of this port.
     * @return success: false if no portmonitor is attached; params: the parameters, in yarp::os::Property format; message: a description of the error (if any)
     */
    return_getPortmonitorParams getPortmonitorParamsIn();

    /**
     * Set the carrier parameters of the output connection going from this port to another port.
     * @param portname the destination port of the connection
     * @param params the parameters, in yarp::os::Property format; their meaning depends on the carrier of the connection
     * @return success: false if there is no such connection; message: a description of the result
     */
    return_setConnectionParams setConnectionParamsOut(1: string portname, 2: string params);

    /**
     * Set the carrier parameters of the input connection coming from another port to this port.
     * @param portname the source port of the connection
     * @param params the parameters, in yarp::os::Property format; their meaning depends on the carrier of the connection
     * @return success: false if there is no such connection; message: a description of the result
     */
    return_setConnectionParams setConnectionParamsIn(1: string portname, 2: string params);

    /**
     * Get the carrier parameters of the output connection going from this port to another port.
     * @param portname the destination port of the connection
     * @return success: false if there is no such connection; params: the parameters, in yarp::os::Property format; message: a description of the error (if any)
     */
    return_getConnectionParams getConnectionParamsOut(1: string portname);

    /**
     * Get the carrier parameters of the input connection coming from another port to this port.
     * @param portname the source port of the connection
     * @return success: false if there is no such connection; params: the parameters, in yarp::os::Property format; message: a description of the error (if any)
     */
    return_getConnectionParams getConnectionParamsIn(1: string portname);

    /**
     * List the ports connected to the input of this port.
     * @return connections: the names of the source ports of the input connections
     */
    return_listConnections listConnectionsIn();

    /**
     * List the ports connected to the output of this port.
     * @return connections: the names of the destination ports of the output connections
     */
    return_listConnections listConnectionsOut();

    /**
     * Give details about the input connection coming from another port to this port.
     * @param portname the source port of the connection
     * @return found: false if there is no such connection; from_port, to_port: the ends of the connection; carrier: the carrier of the connection; connectionless: true if the carrier is connectionless (e.g. udp); push: false if the carrier is reversed (pull)
     */
    return_getConnectionInfo getConnectionInfoIn(1: string portname);

    /**
     * Give details about the output connection going from this port to another port.
     * @param portname the destination port of the connection
     * @return found: false if there is no such connection; from_port, to_port: the ends of the connection; carrier: the carrier of the connection; connectionless: true if the carrier is connectionless (e.g. udp); push: false if the carrier is reversed (pull)
     */
    return_getConnectionInfo getConnectionInfoOut(1: string portname);

    /**
     * Report the version of the administrative protocol of this port (it is distinct from the version of the YARP library).
     * @return version_major, version_minor, version_patch: the version of the protocol
     */
    return_getAdminProtocolVersion getAdminProtocolVersion();

    /**
     * Give information about this port, its main thread, and the process and the platform on which it is running.
     * @return process: name, arguments, scheduling policy and priority, pid of the process; platform: name, distribution, release, codename, kernel, environment variables of the operating system; thread: tid, priority, policy of the main thread of the port; port: is_input, is_output, is_rpc, type, hostname of the port
     */
    return_getPortInfo getPortInfo();

    /**
     * Get the quality of service of the input connection coming from another port to this port.
     * @param portname the source port of the connection
     * @return found: false if there is no such connection; thread: tid, scheduling priority and policy of the thread which handles the connection; qos_tos: the type of service of the packets of the connection (-1 if not available)
     */
    return_getConnectionQos getConnectionQosIn(1: string portname);

    /**
     * Get the quality of service of the output connection going from this port to another port.
     * @param portname the destination port of the connection
     * @return found: false if there is no such connection; thread: tid, scheduling priority and policy of the thread which handles the connection; qos_tos: the type of service of the packets of the connection (-1 if not available)
     */
    return_getConnectionQos getConnectionQosOut(1: string portname);

    /**
     * Set the quality of service of the input connection coming from another port to this port.
     * @param portname the source port of the connection
     * @param scheduler_priority the scheduling priority of the thread which handles the connection (OS specific; -1, together with scheduler_policy -1, for the default)
     * @param scheduler_policy the scheduling policy of the thread which handles the connection (OS specific, e.g. on Linux 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR)
     * @param qos_tos the type of service of the packets of the connection, e.g. LOW = 40, NORMAL = 0, HIGH = 144, CRITICAL = 176 (a negative value leaves it unchanged)
     * @return success: false if there is no such connection or if the qos could not be set; message: a description of the error (if any)
     */
    return_setConnectionQos setConnectionQosIn(1: string portname, 2: i32 scheduler_priority, 3: i32 scheduler_policy, 4: i32 qos_tos);

    /**
     * Set the quality of service of the output connection going from this port to another port.
     * @param portname the destination port of the connection
     * @param scheduler_priority the scheduling priority of the thread which handles the connection (OS specific; -1, together with scheduler_policy -1, for the default)
     * @param scheduler_policy the scheduling policy of the thread which handles the connection (OS specific, e.g. on Linux 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR)
     * @param qos_tos the type of service of the packets of the connection, e.g. LOW = 40, NORMAL = 0, HIGH = 144, CRITICAL = 176 (a negative value leaves it unchanged)
     * @return success: false if there is no such connection or if the qos could not be set; message: a description of the error (if any)
     */
    return_setConnectionQos setConnectionQosOut(1: string portname, 2: i32 scheduler_priority, 3: i32 scheduler_policy, 4: i32 qos_tos);

    /**
     * Set the scheduling parameters of all the threads of the process which owns this port.
     * @param priority the scheduling priority (OS specific)
     * @param policy the scheduling policy (OS specific, e.g. on Linux 0 = SCHED_OTHER, 1 = SCHED_FIFO, 2 = SCHED_RR)
     * @return true if the scheduling parameters have been set
     */
    bool setProcessScheduling(1: i32 priority, 2: i32 policy);
}
(
    yarp.api.include = "yarp/os/api.h"
    yarp.api.keyword = "YARP_os_API"
)
