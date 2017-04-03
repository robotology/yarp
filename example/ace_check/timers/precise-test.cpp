
/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

// Example code to test timers in Linux (and ACE),
// run as sudo.
//
// Plug oscilloscope to the parallel port (any data bit),
// you should see a square wave, compare the length of
// the "high" level with th measure from the timer.
//
// Test show that gettimeofday() is precise enough to
// deal with the precision of the scheduler. RTSC
// seem less precise, but this could be hw dependent.
// Also RTSC could be more precise with short delays
// (to be tested).
//
// gettimeofday maps to ACE::gettimeofday(), whereas
// ACE's high res timers are probably implemented
// with RTSC.
//
// March 2008 -- Lorenzo Natale

#include <cmath>

#include <ace/ACE.h>
#include <sys/time.h>

#define RTSC(x)   __asm__ __volatile__ (  "rdtsc"                \
                                         :"=a" (((unsigned long*)&x)[0]),  \
                                          "=d" (((unsigned long*)&x)[1]))
#include <unistd.h>
#include <sys/io.h>
#define extern static
#include </usr/include/asm/io.h>
#undef extern

//this is the clock of your cpu, check /proc/cpuinfo
const int SLEEP=40;
#define CLOCK_TICKS 1663
#define PARALLEL_PORT 0x378

int main()
{
    ACE::init();
    fprintf(stderr, "Starting ACE timers test\n");

    unsigned short base=PARALLEL_PORT;
    if (ioperm(base, 1, 1))
        {
            fprintf(stderr, "Could not get the port at %x\n", base);

        }
    ACE_hrtime_t usecs;
    ACE_Time_Value sleep;

    int i;
    for(i=0;i<times;i++)
        {
            sleep.ms(SLEEP);
            struct timeval tv1;
            struct timeval tv2;
            gettimeofday(&tv1,0);
            // RTSC(start);
            outb(0xff, base);

            //put here the code you want to time
            ACE_OS::sleep(sleep);
            /////////////////////

            // RTSC(stop);
            outb(0x0, base);

            gettimeofday(&tv2,0);

            // wait some time
            ACE_OS::sleep(sleep);
            double tmp;
            tmp=(tv2.tv_sec-tv1.tv_sec)*1000;
            tmp+=(tv2.tv_usec-tv1.tv_usec)/1000;
            // tmp=(stop-start)/(CLOCK_TICKS*1000)
            fprintf(stderr, "%lf\n", tmp);
        }

    ioperm(base, 1, 0);
    ACE::fini();
}
