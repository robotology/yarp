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

#include <string>
#include <vector>

using namespace std;

static ppEventDebugger pp(0x378);

const int THREAD_PERIOD=20;

class ThreadB: public Thread
{
    Semaphore mutex;
    int iterations;
    double stamp;
    vector<double> measures;
public:

    int setIterations(int it)
    {
        iterations=it;
        if (iterations>0)
            measures.reserve(iterations);
    }

    bool threadInit()
    {
        fprintf(stderr, "ThreadB starting\n");
        return true;
    }

    void wakeUp(double t)
    {
        pp.set();
        stamp=t;
        mutex.post();
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

    void run()
    {
        while(!isStopping())
            {
                static int count=0;
                mutex.wait();
                count++;
                pp.reset();
                if (count<iterations)
                    {
                        double dT=(Time::now()-stamp)*1000; //ms
                        measures.push_back(dT);
                    }
            }
    }

};

class ThreadA: public RateThread
{
    ThreadB *slave;
    int iterations;

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
        if(slave!=0)
            slave->wakeUp(Time::now());
    }

};

int main(int argc, char **argv) 
{
    Network yarp;
    Time::turboBoost();
    Property p;
    p.fromCommand(argc, argv);

    int period=p.check("period", Value(THREAD_PERIOD)).asInt();
    int iterations=p.check("iterations", Value(-1)).asInt();

    ThreadB tB;
    ThreadA tA(period);
    tB.setIterations(iterations);

    tA.setSlave(&tB);

    tB.start();
    tA.start();

    double time=(period*(iterations+10))/1000;
    Time::delay(time);

    tB.stop();  //stop B first, important
    tA.stop();

    tB.dump("dump.txt");
}
