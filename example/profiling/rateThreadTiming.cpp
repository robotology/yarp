/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// rate thread example -nat

#include <stdio.h>

#include <yarp/os/all.h>
#include <stdio.h>
#include <vector>
#include <stdio.h>
#include <string>

using namespace yarp::os;
using namespace std;

const double THREAD_PERIOD=0.015;
const int MAIN_WAIT=10;
const double THREAD_CPU_TIME=0.1;

/*
 * We start a thread with a given rate. We let
 * the thread run for a certain time. We measure
 * the real period.
 * Parameters:
 * --period set the periodicity of the thread (s).
 * --time the time we wait before quitting (seconds).
 * --iterations how many iterations the thread will do
 * --cpu time spent in thread (percentage)
 * August 08, Lorenzo Natale.
 */

class Thread1 : public PeriodicThread {
    double cpuUsage;
    int iterations;
    std::vector<double> measures;
public:
    Thread1(double p=THREAD_PERIOD):PeriodicThread(p)
    {
        cpuUsage=0;
        iterations=-1;
    }

    virtual bool threadInit()
    {
        printf("Starting thread1\n");
        return true;
    }

    //called by start after threadInit, s is true iff the thread started
    //successfully
    virtual void afterStart(bool s)
    {
        if (s)
            printf("Thread1 started successfully\n");
        else
            printf("Thread1 did not start\n");

    }

    void setIterations(int it)
    {
        iterations=it;
        if (iterations>0)
            measures.reserve(iterations);
    }

    void dump(const std::string &filename)
    {
        std::vector<double>::iterator it=measures.begin();
        FILE *fp=fopen(filename.c_str(), "w");
        while(it!=measures.end())
            {
                fprintf(fp, "%lf\n", *it);
                it++;
            }
        fclose(fp);
    }

    virtual void run()
    {
        static int count=0;
        static double prev=Time::now();
        double now=Time::now();
        double dT=now-prev;
        prev=now;

        if ( (count<=iterations) && (count>0))
            {
                measures.push_back(dT*1000);
            }
        count++;
#if 0
        if (getIterations()==20)
            {
                double estP, pStd, estU, uStd;
                getEstPeriod(estP, pStd);
                getEstUsed(estU, uStd);
                fprintf(stderr, "Thread1 est dT:%.3lf[ms] +/- %lf[ms]\n", estP, pStd);
                fprintf(stderr, "Thread1 est used:%.3lf[ms] +/- %lf[ms]\n", estU, uStd);
                resetStat();
            }
#endif

        double time;
        time=getPeriod()*cpuUsage; //go to seconds

        double start=Time::now();
        now=start;
        while(now-start<time)
            now=Time::now();
    }

    virtual void threadRelease()
    {
        printf("Goodbye from thread1\n");
    }

    void setCpuTime(double t)
    { cpuUsage=t;}
};

int main(int argc, char **argv) {
    Network yarp;
    Property p;
    Thread1 t1;

    p.fromCommand(argc, argv);

    double period=p.check("period", Value(THREAD_PERIOD)).asFloat64();
    double time=p.check("time", Value(MAIN_WAIT)).asFloat64();
    double cpuTime=p.check("cpu", Value(THREAD_CPU_TIME)).asFloat64();
    int iterations=p.check("iterations", Value(-1)).asInt32();

    t1.setPeriod(period);
    t1.setCpuTime(cpuTime);
    t1.setIterations(iterations);

    printf("Going to start a thread with period %f[s]\n", period);
    if (iterations!=-1)
        printf("Going to wait %d iterations\n", iterations);
    printf("Thread will use %.0lf/100 cpu time\n", cpuTime*100);

    time=(period*(iterations+10));

    printf("Going to wait %.2lf seconds before quitting\n", time);

    t1.start();

    Time::delay(time);

    t1.stop();

    t1.dump("dump.txt");

    return 0;
}
