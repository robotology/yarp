/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

// Show thread basic functionalities, you may want to have a look at the
// ratethread example.

// added initThread/releaseThread example -nat

#include <stdio.h>

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

class Thread1 : public Thread {
public:
    virtual bool threadInit()
	{
		printf("Starting thread1\n");
		return true;
	}

    virtual void run() {
        while (!isStopping()) {
            printf("Hello, from thread1\n");
            Time::delay(1);
        }
    }

    virtual void threadRelease()
	{
		printf("Goodbye from thread1\n");
	}
};

class Thread2 : public Thread {
public:
	virtual bool threadInit()
	{
		printf("Starting thread2\n");
		return true;
	}

    virtual void run() {
        Time::delay(0.5);
        while (!isStopping()) {
            printf("Hello from thread2\n");
            Time::delay(1);
        }
    }

    virtual void threadRelease()
	{
		printf("Goodbye from thread2\n");
	}
};

int main() {
    Thread1 t1;
    Thread2 t2;
    printf("Starting threads...\n");
    t1.start();
    t2.start();
    printf("started\n");
    Time::delay(3);
    printf("stopping threads...\n");
    t1.stop();
    t2.stop();
    printf("stopped\n");
    return 0;
}

