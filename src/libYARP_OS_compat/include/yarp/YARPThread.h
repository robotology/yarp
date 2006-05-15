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
///	$Id: YARPThread.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///
/*
  paulfitz Mon May 21 13:42:24 EDT 2001
*/

/**
 * \file YARPThread.h A simple class to wrap OS thread management.
 * Very convenient but simple wrapper to encapsulate a thread. Derive
 * from this abstract class and implement Body() to make your own thread
 * code.
 */

#ifndef YARPThread_INC
#define YARPThread_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>
#include <yarp/YARPAll.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * The base class of thread creation/destruction and management.
 * This class contains a fairly intricate procedure for terminating
 * a thread without memory leaks or worse the generation of an exception.
 */
class YARPBareThread
{
protected:
	YARPBareThread(const YARPBareThread& yt);
	void operator=(const YARPBareThread& yt);

protected:
	/**
	 * This is the actual thread implementation the user shouldn't be
	 * concerned with.
	 */
	void *system_resource;

	/**
	 * Thread id.
	 * Each thread in the system is assigned an identifier.
	 */
	int identifier;

	/**
	 * To disappear, unused variable.
	 */
	int size;

	/**
	 * A semaphore for protecting certain operations.
	 * It can be used within the thread body to protect the thread
	 * from unexpected termination requests.
	 */
	YARPSemaphore sema;

	/**
	 * Whether Begin() has been called succesfully.
	 */
	bool running;

	/**
	 * The internal state of the thread.
	 * This is especially used to maintain the state of the thread
	 * during termination.
	 */
	enum 
        { 
            /**
             * The thread is healthy.
             */
            YT_None = 0, 
		
            /**
             * The thread is terminated.
             */
            YT_End = 1,

            /**
             * The thread has been requested to terminate.
             */
            YT_AskedEnd = 2,

            /**
             * The thread is joining its parent.
             */
            YT_Joining = 3,
        };

	/**
	 * The state of the last shutdown request.
	 */
	int shutdown_state;

public:
	/**
	 * Constructor.
	 */
	YARPBareThread(void);

	/**
	 * Destructor.
	 */
	virtual ~YARPBareThread(void);

	/**
	 * Begin thread.
	 * Creates a new thread with a certain stack size and 
	 * starts the thread execution from Body().
	 * @param stack_size is the thread stack size.
	 */
	virtual void Begin(int stack_size=0);

	/**
	 * Asks for thread termination.
	 * It changes the state of the thread termination sequence depending on the value of the
	 * parameter. The procedure for terminating a thread requires the active cooperation of
	 * the thread Body(). The thread should be polling termination requests by calling IsTerminated()
	 * periodically.
	 * @param dontkill can have various values that affect the behavior at termination. 
	 * If dontkill is equal to -1 then the method changes the shutdown state and waits for the 
	 * thread to exit (join). If dontkill is equal to 0 then the thread is terminated 
	 * immediately. If dontkill is greater than 0 then the thread goes in wait state for 
	 * dontkill milliseconds before being terminated.
	 */
	virtual void End(int dontkill = -1);

	/**
	 * The thread body.
	 * Put here your thread code.
	 */
	virtual void Body() = 0;

	/**
	 * Get the thread internal (OS dependent) id.
	 * @return the identifier.
	 */
	int GetIdentifier() { return identifier; }

	/**
	 * This is the actual thread function as required by the OS.
	 * The user is not required to know about its existence.
	 * @param args is a pointer to the argument data.
	 */
#ifdef __WIN32__
	friend static unsigned __stdcall ExecuteThread (void *args);
#else
	friend unsigned ExecuteThread (void *args);
#endif

	/**
	 * Gets the current thread priority.
	 * @return the current thread priority (OS dependent).
	 */
	int GetPriority (void);

	/**
	 * Sets the thread priority.
	 * ACE suggests not to use the priority values directly but rather
	 * using some OS independent methods. For example:
	 * thread.SetPriority (ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, thread.GetPriority());
	 *
	 * @return non-negative on success.
	 */
	int SetPriority (int prio);

	/**
	 * Waits for the thread to complete.
	 * @param timeout [NOT IMPLEMENTED].
	 * @return non-negative on success.
	 */
	int Join (int timeout = 0);

	/**
	 * Post a request of termination.
	 * It can be used intead of End() to ask for termination.
	 * Follow this call by a Join() to wait for thread exit.
	 */
	void AskForEnd (void);

	/**
	 * Allows recycling of the thread after unclean exit.
	 * Call this function to reset the thread shutdown state and
	 * recycle the instance of the class.
	 */
	void CleanState (void);

	/**
	 * Checks whether a request for termination has been posted.
	 * This method should be called periodically from within the 
	 * thread body, in case it returns true the thread should return
	 * from the body. The parent would typically be waiting
	 * for the child to exit cleanly.
	 * @return true if a request is pending.
	 */
	int IsTerminated(void);
};

/**
 * A simple wrapper with a few additions around YARPBareThread.
 */
class YARPThread : public YARPBareThread
{
private:
	YARPThread(const YARPThread& yt) : YARPBareThread(yt) {}
	void operator=(const YARPThread& yt);

public:
	/**
	 * Contructor.
	 */
	YARPThread (void) : YARPBareThread() {}

	/**
	 * Destructor.
	 */
	virtual ~YARPThread(void) {}

	// obsolete: QNX4 stuff, still here for sentimental reasons.
	static void TerminateAll(void);
	static void PrepareForDeath(void);
	static int IsDying(void);
};


/**
 * A template class for the thread local storage.
 * Use this template to declare a thread local storage of type T. The actual
 * data could be accessed through the Content() method.
 * The thread local storage is a memory area only accessible to a specific thread
 * usually through a key mechanism.
 */
template <class T>
class YARPThreadSpecific : public ACE_TSS<ACE_TSS_Type_Adapter <T> >
{
public:
	/**
	 * Constructor.
	 */
	YARPThreadSpecific() : ACE_TSS<ACE_TSS_Type_Adapter <T> >() {}

	/*
	 * Access to the data.
	 * @return a reference to the thread local storage data.
	 */
	T& Content() { return ((*((ACE_TSS<ACE_TSS_Type_Adapter <T> > *)this))->operator T& ()); } 
};

#endif
