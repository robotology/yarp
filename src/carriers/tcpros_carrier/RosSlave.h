/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TcpRosLogComponent.h"

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
    bool worked;
public:
    RosSlave() :
        portnum(-1),
        done(0),
        worked(false)
    {}

    void start(const char *hostname, int portnum) {
        this->hostname = hostname;
        this->portnum = portnum;
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
        yCDebug(TCPROSCARRIER, "slave got request %s", cmd.toString().c_str());
        reply.addInt32(1);
        reply.addString("");
        yarp::os::Bottle& lst = reply.addList();
        lst.addString("TCPROS");
        lst.addString(hostname.c_str());
        lst.addInt32(portnum);
        yarp::os::ConnectionWriter *writer = reader.getWriter();
        if (writer==NULL) { return false; }
        yCDebug(TCPROSCARRIER, "replying with %s", reply.toString().c_str());
        reply.write(*writer);
        done.post();
        return true;
    }
};
