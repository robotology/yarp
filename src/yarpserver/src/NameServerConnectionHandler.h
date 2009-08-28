// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_NAMESERVERCONNECTIONHANDLER_INC
#define YARPDB_NAMESERVERCONNECTIONHANDLER_INC

#include <stdio.h>

#include <string>

#include <yarp/os/PortReader.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>

#include "NameService.h"

/**
 *
 * Manage a single connection to the name server.
 *
 */
class NameServerConnectionHandler : public yarp::os::PortReader {
private:
    NameService *service;
public:
    NameServerConnectionHandler(NameService *service) {
        this->service = service;
    }

    virtual bool read(yarp::os::ConnectionReader& reader) {
        return apply(reader,NULL);
    }

    virtual bool apply(yarp::os::ConnectionReader& reader,
                       yarp::os::ConnectionWriter *writer = NULL) {
        yarp::os::Bottle cmd, reply, event;
        bool ok = cmd.read(reader);
        if (!ok) return false;
        yarp::os::Contact remote;
        remote = reader.getRemoteContact();
        service->lock();
        ok = service->apply(cmd,reply,event,remote);
        for (int i=0; i<event.size(); i++) {
            yarp::os::Bottle *e = event.get(i).asList();
            if (e!=NULL) {
                service->onEvent(*e);
            }
        }
        service->unlock();
        if (writer==NULL) {
            writer = reader.getWriter();
        }
        if (writer!=NULL) {
            //printf("sending reply %s\n", reply.toString().c_str());
            if (reply.get(0).toString()=="old") {
                // support old name server messages
                for (int i=1; i<reply.size(); i++) {
                    yarp::os::Value& v = reply.get(i);
                    if (v.isList()) {
                        std::string s = v.asList()->toString().c_str();
                        
                        // old name server messages don't do quotes
                        for (int i=s.length()-1; i>=0; i--) {
                            if (s[i]=='\"') {
                                s.erase(i,1);
                            }
                        }
                        if (s.length()>0) {
                            writer->appendString(s.c_str());
                        }
                    } else {
                        yarp::os::Bottle b;
                        b.add(v);
                        b.write(*writer);
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


