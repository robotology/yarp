/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>

#include <yarp/os/LogComponent.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/FallbackNameClient.h>
#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(FALLBACKNAMECLIENT,
                   "yarp.os.impl.FallbackNameClient",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::defaultPrintCallback(),
                   nullptr)
} // namespace

void FallbackNameClient::run()
{
    NameConfig nc;
    Contact call = FallbackNameServer::getAddress();
    DgramTwoWayStream send;
    send.join(call, true);
    listen.join(call, false);
    if (!listen.isOk()) {
        yCError(FALLBACKNAMECLIENT, "Multicast not available");
        return;
    }
    std::string msg = std::string("NAME_SERVER query ") + nc.getNamespace();
    send.beginPacket();
    send.writeLine(msg.c_str(), (int)msg.length());
    send.flush();
    send.endPacket();
    for (int i = 0; i < 5; i++) {
        listen.beginPacket();
        std::string txt = listen.readLine();
        listen.endPacket();
        if (closed) {
            return;
        }
        yCDebug(FALLBACKNAMECLIENT, "Fallback name client got %s", txt.c_str());
        if (txt.find("registration ") == 0) {
            address = NameClient::extractAddress(txt);
            yCInfo(FALLBACKNAMECLIENT, "Received address %s", address.toURI().c_str());
            return;
        }
    }
}


void FallbackNameClient::close()
{
    if (!closed) {
        closed = true;
        listen.interrupt();
        listen.close();
        join();
    }
}


Contact FallbackNameClient::getAddress()
{
    return address;
}


Contact FallbackNameClient::seek()
{
    int tries = 3;
    for (int k = 0; k < tries; k++) {

        FallbackNameClient seeker;

        yCInfo(FALLBACKNAMECLIENT, "Polling for name server (using multicast), try %d of max %d", k + 1, tries);

        seeker.start();
        SystemClock::delaySystem(0.25);
        if (seeker.getAddress().isValid()) {
            return seeker.getAddress();
        }
        int len = 20;
        for (int i0 = 0; i0 < len; i0++) {
            fprintf(stderr, "++");
        }
        fprintf(stderr, "\n");

        for (int i = 0; i < len; i++) {
            SystemClock::delaySystem(0.025);
            fprintf(stderr, "++");
            if (seeker.getAddress().isValid()) {
                fprintf(stderr, "\n");
                return seeker.getAddress();
            }
        }
        fprintf(stderr, "\n");
        yCInfo(FALLBACKNAMECLIENT, "No response to search for server");
        seeker.close();
        seeker.join();
    }
    return Contact();
}
