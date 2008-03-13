// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


// rate thread example -nat

#include <stdio.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>

#include <yarp/sig/Matrix.h>
#include <yarp/math/Rand.h>

using namespace yarp::os;
using namespace yarp::math;
using namespace yarp::sig;

const int NROWS=100;
const int NCOLS=100;

const int THREAD_PERIOD=4;
const int MAIN_WAIT=3;

class Thread1 : public RateThread {
    Matrix m;
public:
	Thread1(int r):RateThread(r){}
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
        if (getIterations()==5)
            {
                double estP=getEstPeriod();
                double estU=getEstUsed();
                fprintf(stderr, "Thread1 est dT:%.3lf[ms]\n", estP);
                fprintf(stderr, "Thread1 est used:%.3lf[ms]\n", estU);
                resetStat();
            }

        for(int r=0;r<m.rows();r++)
            for(int c=0;c<m.cols();c++)
                m[r][c]=Rand::scalar(-1,1);
    }

    virtual void threadRelease()
	{
		printf("Goodbye from thread1\n");
	}
};

int main() {
    Thread1 t1(THREAD_PERIOD);

    t1.start(); 

    Time::delay(MAIN_WAIT);

    t1.stop();
    return 0;
}
