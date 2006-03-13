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
///	$Id: YARPSemaphore.h,v 1.1 2006-03-13 13:35:18 eshuy Exp $
///
///
/*
	paulfitz Mon May 21 13:35:42 EDT 2001
 */

#ifndef YARPSemaphore_INC
#define YARPSemaphore_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/Synch.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPErrorCodes.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif


/**
 * \file YARPSemaphore.h Resources for synchronizing threads.
 */

/**
 * Fake semaphore that does nothing.  Useful as a drop-in replacement
 * for a real semaphore where synchronization is not needed; this can
 * save resources.
 *
 * @see YARPSemaphore
 */

class YARPNullSemaphore
{
private:

public:
	// Assertion fails if insufficient resources at initialization
	YARPNullSemaphore(int initial_count = 1) { ACE_UNUSED_ARG(initial_count); }
	YARPNullSemaphore(const YARPNullSemaphore& yt) { ACE_UNUSED_ARG(yt); }

	virtual ~YARPNullSemaphore() {}

	void BlockingWait() {}
	int PollingWait() { return YARP_OK; }  // returns true if Wait() occurred
	int Wait(int blocking = 1) { ACE_UNUSED_ARG(blocking); return 1; }
	void Post() {}
};

/**
 * Semaphores for synchronizing and sharing resources across threads.
 */

class YARPSemaphore
{
private:
	void *system_resource;

public:
	// Assertion fails if insufficient resources at initialization

	/**
	 * Constructor.
	 *
	 * @param initial_count the initial value of the semaphore,
	 * defaults to 1 (suitable for mutual exclusion).
	 */
	YARPSemaphore(int initial_count=1);

	/**
	 * Copy constructor.
	 *
	 * @param yt the target semaphore to copy.
	 */
	YARPSemaphore(const YARPSemaphore& yt);

	virtual ~YARPSemaphore();

	/**
	 * Waits for the semaphore to have a non-zero value.  If at
	 * the time the method is called the semaphore is zero, the
	 * method will "block" and wait until another thread
	 * increments the semaphore.  When that occurs, the method
	 * will reduce the semaphore value by one and return to the
	 * caller.
	 */
	void BlockingWait();

	/**
	 * Checks if the semaphore has a non-zero value.  If it does,
	 * the method will reduce the semaphore value by one.  The method
	 * always returns immediately, and never waits for the semaphore
	 * to change value.
	 *
	 * @return true (non-zero) iff at the time the method is called
	 * the semaphore is non-zero.
	 */
	int PollingWait();

	/**
	 * Checks if the semaphore has a non-zero value, and
	 * optionally waits if it does not.  When the semaphore is non-zero,
	 * this method reduces it by one.
	 *
	 * @param blocking if this is true (non-zero), then the method
	 * will behave like BlockingWait().
	 *
	 * @return true (non-zero) if the semaphore was reduced by this method.
	 */
	int Wait(int blocking = 1)
	{
		if (blocking) 
		{ 
			BlockingWait(); 
			return 1; 
		}
		else
		{ 
			return PollingWait(); 
		} 
	}

	/**
	 * Increments the semaphore by 1.
	 */
	void Post();
};

/**
 * A simple template class that adds a semaphore to another class.
 * Use this template to add a mutual exclusion semaphore to a generic
 * class <OBJ>. The template provides a public semaphore and two
 * methods to get ownership of the semaphore or to release it.
 */
template <class OBJ> class YARPProtect : public OBJ
{
public:
	/**
	 * The semaphore instance.
	 */
	YARPSemaphore _sema;

	/**
	 * Constructor.
	 * Note that the semaphore is initialized to 1 allowing for
	 * mutual exclusion.
	 */
	YARPProtect () : _sema(1) {}

	/**
	 * Destructor.
	 */
	~YARPProtect () {}

	/**
	 * Wait on the semaphore.
	 * Call this method to decrement the value of the semaphore and
	 * possibly wait for its value to become positive.
	 * @param blocking specifies whether this is an actual request of
	 * decrement or rather just a polling of the semaphore value.
	 * @return if polling, returns 1 if the semaphore can be decremented
	 * without blocking.
	 */
	int lock (int blocking = 1) { return _sema.Wait(blocking); }

	/**
	 * Release the semaphore.
	 * Increments the semaphore and potentially wakes up a waiting
	 * thread.
	 */
	void unlock (void) { _sema.Post(); }
};


/**
 * Event-like sinchronization object.
 * The event is a lightweight semaphore, mostly to be used as an optimization
 * as a replacement for semaphores.
 */
class YARPEvent : public ACE_Event
{
protected:
	YARPEvent (const YARPEvent&);
	void operator= (const YARPEvent&);

public:
	/**
	 * Constructor.
	 * @param manual_reset if not zero the event object must be reset manually by calling
	 * Reset() otherwise the event is automatically Reset() depending on how the object
	 * is signalled: Pulse() or Signal().
	 * @param initial_state whether the event is initially signalled.
	 */
	YARPEvent (int manual_reset = 0, int initial_state = 0) : ACE_Event (manual_reset, initial_state) {}

	/**
	 * Destructor.
	 */
	~YARPEvent () {}

	/**
	* If MANUAL reset
	*    sleep till the event becomes signaled
	*    event remains signaled after Wait() completes.
	* If AUTO reset
	*    sleep till the event becomes signaled
	*    event resets Wait() completes.
	*/
	inline int Wait (void) { return wait(); }

	/**
	* If MANUAL reset
	*    wake up all waiting threads
	*    set to signaled state.
	* If AUTO reset
	*    if no thread is waiting, set to signaled state
	*    if thread(s) are waiting, wake up one waiting thread and
	*    reset event.
	*/
	inline int Signal (void) { return signal(); }

	/**
	* If MANUAL reset
	*    wakeup all waiting threads and
	*    reset event.
	* If AUTO reset
	*    wakeup one waiting thread (if present) and
	*    reset event.
	*/
	inline int Pulse (void) { return pulse(); }

	/**
	* Resets the event object.
	*/
	inline int Reset (void) { return reset(); }
};

#endif
