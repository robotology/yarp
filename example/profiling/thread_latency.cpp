// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/os/RateThread.h>
using namespace yarp::os;

// Thread latency, basic test.
// Thread A is periodic. Thread B is 
// not periodic, but waits on a mutex. Thread
// A posts the mutex to wake up B.
// We measure the time it takes to for B to 
// wake up.
// 
// Lorenzo Natale August 2008

#include <ppEventDebugger.h>

static ppEventDebugger pp(0x378);

class ThreadB: public Thread
{
    Semaphore mutex;
public:
    bool threadInit()
    {
        fprintf(stderr, "ThreadB starting\n");
        return true;
    }

    void wakeUp()
    {
        pp.set();
        mutex.post();
    }

    void run()
    {
        while(!isStopping())
            {
                mutex.wait();
                pp.reset();
                fprintf(stderr, "TheredB::run()\n");
            }
    }

};

class ThreadA: public RateThread
{
    ThreadB *slave;

public:
    ThreadA(int period): RateThread(period)
    {
        slave=0;
    }

    void setSlave(ThreadB *B)
    {
        slave=B;
    }

    bool threadInit()
    {
        fprintf(stderr, "ThreadA starting\n");
        return true;
    }

    void run()
    {
        fprintf(stderr, "ThreadA run()\n");
        if(slave!=0)
            slave->wakeUp();
    }

};

int main(int argc, char **argv) 
{
    Network yarp;

    ThreadB tB;
    ThreadA tA(1);

    tA.setSlave(&tB);

    tB.start();
    tA.start();

    Time::delay(100);

    tB.stop();  //stop B first, important
    tA.stop();
}
