// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


// rate thread example -nat

#include <stdio.h>

#include <yarp/os/all.h>
#include <yarp/sig/Matrix.h>
#include <stdio.h>

using namespace yarp::os;
using namespace yarp::sig;

const int NROWS=20;
const int NCOLS=20;
const int THREAD_PERIOD=15;
const int MAIN_WAIT=10;

/*
 * We start a thread with a given rate. We let
 * the thread run for a certain time. We measure
 * the real period.
 * Parameters:
 * --period set the periodicity of the thread (ms).
 * --time the time we wait before quitting (seconds).
 *
 * August 08, Lorenzo Natale.
 */

class Thread1 : public RateThread {
    Matrix m;
public:
	Thread1(int r=THREAD_PERIOD):RateThread(r){}

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


        m.resize(NROWS,NCOLS);
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

        for(int r=0;r<m.rows();r++)
            for(int c=0;c<m.cols();c++)
                m[r][c]=rand();
    }

    virtual void threadRelease()
	{
		printf("Goodbye from thread1\n");
	}
};

int main(int argc, char **argv) {
    Property p;
    Thread1 t1;

    p.fromCommand(argc, argv);

    int period=p.check("period", Value(THREAD_PERIOD)).asInt();
    double time=p.check("time", Value(MAIN_WAIT)).asDouble();

    Time::turboBoost();

    t1.setRate(period);

    printf("Going to start a thread with period %d[ms]\n", period);
    printf("Going to wait %.2lf seconds before quitting\n", time);

    t1.start(); 

    Time::delay(time);

    t1.stop();
    return 0;
}
