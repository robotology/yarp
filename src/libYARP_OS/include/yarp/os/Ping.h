/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PING_H
#define YARP_OS_PING_H

#include <cmath>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class Stat;
        class ConnectResult;
        class RateResult;
        class Ping;
    }
}

class yarp::os::Stat {
public:
    Stat() {
        clear();
    }

    void clear() {
        tot = tot2 = 0;
        ct = at = 0;
        mu = 0;
        sigma = 1e10;
        // infinity would be better, but methods of getting infinity
        // require awkward dependencies
    }

    void add(double val) {
        tot += val;
        tot2 += val*val;
        ct++;
    }

    void add(const Stat& alt) {
        tot += alt.tot;
        tot2 += alt.tot2;
        ct += alt.ct;
    }

    double mean() {
        compute();
        return mu;
    }

    double deviation() {
        compute();
        return sigma;
    }

    double count() {
        return ct;
    }

    operator double() {
        return mean();
    }

private:
    void compute() {
        if (ct!=at) {
            // ct must be > 0
            mu = tot/ct;
            sigma = tot2/ct - mu*mu;
            if (sigma<0) sigma = 0; // round-off error
            sigma = sqrt(sigma);
            at = ct;
        }
    }

    int ct, at;
    double tot, tot2;
    double mu, sigma;
};

class yarp::os::ConnectResult {
public:
    Stat totalTime;  // total includes name server lookups
    Stat targetTime; // all time involving the target port

    void clear() {
        totalTime.clear();
        targetTime.clear();
    }

    void add(const ConnectResult& alt) {
        totalTime.add(alt.totalTime);
        targetTime.add(alt.targetTime);
    }
};

class yarp::os::RateResult {
public:
    Stat period;

    void clear() {
        period.clear();
    }

    void add(const RateResult& alt) {
        period.add(alt.period);
    }
};


/**
 *
 * Measure performance of a YARP port.  Can also be partially used for
 * non-YARP ports with a compatible protocol.
 *
 */
class yarp::os::Ping {
public:
    Ping(const char *target = nullptr) {
        if (target != nullptr) {
            setTarget(target);
        }
    }

    bool setTarget(const char *target) {
        this->target = target;
        return true;
    }

    void connect();

    void sample();

    void clear() {
        lastConnect.clear();
        accumConnect.clear();
    }

    ConnectResult getLastConnect() {
        return lastConnect;
    }

    ConnectResult getAverageConnect() {
        return accumConnect;
    }

    void report();

    static ConstString renderTime(double t, int space, int decimal);

private:
    ConstString target;
    ConnectResult lastConnect, accumConnect;
};


#endif // YARP_OS_PING_H
