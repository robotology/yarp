/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/conf/system.h>

#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/FallbackNameClient.h>
#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>

using namespace yarp::os::impl;
using namespace yarp::os;

void FallbackNameClient::run()
{
    NameConfig nc;
    Contact call = FallbackNameServer::getAddress();
    DgramTwoWayStream send;
    send.join(call, true);
    listen.join(call, false);
    if (!listen.isOk()) {
        YARP_ERROR(Logger::get(), std::string("Multicast not available"));
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
        YARP_DEBUG(Logger::get(), std::string("Fallback name client got ") + txt);
        if (txt.find("registration ") == 0) {
            address = NameClient::extractAddress(txt);
            YARP_INFO(Logger::get(), std::string("Received address ") + address.toURI());
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

        YARP_INFO(Logger::get(),
                  std::string("Polling for name server (using multicast), try ") + NetType::toString(k + 1) + std::string(" of max ") + NetType::toString(tries));

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
        YARP_INFO(Logger::get(), "No response to search for server");
        seeker.close();
        seeker.join();
    }
    return Contact();
}
