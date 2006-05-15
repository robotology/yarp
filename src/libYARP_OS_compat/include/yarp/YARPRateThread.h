// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, /// 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPRateThread.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

//
// 
#ifndef __YARPRateThreadh__
#define __YARPRateThreadh__

//////////////////////////////////////////////////////////////////////////
//
// YARPRateThread.h
//
// 
// Thread class implementation. Inherit from it and overload the three 
// pure virtual methods.
// Based on ACE.
//
// link:
// ace.lib
//
// WIN32: see SystemUtils::set_high_res_scheduling() if you want to do precise 
// scheduling (< 10/15 ms depening on the machine) --> link winmm.lib
// Linux: 
// - kernels prior to 2.5 have 10ms resolution (100Hz)
// - kernels >= 2.5 have 1ms resolution (1kHz)
// - Check the value of Hz in asm/param.h
//
// THREAD_STATS if you want to test scheduling statistics 
// 
// September 2002 -- by nat
// 
// September 2002 -- by nat -- modified release flag, added timeout
//
// TODO: check thread priorities

// define this if you want scheduling statistics
// #define THREAD_STATS
//	this is compiled out because it is rather time-consuming.

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Thread_Manager.h>
#include <ace/Synch.h>
#include <ace/Time_Value.h>
#include <ace/High_Res_Timer.h>

/**
 * \file YARPRateThread.h This contains definitions for a thread with
 * periodic activation.
 */

#ifdef THREAD_STATS
#include "IterativeStats.h" //from models lib link models.lib/modelsdb.lib
#endif

#include <yarp/YARPString.h>

#ifdef __QNX6__
#include <sys/neutrino.h>
#endif

const ACE_Time_Value _timeout_value(20,0);	// (20 sec) timeout value for the release (20 sec)

#ifdef THREAD_STATS
typedef struct thr_stts
{
	IterativeStats period;
	IterativeStats time;
} thread_statistics;
#endif

/**
 * YARPRateThread is the base class for a periodic thread. The user should derive
 * from this class and implement doInit(), doLoop(), and doRelease() methods. These will
 * contain the initialization, the main thread code, and the release code respectively.
 * The thread will wait on a timer and will acivate periodically until termination
 * is requested.
 */
class YARPRateThread 
{
protected:
	bool isRunningF;		// running flag.	
	bool isSuspendedF;		// suspended flag
	ACE_Time_Value period;	// period.
	YARPString name;		// thread name.
	int thread_id;			// thread id
	int thread_priority;	// thread priority
	ACE_hthread_t  handle;  // os internal descriptor, it is filled by the thread itself
	bool end;

	ACE_Auto_Event synchro;	// event for init synchro
	
	/**
	 * The intialization code. This is called once upon thread startup.
	 */
	virtual void doInit() =0;

	/**
	 * The main thread code. This is called periodically whenever the timer
	 * expires.
	 */
	virtual void doLoop() =0;

	/**
	 * The release code. When the thread receives a termination request it calls
	 * this code once.
	 */
	virtual void doRelease() =0;

#ifdef THREAD_STATS
	thread_statistics thread_stats;
#endif

	ACE_Lock_Adapter<ACE_Thread_Mutex> mutex;

public:
	/**
	 * Constructor.
	 * @param c is the name of thread. Currently unused.
	 * @param r is the thread interval in milliseconds.
	 */
	YARPRateThread(const char *c, int r): name(c)
	{
		isRunningF = false;
		isSuspendedF = false;

		thread_id = -1;
		thread_priority = ACE_DEFAULT_THREAD_PRIORITY;

		period.set(0, r*1000);	//period here is usec
	}

	/**
	 * Destructor.
	 */
	virtual ~YARPRateThread(void)
	{
		// close the thread
		if (isRunning()) 
            {
                terminate();
            }

	}

	/**
	 * Waits on the internal mutex.
	 */
	inline void lock(void)
	{	
		// timeout ?
		mutex.acquire();
	}

	/**
	 * Posts the internal mutex.
	 */
	inline void unlock(void)
	{
		mutex.release();
	}

	/**
	 * Starts the thread.
	 * @param wait is an unused argument.
	 */
	virtual void start(bool wait = true)
	{
		ACE_UNUSED_ARG (wait);
		lock ();

#ifdef __WIN32__
		// create suspended.
		thread_id = ACE_Thread_Manager::instance ()->spawn((unsigned long (__cdecl *)(void *))real_thread, //thread function
														   this,		//thread parameter
														   THR_NEW_LWP||THR_SUSPENDED,
														   0,
														   0,
														   thread_priority
                                                           );
#else
		// create suspended.
		thread_id = ACE_Thread_Manager::instance ()->spawn(real_thread, //thread function
                                                           this,		//thread parameter
                                                           THR_NEW_LWP||THR_SUSPENDED,
                                                           0,
                                                           0,
                                                           thread_priority
                                                           );
#endif
		ACE_Thread_Manager::instance()->resume_grp(thread_id);
		isRunningF = true;

		// wait for synchro
		synchro.wait();
		
		unlock (); //check the position of this lock() (it was before the wait)
	}

	/**
	 * Terminates the thread.
	 * @param timeout is a flag that tells whether to timeout
	 * on exit in case the thread code doesn't respond properly.
	 */
	void terminate(bool timeout = true)
	{
		if (!isRunning())
			return;

		if (isSuspended())
			resume();
		
		lock ();

		end = true;
		bool timeout_flag = false;

		if (!timeout)
            {
                synchro.wait();
                timeout_flag = false;
            }
		else if (synchro.wait(&_timeout_value, 0) == -1)
            {
#if defined(__WIN32__)
                // win32 doesn't support kill signal, so we have to use the API
                TerminateThread (handle, -1);
#elif defined(__QNX6__)
                ThreadDestroy (thread_id, -1, (void *)-1);
#elif defined(__LINUX__)
                pthread_cancel (thread_id);
#elif defined(__DARWIN__)
                pthread_cancel (thread_id);
#else
#error "destroy thread: not implemented for the specified architecture"
#endif
                timeout_flag = true;
            }

		// after the syncro event or the kill the thread should end, just wait for it
		ACE_Thread_Manager::instance()->wait_grp(thread_id);		
		
		isRunningF = false;
		isSuspendedF = false;

#ifdef THREAD_STATS
        if (timeout_flag)
			{
				cout << "WARNING: " << name << " exited with timeout\n";
			}
        else
            cout << name << " exited gracefully.\n";

        cout << "Dumping statistics:\n";
        cout << "Estimated period: "<< thread_stats.period.get_mean();
        cout << " +/-" << thread_stats.period.get_std() << "\n";
        cout << "Estimated time: "<< thread_stats.time.get_mean();
        cout << " +/-" << thread_stats.time.get_std();
        cout << "\n";
#endif /* THREAD_STATS */

		unlock ();
	}

	/**
	 * Sets the thread priority.
	 * @param p is the thread priority. Warning: this might be OS dependent.
	 */
	void setPriority(int p)
	{
		thread_priority = p;
		if (isRunning())
			ACE_OS::thr_setprio(handle, p);
	}

	/**
	 * Gets the thread priority.
	 * @return the thread priority value.
	 */
	int getPriority()
	{
		if (isRunning())
			ACE_OS::thr_getprio(handle, thread_priority);
		
		return thread_priority;
	}

	/**
	 * Sets a new thread interval.
	 * @param i is the new interval in milliseconds.
	 */
	void setInterval(int i)
	{
		if (i >= 0)
            {
                bool was_running = isRunning();
                if (was_running)
                    {
                        suspend();
                    }

                period.set(0,i*1000);
		

                if (was_running)
                    resume();
            }
	}

	/**
	 * Suspends the thread execution.
	 */
	void suspend()
	{
		lock();
		ACE_Thread_Manager::instance ()->suspend_grp(thread_id);
		isSuspendedF = true;

#ifdef THREAD_STATS
		thread_stats.period.reset();
		thread_stats.time.reset();
#endif
		unlock();
	}

	/**
	 * Resumes the thread execution.
	 */
	void resume()
	{
		lock();
		isSuspendedF = false;
		ACE_Thread_Manager::instance ()->resume_grp(thread_id);
		unlock();
	}

	/**
	 * Tests whether the thread is running.
	 * @return true if the thread is in execution.
	 */
	bool isRunning(void)
	{
		lock ();
		bool ret = isRunningF;
		unlock ();
		return ret;
	}

	/**
	 * Tests whether the thread is suspended.
	 * @return true if the thread is suspended.
	 */
	bool isSuspended(void)
	{
		lock();
		bool ret = isSuspendedF;
		unlock();
		return ret;
	}

	/**
	 * Gets the thread name.
	 * @return the name of the thread.
	 */
	const char *getName(void) const { return name.c_str(); }

	/**
	 * Gets the thread ID.
	 * @return the ID of the thread (OS specific).
	 */
	const int getThreadID(void) const { return thread_id; }

	static void *real_thread(void *p_arg)
	{
		YARPRateThread *context = (YARPRateThread *) p_arg;

		ACE_High_Res_Timer	begin_timer;	// timer to estimate period
		ACE_High_Res_Timer	thread_timer;	// timer to estimate thread time
		ACE_Time_Value		est_time;		// thread time
#ifdef THREAD_STATS
		ACE_Time_Value		est_period;		// thread period
#endif
		ACE_Time_Value		sleep_period;	// thread sleep

		context->end = false;		
		ACE_Thread_Manager::instance()->thr_self(context->handle);	//get thread os handle
		
		context->doInit();
#ifdef THREAD_STATS
        begin_timer.start();
#endif /* THREAD_STATS */
		context->synchro.signal();
		// set synchro event
		// this first sleep in principle is not needed; it is to avoid checking the
		// first "est_period" value
		ACE_OS::sleep(context->period);
		while (!context->end)
            {
#ifdef THREAD_STATS
				// est period
				begin_timer.stop();
				begin_timer.elapsed_time(est_period);
				begin_timer.start();
				// context->thread_stats.period+=est_period.msec();
				context->thread_stats.period+=est_period.usec()/1000.0;
#endif /* THREAD_STATS */

                // loop
                thread_timer.start();
                context->doLoop ();
                thread_timer.stop();
                thread_timer.elapsed_time(est_time);
			
#ifdef THREAD_STATS
				// context->thread_stats.time+=est_time.msec();
				context->thread_stats.time+=est_time.usec()/1000.0;
#endif /* THREAD_STATS */

                // compute sleep time
                sleep_period = context->period-est_time;
			
                if (sleep_period.usec() < 0 || sleep_period.sec() < 0)
                    sleep_period.set(0,0);

                ACE_OS::sleep(sleep_period);
                // WaitForSingleObject(w32timer, INFINITE);
            }

		context->doRelease ();
		// since we have no control on the doRelease function
		// terminate is waiting on "syncro" with timout; the thread is killed if
		// timout accurs.
		context->synchro.signal();	
		// Release waits on ACE_Thread_Manager::instance()->wait_grp();
		return 0;
	}
};

#endif
