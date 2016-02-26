/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

// rate thread example -nat

#include <stdio.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>

#include <yarp/sig/Matrix.h>
#include <stdio.h>

using namespace yarp::os;
using namespace yarp::sig;

const int NROWS=20;
const int NCOLS=20;
const int THREAD_PERIOD=15;
const int MAIN_WAIT=100;

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

int main() {
    Thread1 t1(THREAD_PERIOD);
    Time::turboBoost();
    t1.start(); 

    Time::delay(MAIN_WAIT);

    t1.stop();
    return 0;
}
