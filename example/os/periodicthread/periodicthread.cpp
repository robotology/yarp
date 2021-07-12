/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Time.h>

#include <cstdio>

using yarp::os::Network;
using yarp::os::PeriodicThread;


class Thread1 : public PeriodicThread
{
public:
    Thread1(double p) :
            PeriodicThread(p)
    {
    }

    bool threadInit() override
    {
        printf("Starting thread1\n");
        return true;
    }

    //called by start after threadInit, s is true iff the thread started
    //successfully
    void afterStart(bool s) override
    {
        if (s) {
            printf("Thread1 started successfully\n");
        } else {
            printf("Thread1 did not start\n");
        }
    }

    void run() override
    {
        printf("Hello, from thread1\n");
    }

    void threadRelease() override
    {
        printf("Goodbye from thread1\n");
    }
};

class Thread2 : public PeriodicThread
{
public:
    Thread2(double p) :
            PeriodicThread(p)
    {
    }

    bool threadInit() override
    {
        printf("Starting thread2\n");
        return true;
    }

    //called by start after threadInit, s is true iff the thread started
    //successfully
    void afterStart(bool s) override
    {
        if (s) {
            printf("Thread2 started successfully\n");
        } else {
            printf("Thread2 did not start\n");
        }
    }

    void run() override
    {
        printf("Hello, from thread2\n");
    }

    void threadRelease() override
    {
        printf("Goodbye from thread2\n");
    }
};

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network network;

    constexpr double thread1_period1 = 0.500; //run every 500ms
    constexpr double thread1_period2 = 0.250; //run every 250ms
    constexpr double thread2_period1 = 1.000; //run every 1s
    constexpr double thread2_period2 = 0.500; //run every 500ms
    constexpr double stat_delay = 3.0;        // 3s
    constexpr double pause_loop_delay = 0.2;  // 200ms

    Thread1 t1(thread1_period1);
    Thread2 t2(thread2_period1);
    printf("thread1 period is %f[s]\n", thread1_period1);
    printf("thread2 period is %f[s]\n", thread2_period1);

    printf("Starting threads...\n");
    bool ok = t1.start();
    ok = ok && t2.start();
    if (!ok) {
        printf("One of the thread failed to initialize, returning\n");
        return -1;
    }

    yarp::os::Time::delay(3);
    printf("Thread1 ran %d times, estimated period: %.lf[ms]\n", t1.getIterations(), t1.getEstimatedPeriod());
    printf("Thread2 ran %d times, estimated period: %.lf[ms]\n", t2.getIterations(), t2.getEstimatedPeriod());

    printf("suspending threads...\n");
    t1.suspend();
    t2.suspend();

    printf("Waiting some time");
    for (int k = 1; k < 20; k++) {
        printf(".");
        fflush(stdout);
        yarp::os::Time::delay(pause_loop_delay);
    }
    printf("\n");


    printf("Changing thread1 period to %f[s]\n", thread1_period2);
    printf("Changing thread2 period to %f[s]\n", thread2_period2);
    t1.setPeriod(thread1_period2);
    t2.setPeriod(thread2_period2);

    printf("Resuming threads...\n");
    t1.resetStat();
    t2.resetStat();
    t1.resume();
    t2.resume();

    yarp::os::Time::delay(stat_delay);

    printf("Thread1 ran %d times, estimated period: %.lf[ms]\n", t1.getIterations(), t1.getEstimatedPeriod());
    printf("Thread2 ran %d times, estimated period: %.lf[ms]\n", t2.getIterations(), t2.getEstimatedPeriod());
    t1.stop();
    t2.stop();
    printf("stopped\n");

    return 0;
}
