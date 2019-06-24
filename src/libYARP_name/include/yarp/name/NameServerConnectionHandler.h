/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPDB_NAMESERVERCONNECTIONHANDLER_INC
#define YARPDB_NAMESERVERCONNECTIONHANDLER_INC

#include <yarp/name/api.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <yarp/name/NameService.h>

#include <cstdio>
#include <string>
#include <algorithm>

namespace yarp {
    namespace name {
        class NameServerConnectionHandler;
    }
}


/**
 *
 * Manage a single connection to the name server.
 *
 */
class yarp::name::NameServerConnectionHandler : public yarp::os::PortReader {
private:
    NameService *service;
public:
    NameServerConnectionHandler(NameService *service) {
        this->service = service;
    }

    bool read(yarp::os::ConnectionReader& reader) override {
        return apply(reader,0/*NULL*/);
    }

    virtual bool apply(yarp::os::ConnectionReader& reader,
                       yarp::os::ConnectionWriter *writer,
                       bool lock = true) {
        yarp::os::Bottle cmd, reply, event;
        bool ok = cmd.read(reader);
        if (!ok) return false;
        yarp::os::Contact remote;
        remote = reader.getRemoteContact();
        if (lock) service->lock();
        service->apply(cmd,reply,event,remote);
        for (size_t i=0; i<event.size(); i++) {
            yarp::os::Bottle *e = event.get(i).asList();
            if (e!=0/*NULL*/) {
                service->onEvent(*e);
            }
        }
        if (lock) service->unlock();
        if (writer==0/*NULL*/) {
            writer = reader.getWriter();
        }
        if (writer!=0/*NULL*/) {
            //printf("sending reply %s\n", reply.toString().c_str());
            if (reply.get(0).toString()=="old") {
                // support old name server messages
                for (size_t i=1; i<reply.size(); i++) {
                    yarp::os::Value& v = reply.get(i);
                    if (v.isList()) {
                        // old name server messages don't have quotes,
                        // so we strip them.
                        std::string si = v.asList()->toString();
                        si.erase(std::remove(si.begin(), si.end(), '\"'), si.end());
                        if (si.length()>0) {
                            writer->appendText(si.c_str());
                        }
                    } else {
                        if (v.isString()) {
                            writer->appendText(v.asString().c_str());
                        } else {
                            yarp::os::Bottle b;
                            b.add(v);
                            b.write(*writer);
                        }
                    }
                }
                writer->appendText("*** end of message");
            } else {
                reply.write(*writer);
            }
         }
        return true;
    }
};


#endif


