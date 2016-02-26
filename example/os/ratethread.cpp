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

using namespace yarp::os;

class Thread1 : public RateThread {
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
	}

    virtual void run() 
    {
        printf("Hello, from thread1\n");
    }

    virtual void threadRelease()
	{
		printf("Goodbye from thread1\n");
	}
};

class Thread2: public RateThread {
public:
	Thread2(int r):RateThread(r){}
	virtual bool threadInit()
	{
		printf("Starting thread2\n");
		return true; 
	}

	//called by start after threadInit, s is true iff the thread started
	//successfully
	virtual void afterStart(bool s)
	{
		if (s)
			printf("Thread2 started successfully\n");
		else
			printf("Thread2 did not start\n");
	}

	virtual void run()
	{
		printf("Hello, from thread2\n");
	}

	virtual void threadRelease()
	{
		printf("Goodbye from thread2\n");
	}
};

int main() {
    Thread1 t1(500);  //run every 500ms
	Thread2 t2(1000); //run every 1s
	printf("thread1 rate is %d[ms]\n", 500);
	printf("thread2 rate is %d[ms]\n", 1000);

	printf("Starting threads...\n");
    bool ok=t1.start();
    ok = ok&&t2.start();
	if (!ok)
        {
            printf("One of the thread failed to initialize, returning\n");
            return -1;
        }

    Time::delay(3);
    printf("Thread1 ran %d times, estimated rate: %.lf[ms]\n", t1.getIterations(), t1.getEstPeriod());
    printf("Thread2 ran %d times, estimated rate: %.lf[ms]\n", t2.getIterations(), t2.getEstPeriod());

    printf("suspending threads...\n");
	t1.suspend();
    t2.suspend();

	printf("Waiting some time");
	for(int k=1;k<20;k++)
        {
            printf(".");
            fflush(stdout);
            Time::delay(0.2); //200[ms]
        }
	printf("\n");


	printf("Changing thread1 rate to %d[ms]\n", 250);
	printf("Changing thread2 rate to %d[ms]\n", 500);

	t1.setRate(250);
    t2.setRate(500);

	printf("Resuming threads...\n");
    t1.resetStat();
    t2.resetStat();
	t1.resume();
    t2.resume();

	Time::delay(3);
    
    printf("Thread1 ran %d times, estimated rate: %.lf[ms]\n", t1.getIterations(), t1.getEstPeriod());
    printf("Thread2 ran %d times, estimated rate: %.lf[ms]\n", t2.getIterations(), t2.getEstPeriod());
    t1.stop();
    t2.stop();
    printf("stopped\n");

    return 0;
}

