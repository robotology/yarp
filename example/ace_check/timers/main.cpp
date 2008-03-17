// Test how ACE timers performs on your system.
// This is hardware dependent.
// 
// Performance should degrade for long waiting time. 
// Expected errors are < 1ms, for sleep in the range 
// of 1-100 ms, and around 1-3 ms for waits of 1-2 
// seconds.
//
// March 2008 -- Lorenzo Natale

#include <math.h>

#include <ace/ACE.h>
#include <ace/High_Res_Timer.h>

const int wTimes=11;
//list of delays to be tested/measured
const int sleepT[wTimes]={1,2,5,10,15,20,50,100,500,1000,2000};
const int iterations=10;

int main()
{
    ACE::init();
    fprintf(stderr, "Starting ACE timers test\n");

    ACE_High_Res_Timer timer;
    ACE_hrtime_t usecs;
    ACE_Time_Value sleep;
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
                    
                    timer.reset();
                    timer.start();
                    ACE_OS::sleep(sleep);
                    timer.stop();
                    
                    timer.elapsed_microseconds(usecs);
                    time=usecs/1000.0;
                    avErrors[k]+=fabs(req-time)/iterations;
                    fprintf(stderr, ".");
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
