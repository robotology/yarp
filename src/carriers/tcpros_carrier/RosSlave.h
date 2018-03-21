/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>

#include <tcpros_carrier_api.h>

// temporary slave
class RosSlave : public yarp::os::PortReader {
private:
    yarp::os::Port slave;
    yarp::os::ConstString hostname;
    int portnum;
    yarp::os::Semaphore done;
    bool verbose;
    bool worked;
public:
    RosSlave(bool verbose) :
        portnum(-1),
        done(0),
        verbose(verbose),
        worked(false)
    {}

    void start(const char *hostname, int portnum) {
        this->hostname = hostname;
        this->portnum = portnum;
        slave.setVerbosity(-1);
        slave.setReader(*this);
        slave.open("...");
    }

    void stop() {
        double delay = 0.1;
        while (!done.check()) {
            if (delay>1) {
                worked = false;
                break;
            }
            // Using SystemClock since yarp version 2.3.70 as part of global clock refactoring & bugFixing
            yarp::os::SystemClock::delaySystem(delay);
            delay *= 2;
        }
        if (delay<=1) {
            worked = true;
        }
        slave.close();
    }

    yarp::os::Contact where() {
        return slave.where();
    }

    bool isOk() {
        return worked;
    }

    virtual bool read(yarp::os::ConnectionReader& reader) override {
        yarp::os::Bottle cmd, reply;
        bool ok = cmd.read(reader);
        if (!ok) return false;
        if (verbose) printf("slave got request %s\n", cmd.toString().c_str());
        reply.addInt(1);
        reply.addString("");
        yarp::os::Bottle& lst = reply.addList();
        lst.addString("TCPROS");
        lst.addString(hostname.c_str());
        lst.addInt(portnum);
        yarp::os::ConnectionWriter *writer = reader.getWriter();
        if (writer==NULL) { return false; }
        if (verbose) printf("replying with %s\n", reply.toString().c_str());
        reply.write(*writer);
        done.post();
        return true;
    }
};
