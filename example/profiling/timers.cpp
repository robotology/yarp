/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/os/RateThread.h>
using namespace yarp::os;

// Timers test.
// We generate a 50% duty cycle square wave
// of given period, use only Time::now().
// 
// Lorenzo Natale August 2008 (TB improved)

// Parameters:
// --period: wave period (ms)
// --time: total time (seconds)

#ifdef USE_PARALLEL_PORT
#include <ppEventDebugger.h>
static ppEventDebugger pp(0x378);
#endif

int main(int argc, char **argv) 
{
    Property p;
    p.fromCommand(argc, argv);

    double totalTime=-1; //seconds
    double period=0.1; //s

    if (p.check("period"))
        period=p.find("period").asDouble()/1000.0;
    if (p.check("time"))
        totalTime=p.find("time").asDouble();

    printf("Generating wave with period T=%.2lf[ms]\n", period*1000);
    if (totalTime!=-1)
        printf("Waiting %.2lf seconds\n", totalTime);
    else
        printf("Waiting forever\n");

    bool forever=false;

    if (totalTime==-1)
        forever=true;

    double start=Time::now();
    double currentIt=start;
    while((currentIt-start<totalTime) || forever)
        {
            currentIt=Time::now();

            while((Time::now()-currentIt)<period/2.0)
                {
#ifdef USE_PARALLEL_PORT
                    pp.set();
#endif

                }
            while((Time::now()-currentIt)<period)
                {
#ifdef USE_PARALLEL_PORT
                    pp.reset();
#endif
                }
        }
}
