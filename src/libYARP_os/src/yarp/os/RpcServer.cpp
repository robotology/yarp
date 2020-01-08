/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RpcServer.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

RpcServer::RpcServer()
{
    port.setInputMode(true);
    port.setOutputMode(false);
    port.setRpcMode(true);
}

RpcServer::~RpcServer()
{
    port.close();
}

bool RpcServer::write(const PortWriter& writer, const PortWriter* callback) const
{
    YARP_UNUSED(writer);
    YARP_UNUSED(callback);
    YARP_SPRINTF1(Logger::get(), error, "RpcServer %s cannot write, please use a regular Port or RpcClient for that", port.getName().c_str());
    return false;
}

bool RpcServer::write(const PortWriter& writer,
                      PortReader& reader,
                      const PortWriter* callback) const
{
    YARP_UNUSED(writer);
    YARP_UNUSED(reader);
    YARP_UNUSED(callback);
    YARP_SPRINTF1(Logger::get(), error, "RpcServer %s cannot write, please use a regular Port or RpcClient for that", port.getName().c_str());
    return false;
}

bool RpcServer::read(PortReader& reader, bool willReply)
{
    if (!willReply) {
        YARP_SPRINTF1(Logger::get(), error, "RpcServer %s must reply, please use a regular Port if you do not want to", port.getName().c_str());
        // this is an error for RpcServer
        return false;
    }
    return port.read(reader, true);
}

void RpcServer::setInputMode(bool expectInput)
{
    yAssert(expectInput);
}


void RpcServer::setOutputMode(bool expectOutput)
{
    yAssert(!expectOutput);
}


void RpcServer::setRpcMode(bool expectRpc)
{
    yAssert(expectRpc);
}
