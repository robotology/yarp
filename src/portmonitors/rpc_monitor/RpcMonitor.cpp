/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RpcMonitor.h"

#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/CommandBottle.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Things.h>

#include <map>

// Monitor: log component
namespace {
YARP_LOG_COMPONENT(RPCMONITOR,
                   "yarp.portmonitor.rpcMonitor",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

} // namespace

// Monitor: create
bool RpcMonitor::create(const yarp::os::Property& options)
{
    sender = options.find("sender_side").asBool();
    source = options.find("source").asString();
    destination = options.find("destination").asString();
    const std::string source_port = (sender ? source : destination) + "/monitor";

    const std::string monitor_port = options.check("monitor_name", yarp::os::Value("/monitor")).asString();

    if (!sender) {
        yCError(RPCMONITOR, "Attaching on receiver side is not supported yet.");
        return false;
    }
    if (!port.open(source_port)) {
        yCError(RPCMONITOR, "Could not open port %s.", source_port.c_str());
        return false;
    }
    if (!yarp::os::Network::connect(source_port, monitor_port, "fast_tcp"))
    {
        yCError(RPCMONITOR, "Could not connect to port %s.", monitor_port.c_str());
        return false;
    }
    return true;
}

// Monitor: update
yarp::os::Things& RpcMonitor::update(yarp::os::Things& thing)
{
    yarp::os::Bottle msg;
    msg.addFloat64(yarp::os::SystemClock::nowSystem());
    msg.addString(source);
    msg.addString(destination);
    msg.addString("command");
    msg.addString(sender ? "client" : "server");
    msg.addString("RpcMonitor");
    auto& bcmd = msg.addList();
    auto& bargs [[maybe_unused]] = msg.addList();
    auto& breply [[maybe_unused]] = msg.addList();

    if (const auto* reply = thing.cast_as<yarp::os::CommandBottle>()) {
        bcmd.addString(reply->cmd.get(0).toString());
        yarp::os::Portable::copyPortable(reply->cmd, bargs);
    } else if (const auto* cmd = thing.cast_as<yarp::os::Bottle>()) {
        bcmd.addString(cmd->get(0).toString());
        yarp::os::Portable::copyPortable(*cmd, bargs);
    } else {
        yCWarning(RPCMONITOR) << "Sending unknown command";
        bcmd.addString("[unknown]");
    }
    yCDebug(RPCMONITOR, "Writing: %s", msg.toString().c_str());
    if (port.getOutputCount() > 0) {
        port.write(msg);
    }
    return thing;
}

// Monitor: updateReply
yarp::os::Things& RpcMonitor::updateReply(yarp::os::Things& thing)
{
    yarp::os::Bottle msg;
    msg.addFloat64(yarp::os::SystemClock::nowSystem());
    msg.addString(source);
    msg.addString(destination);
    msg.addString("reply");
    msg.addString(sender ? "rpc client" : "rpc server");
    msg.addString("RpcMonitor");
    auto& bcmd = msg.addList();
    auto& bargs [[maybe_unused]] = msg.addList();
    auto& breply [[maybe_unused]] = msg.addList();

    if (const auto* reply = thing.cast_as<yarp::os::CommandBottle>()) {
        bcmd.addString(reply->cmd.get(0).toString());
        yarp::os::Portable::copyPortable(reply->cmd, bargs);
        yarp::os::Portable::copyPortable(reply->reply, breply);
    } else if (const auto* reply = thing.cast_as<yarp::os::Bottle>()) {
        yarp::os::Portable::copyPortable(*reply, breply);
    } else {
        yCWarning(RPCMONITOR) << "Received unknown reply";
        bcmd.addString("[unknown]");
    }
    yCDebug(RPCMONITOR, "Writing: %s", msg.toString().c_str());
    if (port.getOutputCount() > 0) {
        port.write(msg);
    }
    return thing;
}
