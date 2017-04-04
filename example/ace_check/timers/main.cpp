
/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

// Test timing on your system, this affects
// the precision with which you can schedule
// periodic thread. It boils down to
// - how precise are timers
// - how precise is Sleep()
//
// In general this depends on:
// - hardware
// - frequency of the scheduler, in Windows
// you can change it at runtime (see Time::turboboost())
// in Linux you have to change a parameter when you
// compile the kernel (default values have been changed
// from 100Hz, to 1000Hz and now seems back to 250Hz).
//
// With scheduler at 1kHz, expected performances
// should around +/- 1-2ms, or better.
//
// March 2008 -- Lorenzo Natale

#include <cmath>

#include <ace/ACE.h>
#include <ace/High_Res_Timer.h>
#include <ace/Profile_Timer.h>

const int wTimes=11;
//list of delays to be tested/measured
const int sleepT[wTimes]={1,2,5,10,15,20,50,100,500,1000,2000};
const int iterations=10;

int main()
{
    ACE::init();
    fprintf(stderr, "Starting timing test\n");

    ACE_Time_Value now1,now2;

    ACE_High_Res_Timer timer;
    ACE_Profile_Timer profiler;
    ACE_Time_Value sleep;
    ACE_Time_Value elapsed;
    double avErrors[wTimes];

    int i,k;

    for(i=0;i<wTimes;i++)
        avErrors[i]=0;

    for(i=0;i<iterations;i++)
        {
            double req;
            double time;
            for(k=0;k<wTimes;k++)
                {
                    req=sleepT[k];
                    sleep.msec(sleepT[k]);

                    now1 = ACE_OS::gettimeofday ();

                    //ACE_OS::sleep(sleep);
                    usleep(sleep.sec()*1000000+sleep.usec()-1000);

                    now2 = ACE_OS::gettimeofday ();

                    time=(now2.sec()-now1.sec())*1000;
                    time+=(now2.usec()-now1.usec())/1000;
                    avErrors[k]+=fabs(req-time)/iterations;
                    fprintf(stderr, "*");
                }
            fprintf(stderr, "Completed %d out of %d\n", i+1, iterations);
        }

    for(i=0;i<wTimes;i++)
        {
            sleep.msec(sleepT[i]);
            fprintf(stderr, "Req %us and %u[ms], average error %.3lf\n", (unsigned int)sleep.sec(), (unsigned int) sleep.usec()/1000, avErrors[i]);
        }

    ACE::fini();
}
