/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Show thread basic functionalities, you may want to have a look at the
 * ratethread example.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

#include <cstdio>

using yarp::os::Network;
using yarp::os::Thread;

constexpr double thread1_delay = 1.0;
constexpr double thread2_delay = 0.5;
constexpr double main_delay = 3.0;

class Thread1 : public Thread
{
public:
    bool threadInit() override
    {
        printf("Starting thread1\n");
        return true;
    }

    void run() override
    {
        while (!isStopping()) {
            printf("Hello, from thread1\n");
            yarp::os::Time::delay(thread1_delay);
        }
    }

    void threadRelease() override
    {
        printf("Goodbye from thread1\n");
    }
};

class Thread2 : public Thread
{
public:
    bool threadInit() override
    {
        printf("Starting thread2\n");
        return true;
    }

    void run() override
    {
        yarp::os::Time::delay(thread2_delay);
        while (!isStopping()) {
            printf("Hello from thread2\n");
            yarp::os::Time::delay(thread2_delay);
        }
    }

    void threadRelease() override
    {
        printf("Goodbye from thread2\n");
    }
};

int main()
{
    Network yarp;
    Thread1 t1;
    Thread2 t2;
    printf("Starting threads...\n");
    t1.start();
    t2.start();
    printf("started\n");
    yarp::os::Time::delay(main_delay);
    printf("stopping threads...\n");
    t1.stop();
    t2.stop();
    printf("stopped\n");
    return 0;
}
