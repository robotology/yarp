// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/os/RateThread.h>
using namespace yarp::os;

// Timers test.
// We generate a 50% duty cycle square wave
// of given period, use only Time::now().
// 
// Lorenzo Natale August 2008 (TB improved)

#include <ppEventDebugger.h>

static ppEventDebugger pp(0x378);

int main(int argc, char **argv) 
{
    double totalTime=5; //seconds
    double period=0.01; //s

    double start=Time::now();
    double currentIt=start;
    while(currentIt-start<totalTime)
        {
            currentIt=Time::now();

            while((Time::now()-currentIt)<period/2.0)
                pp.set();
            while((Time::now()-currentIt)<period)
                pp.reset();
        }
}
