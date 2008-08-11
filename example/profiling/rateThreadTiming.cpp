// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


// rate thread example -nat

#include <stdio.h>

#include <yarp/os/all.h>
#include <stdio.h>

using namespace yarp::os;

const int THREAD_PERIOD=15;
const int MAIN_WAIT=10;
const double THREAD_CPU_TIME=0.1;

/*
 * We start a thread with a given rate. We let
 * the thread run for a certain time. We measure
 * the real period.
 * Parameters:
 * --period set the periodicity of the thread (ms).
 * --time the time we wait before quitting (seconds).
 * --cpu time spent in thread (percentage)
 * August 08, Lorenzo Natale.
 */

class Thread1 : public RateThread {
    double cpuUsage;
public:
	Thread1(int r=THREAD_PERIOD):RateThread(r)
    { cpuUsage=0; }

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

    virtual void run() 
    {
        if (getIterations()==10)
            {
                double estP=getEstPeriod();
                double estU=getEstUsed();
                fprintf(stderr, "Thread1 est dT:%.3lf[ms]\n", estP);
                fprintf(stderr, "Thread1 est used:%.3lf[ms]\n", estU);
                resetStat();
            }

        double time=getRate()*cpuUsage/1000; //go to seconds

        double start=Time::now();
        double now=start;
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
    Property p;
    Thread1 t1;
    
    p.fromCommand(argc, argv);

    int period=p.check("period", Value(THREAD_PERIOD)).asInt();
    double time=p.check("time", Value(MAIN_WAIT)).asDouble();
    double cpuTime=p.check("cpu", Value(THREAD_CPU_TIME)).asDouble();

    Time::turboBoost();

    t1.setRate(period);
    t1.setCpuTime(cpuTime);

    printf("Going to start a thread with period %d[ms]\n", period);
    printf("Going to wait %.2lf seconds before quitting\n", time);

    t1.start(); 

    Time::delay(time);

    t1.stop();
    return 0;
}
