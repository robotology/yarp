// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcServer.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

RpcServer::RpcServer() {
    port.setInputMode(true);
    port.setOutputMode(false);
    port.setRpcMode(true);
}

RpcServer::~RpcServer() {
    port.close();
}

bool RpcServer::write(PortWriter& writer, PortWriter *callback) const {
    YARP_SPRINTF1(Logger::get(),error,"RpcServer %s cannot write, please use a regular Port or RpcClient for that",port.getName().c_str());
    return false;
}

bool RpcServer::write(PortWriter& writer, PortReader& reader,
                                PortWriter *callback) const {
    YARP_SPRINTF1(Logger::get(),error,"RpcServer %s cannot write, please use a regular Port or RpcClient for that",port.getName().c_str());
    return false;
}

bool RpcServer::read(PortReader& reader, bool willReply) {
    if (!willReply) {
        YARP_SPRINTF1(Logger::get(),error,"RpcServer %s must reply, please use a regular Port if you do not want to",port.getName().c_str());
        // this is an error for RpcServer
        return false;
    }
    return port.read(reader,true);
}

void RpcServer::setInputMode(bool expectInput) {
    YARP_ASSERT(expectInput);
}


void RpcServer::setOutputMode(bool expectOutput) {
    YARP_ASSERT(!expectOutput);
}


void RpcServer::setRpcMode(bool expectRpc) {
    YARP_ASSERT(expectRpc);
}

