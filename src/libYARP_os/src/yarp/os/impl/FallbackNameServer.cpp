/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/FallbackNameServer.h>

#include <yarp/conf/system.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameServer.h>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(FALLBACKNAMESERVER, "yarp.os.impl.FallbackNameServer")
} // namespace


Contact FallbackNameServer::getAddress()
{
    return Contact("fallback",
                   "mcast",
                   "224.2.1.1",
                   NetworkBase::getDefaultPortRange());
}


void FallbackNameServer::run()
{
    DgramTwoWayStream send;
    send.join(getAddress(), true);
    listen.join(getAddress(), false);

    yCDebug(FALLBACKNAMESERVER, "Fallback server running");
    while (listen.isOk() && send.isOk() && !closed) {
        yCDebug(FALLBACKNAMESERVER, "Fallback server waiting");
        std::string msg;
        listen.beginPacket();
        msg = listen.readLine();
        listen.endPacket();
        yCDebug(FALLBACKNAMESERVER, "Fallback server got something");
        if (listen.isOk() && !closed) {
            yCDebug(FALLBACKNAMESERVER, "Fallback server got %s", msg.c_str());
            if (msg.find("NAME_SERVER ") == 0) {
                Contact addr;
                std::string result = owner.apply(msg, addr);
                send.beginPacket();
                send.writeLine(result.c_str(), (int)result.length());
                send.flush();
                send.endPacket();
            }
        }
    }
}


void FallbackNameServer::close()
{
    closed = true;
    listen.interrupt();
}
