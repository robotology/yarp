/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_NAMESERVERCONNECTIONHANDLER_INC
#define YARPDB_NAMESERVERCONNECTIONHANDLER_INC

#include <yarp/name/api.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <yarp/name/NameService.h>

#include <stdio.h>

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

    virtual bool read(yarp::os::ConnectionReader& reader) {
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
        ok = service->apply(cmd,reply,event,remote);
        for (int i=0; i<event.size(); i++) {
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
                for (int i=1; i<reply.size(); i++) {
                    yarp::os::Value& v = reply.get(i);
                    if (v.isList()) {
                        yarp::os::ConstString si = v.asList()->toString();
                        char *buf = (char*)si.c_str();
                        size_t idx = 0;
                        // old name server messages don't have quotes,
                        // so we strip them.
                        for (size_t i=0; i<si.length(); i++) {
                            if (si[i]!='\"') {
                                if (idx!=i) {
                                    buf[idx] = si[i];
                                }
                                idx++;
                            }
                        }
                        yarp::os::ConstString so(si.c_str(),idx);
                        if (so.length()>0) {
                            writer->appendString(so.c_str());
                        }
                    } else {
                        if (v.isString()) {
                            writer->appendString(v.asString().c_str());
                        } else {
                            yarp::os::Bottle b;
                            b.add(v);
                            b.write(*writer);
                        }
                    }
                }
                writer->appendString("*** end of message");
            } else {
                reply.write(*writer);
            }
         }
        return true;
    }
};


#endif


