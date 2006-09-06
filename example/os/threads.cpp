// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

using namespace yarp::os;

class Thread1 : public Thread {
public:
    virtual void run() {
        while (!isStopping()) {
            printf("Hello, from thread1\n");
            Time::delay(1);
        }
    }
};


class Thread2 : public Thread {
public:
    virtual void run() {
        Time::delay(0.5);
        while (!isStopping()) {
            printf("Hello from thread2\n");
            Time::delay(1);
        }
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

