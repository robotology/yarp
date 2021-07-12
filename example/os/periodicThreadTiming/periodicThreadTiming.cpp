/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Time.h>

#include <yarp/sig/Matrix.h>

#include <cstdio>

using yarp::os::Network;
using yarp::os::PeriodicThread;
using yarp::sig::Matrix;

constexpr int NROWS = 20;
constexpr int NCOLS = 20;
constexpr double THREAD_PERIOD = 0.015;
constexpr int MAIN_WAIT = 100;

class Thread1 : public PeriodicThread
{
    Matrix m;

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

        m.resize(NROWS, NCOLS);
    }

    void run() override
    {
        if (getIterations() == 10) {
            double estP = getEstimatedPeriod();
            double estU = getEstimatedUsed();
            fprintf(stderr, "Thread1 est dT:%.3lf[ms]\n", estP);
            fprintf(stderr, "Thread1 est used:%.3lf[ms]\n", estU);
            resetStat();
        }

        for (size_t r = 0; r < m.rows(); r++) {
            for (size_t c = 0; c < m.cols(); c++) {
                m[r][c] = rand();
            }
        }
    }

    void threadRelease() override
    {
        printf("Goodbye from thread1\n");
    }
};

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    yarp::os::Network network;
    Thread1 t1(THREAD_PERIOD);
    t1.start();

    yarp::os::Time::delay(MAIN_WAIT);

    t1.stop();
    return 0;
}
