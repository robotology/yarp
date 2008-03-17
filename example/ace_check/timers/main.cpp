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
#include <ace/Profile_Timer.h>

const int wTimes=11;
//list of delays to be tested/measured
const int sleepT[wTimes]={1,2,5,10,15,20,50,100,500,1000,2000};
const int iterations=2;

#include <sys/time.h>

#define RTSC(x)   __asm__ __volatile__ (  "rdtsc"                \
                                         :"=a" (((unsigned long*)&x)[0]),  \
                                          "=d" (((unsigned long*)&x)[1]))
int main()
{
    ACE::init();
    fprintf(stderr, "Starting ACE timers test\n");

    ACE_Time_Value now1,now2;

    ACE_High_Res_Timer timer;
    ACE_Profile_Timer profiler;
    ACE_Profile_Timer::ACE_Elapsed_Time  elTime;
    ACE_hrtime_t usecs;
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
                    
                    //timer.reset();
                    //timer.start();
                    //profiler.start();
                    //                    now1 = ACE_OS::gettimeofday ();
                    
                    // struct timeval tv1;
                    // gettimeofday(&tv1,0);

                    long long start;
                    long long end;
                    RTSC(start);

                    ACE_OS::sleep(sleep);
                    //timer.stop();
                    // profiler.stop();
                    
                    //timer.elapsed_microseconds(usecs);
                    //                    timer.elapsed(elapsed);
                    // timer.elapsedTime();
                    // time=usecs/1000.0;
                    //                    profiler.elapsed_time(elTime);
                    //                    time=elTime.real_time*1000;
                    //elapsed.sec()*1000;
                    //elapsed.usec()*1000;
                    //                    now2 = ACE_OS::gettimeofday ();
                    //                    struct timeval tv2;
                    //                    gettimeofday(&tv2,0);
                    RTSC(end);

                    //                    time=(now2.sec()-now1.sec())*1000;
                    //                    time+=(now2.usec()-now1.usec())/1000;
                    //                    time=(tv2.tv_sec-tv1.tv_sec)*1000;
                    //                    time+=(tv2.tv_usec-tv1.tv_usec)/1000;
                    time=(end-start)/(1663*1000);
                    avErrors[k]+=fabs(req-time)/iterations;
                    fprintf(stderr, ".");
                    fprintf(stderr, "%lf\n", time);
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
