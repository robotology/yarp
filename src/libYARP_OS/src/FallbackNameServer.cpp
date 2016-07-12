/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Bytes.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Contact FallbackNameServer::getAddress() {
    return Contact("fallback",
                   "mcast",
                   "224.2.1.1",
                   NetworkBase::getDefaultPortRange());
}


void FallbackNameServer::run() {
    DgramTwoWayStream send;
    send.join(getAddress(),true);
    listen.join(getAddress(),false);

    YARP_DEBUG(Logger::get(),"Fallback server running");
    while (listen.isOk()&&send.isOk()&&!closed) {
        YARP_DEBUG(Logger::get(),"Fallback server waiting");
        ConstString msg;
        listen.beginPacket();
        msg = listen.readLine();
        listen.endPacket();
        YARP_DEBUG(Logger::get(),"Fallback server got something");
        if (listen.isOk()&&!closed) {
            YARP_DEBUG(Logger::get(),ConstString("Fallback server got ") + msg);
            if (msg.find("NAME_SERVER ") == 0) {
                Contact addr;
                ConstString result = owner.apply(msg,addr);
                send.beginPacket();
                send.writeLine(result.c_str(),(int)result.length());
                send.flush();
                send.endPacket();
            }
        }
    }
}


void FallbackNameServer::close() {
    closed = true;
    listen.interrupt();
}



#else

int FallbackNameServerDummySymbol = 42;

#endif // YARP_HAS_ACE





