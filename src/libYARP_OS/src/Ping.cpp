/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Ping.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Log.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <cmath>
#include <cstdio>

using namespace yarp::os;

Stat::Stat()
{
    clear();
}

void Stat::clear()
{
    tot = tot2 = 0;
    ct = at = 0;
    mu = 0;
    sigma = 1e10;
    // infinity would be better, but methods of getting infinity
    // require awkward dependencies
}

void Stat::add(double val)
{
    tot += val;
    tot2 += val * val;
    ct++;
}

void Stat::add(const Stat& alt)
{
    tot += alt.tot;
    tot2 += alt.tot2;
    ct += alt.ct;
}

double Stat::mean()
{
    compute();
    return mu;
}

double Stat::deviation()
{
    compute();
    return sigma;
}

double Stat::count()
{
    return ct;
}

Stat::operator double()
{
    return mean();
}

void Stat::compute()
{
    if (ct != at) {
        // ct must be > 0
        mu = tot / ct;
        sigma = tot2 / ct - mu * mu;
        if (sigma < 0) {
            sigma = 0; // round-off error
        }
        sigma = sqrt(sigma);
        at = ct;
    }
}


void ConnectResult::clear()
{
    totalTime.clear();
    targetTime.clear();
}

void ConnectResult::add(const ConnectResult& alt)
{
    totalTime.add(alt.totalTime);
    targetTime.add(alt.targetTime);
}


void RateResult::clear()
{
    period.clear();
}

void RateResult::add(const RateResult& alt)
{
    period.add(alt.period);
}


Ping::Ping(const char* target)
{
    if (target != nullptr) {
        setTarget(target);
    }
}

bool Ping::setTarget(const char* target)
{
    this->target = target;
    return true;
}


void Ping::connect()
{
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
    Bottle cmd;
    Bottle reply;
    cmd.addVocab(Vocab::encode("ver"));
    bool ok = NetworkBase::write(c, cmd, reply, rpc);
    if (!ok) {
        yError("Port did not respond as expected");
    }
    double stop = SystemClock::nowSystem();
    lastConnect.totalTime.add(stop - start);
    lastConnect.targetTime.add(stop - afterQuery);
    accumConnect.add(lastConnect);
}

void Ping::report()
{
    long int ping = lround(accumConnect.targetTime.count() + 0.5);
    if (ping > 0) {
        printf("Ping #%ld:\n", lround(accumConnect.targetTime.count() + 0.5));
        int space = 14;
        int decimal = 5;
        printf("  %s connection time (%s with name lookup)\n",
               renderTime(lastConnect.targetTime.mean(), space, decimal).c_str(),
               renderTime(lastConnect.totalTime.mean(), space, decimal).c_str());
        if (accumConnect.totalTime.count() > 1) {
            printf("  %s +/- %s on average (%s +/- %s with name lookup)\n",
                   renderTime(accumConnect.targetTime.mean(), space, decimal).c_str(),
                   renderTime(accumConnect.targetTime.deviation(), space, decimal).c_str(),
                   renderTime(accumConnect.totalTime.mean(), space, decimal).c_str(),
                   renderTime(accumConnect.totalTime.deviation(), space, decimal).c_str());
        }
    }
}


std::string Ping::renderTime(double t, int space, int decimal)
{
    std::string unit;
    double times = 1;
    if (space < 0) {
        yError("Negative space");
    }
    if (t >= 1) {
        unit = "sec";
    } else if (t > 1e-3) {
        unit = " ms";
        times = 1e3;
    } else if (t > 1e-6) {
        unit = " us";
        times = 1e6;
    } else if (t > 1e-9) {
        unit = " ns";
        times = 1e9;
    }
    char buf[512];
    std::snprintf(buf, sizeof(buf), "%.*f%s", decimal, t * times, unit.c_str());
    return buf;
}


class PingSampler : public PortReader
{
public:
    Mutex mutex;
    int ct{0};
    double lastTime{0};
    Stat period;

    PingSampler() :
            mutex()
    {
    }

    bool read(ConnectionReader& connection) override
    {
        double now = SystemClock::nowSystem();
        Bottle b;
        bool ok = b.read(connection);
        if (ok) {
            mutex.lock();
            ct++;
            if (ct > 1) {
                double dt = now - lastTime;
                period.add(dt);
            }
            lastTime = now;
            printf("Period is %g +/- %g (%d)\n",
                   period.mean(),
                   period.deviation(),
                   ct);
            mutex.unlock();
        }
        return ok;
    }
};

void Ping::sample()
{
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

void Ping::clear()
{
    lastConnect.clear();
    accumConnect.clear();
}

ConnectResult Ping::getLastConnect()
{
    return lastConnect;
}

ConnectResult Ping::getAverageConnect()
{
    return accumConnect;
}
