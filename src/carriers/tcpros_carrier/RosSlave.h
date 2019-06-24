/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/SystemClock.h>

#include <string>

// temporary slave
class RosSlave :
        public yarp::os::PortReader
{
private:
    yarp::os::Port slave;
    std::string hostname;
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
            // Always use SystemClock for this delay
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

    bool read(yarp::os::ConnectionReader& reader) override {
        yarp::os::Bottle cmd, reply;
        bool ok = cmd.read(reader);
        if (!ok) return false;
        if (verbose) printf("slave got request %s\n", cmd.toString().c_str());
        reply.addInt32(1);
        reply.addString("");
        yarp::os::Bottle& lst = reply.addList();
        lst.addString("TCPROS");
        lst.addString(hostname.c_str());
        lst.addInt32(portnum);
        yarp::os::ConnectionWriter *writer = reader.getWriter();
        if (writer==NULL) { return false; }
        if (verbose) printf("replying with %s\n", reply.toString().c_str());
        reply.write(*writer);
        done.post();
        return true;
    }
};
