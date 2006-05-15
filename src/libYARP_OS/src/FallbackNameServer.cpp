// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


#include <yarp/FallbackNameServer.h>
#include <yarp/DgramTwoWayStream.h>
#include <yarp/NetType.h>
#include <yarp/NameServer.h>
#include <yarp/Bytes.h>

using namespace yarp;

const Address FallbackNameServer::mcastLastResort("224.2.1.1",10001,
                                                  "mcast","fallback");


void FallbackNameServer::run() {
    try {
        DgramTwoWayStream send;
        send.join(getAddress(),true);
        listen.join(getAddress(),false);
    
        YARP_DEBUG(Logger::get(),"Fallback server running");
        while (listen.isOk()&&send.isOk()&&!closed) {
            YARP_DEBUG(Logger::get(),"Fallback server waiting");
            String msg;
            try {
                listen.beginPacket();
                msg = NetType::readLine(listen);
                listen.endPacket();
                YARP_DEBUG(Logger::get(),"Fallback server got something");
                if (listen.isOk()&&!closed) {
                    YARP_DEBUG(Logger::get(),String("Fallback server got ") + msg);
                    if (msg.strstr("NAME_SERVER ") == 0) {
                        String result = owner.apply(msg);
                        //Bytes b((char*)(result.c_str()),result.length());
                        send.beginPacket();
                        send.writeLine(result);
                        send.flush();
                        send.endPacket();
                    }
                }
            } catch (IOException e) {
                YARP_DEBUG(Logger::get(),e.toString() + " <<< fallback exception");
                listen.endPacket();
            }
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<< major fallback exception");
        YARP_ERROR(Logger::get(), "cannot run multi-cast service for name server");
    }
}


void FallbackNameServer::close() {
    closed = true;
    listen.interrupt();
}






