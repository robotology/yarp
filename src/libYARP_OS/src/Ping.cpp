/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Ping.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Vocab.h>

#include <cstdio>

using namespace yarp::os;

#ifdef _MSC_VER
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif

void Ping::connect() {
    lastConnect.clear();
    double start = SystemClock::nowSystem();
    Contact c = NetworkBase::queryName(target);
    double afterQuery = SystemClock::nowSystem();
    if (!c.isValid()) {
        yError("Port not found during ping");
    }
    ContactStyle rpc;
    rpc.admin = true;
    rpc.quiet = true;
    Bottle cmd, reply;
    cmd.addVocab(Vocab::encode("ver"));
    bool ok = NetworkBase::write(c, cmd, reply, rpc);
    if (!ok) {
        yError("Port did not respond as expected");
    }
    double stop = SystemClock::nowSystem();
    lastConnect.totalTime.add(stop-start);
    lastConnect.targetTime.add(stop-afterQuery);
    accumConnect.add(lastConnect);
}

void Ping::report() {
    int ping = (int)(accumConnect.targetTime.count()+0.5);
    if (ping>0) {
        printf("Ping #%d:\n", (int)(accumConnect.targetTime.count()+0.5));
        int space = 14;
        int decimal = 5;
        printf("  %s connection time (%s with name lookup)\n",
               renderTime(lastConnect.targetTime.mean(), space, decimal).c_str(),
               renderTime(lastConnect.totalTime.mean(), space, decimal).c_str());
        if (accumConnect.totalTime.count()>1) {
            printf("  %s +/- %s on average (%s +/- %s with name lookup)\n",
                   renderTime(accumConnect.targetTime.mean(), space, decimal).c_str(),
                   renderTime(accumConnect.targetTime.deviation(), space, decimal).c_str(),
                   renderTime(accumConnect.totalTime.mean(), space, decimal).c_str(),
                   renderTime(accumConnect.totalTime.deviation(), space, decimal).c_str());
        }
    }
}


ConstString Ping::renderTime(double t, int space, int decimal) {
    ConstString unit = "";
    double times = 1;
    if (space<0) {
        yError("Negative space");
    }
    if (t>=1) {
        unit = "sec";
    } else if (t>1e-3) {
        unit = " ms";
        times = 1e3;
    } else if (t>1e-6) {
        unit = " us";
        times = 1e6;
    } else if (t>1e-9) {
        unit = " ns";
        times = 1e9;
    }
    char buf[512];
    safe_printf(buf, sizeof(buf), "%.*f%s", decimal, t*times,
                unit.c_str());
    return buf;
}


class PingSampler : public PortReader {
public:
    Semaphore mutex;
    int ct;
    double lastTime;
    Stat period;

    PingSampler() : mutex(1) { ct = 0; lastTime = 0; }

    virtual bool read(ConnectionReader& connection) override {
        double now = SystemClock::nowSystem();
        Bottle b;
        bool ok = b.read(connection);
        if (ok) {
            mutex.wait();
            ct++;
            if (ct>1) {
                double dt = now - lastTime;
                period.add(dt);
            }
            lastTime = now;
            printf("Period is %g +/- %g (%d)\n",
                   period.mean(),
                   period.deviation(),
                   ct);
            mutex.post();
        }
        return ok;
    }
};

void Ping::sample() {
    Port p;
    PingSampler sampler;
    p.setReader(sampler);
    p.open("...");
    printf("Pausing for 5 seconds...\n");
    NetworkBase::connect(target, p.getName());
    SystemClock::delaySystem(5);
    p.close();
    printf("Period is %g +/- %g (%d)\n",
           sampler.period.mean(),
           sampler.period.deviation(),
           sampler.ct);
}
