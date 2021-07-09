/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PING_H
#define YARP_OS_PING_H

#include <yarp/os/api.h>

#include <cmath>
#include <string>


namespace yarp {
namespace os {

class Stat
{
public:
    Stat();

    void clear();

    void add(double val);
    void add(const Stat& alt);
    double mean();
    double deviation();
    double count();

    operator double();

private:
    void compute();

    int ct;
    int at;
    double tot;
    double tot2;
    double mu;
    double sigma;
};

class ConnectResult
{
public:
    Stat totalTime;  // total includes name server lookups
    Stat targetTime; // all time involving the target port

    void clear();
    void add(const ConnectResult& alt);
};

class RateResult
{
public:
    Stat period;

    void clear();
    void add(const RateResult& alt);
};


/**
 * Measure performance of a YARP port.  Can also be partially used for
 * non-YARP ports with a compatible protocol.
 */
class YARP_os_API Ping
{
public:
    Ping(const char* target = nullptr);

    bool setTarget(const char* target);

    void connect();

    void sample();

    void clear();
    ConnectResult getLastConnect();
    ConnectResult getAverageConnect();

    void report();

    static std::string renderTime(double t, int space, int decimal);

private:
    std::string target;
    ConnectResult lastConnect, accumConnect;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PING_H
