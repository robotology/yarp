/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/RpcClient.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;


RpcClient::RpcClient() {
    port.setInputMode(false);
    port.setOutputMode(true);
    port.setRpcMode(true);
}

RpcClient::~RpcClient() {
    port.close();
}

bool RpcClient::read(PortReader& reader, bool willReply) {
    YARP_SPRINTF1(Logger::get(),error,"cannot read from RpcClient %s, please use a regular Port for that",port.getName().c_str());
    return false;
}

bool RpcClient::reply(PortWriter& writer) {
    return false;
}

bool RpcClient::replyAndDrop(PortWriter& writer) {
    return false;
}

void RpcClient::setInputMode(bool expectInput) {
    yAssert(!expectInput);
}


void RpcClient::setOutputMode(bool expectOutput) {
    yAssert(expectOutput);
}


void RpcClient::setRpcMode(bool expectRpc) {
    yAssert(expectRpc);
}

