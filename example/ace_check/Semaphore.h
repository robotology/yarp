/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef _ACECHECK_SEMAPHORE_
#define _ACECHECK_SEMAPHORE_


/**
 * A class for thread synchronization and mutual exclusion.
 * A semaphore has an internal counter.  Multiple threads can
 * safely increment or decrement that counter.  If one thread
 * attempts to decrement the counter below zero, it must wait
 * for another thread to first increment it.  This is a 
 * useful primitive for regulating thread interaction.
 */
class Semaphore {
public:

    /**
     * Constructor.  Sets the initial value of the counter.
     * @param initialCount initial value of the counter
     */
    Semaphore(int initialCount = 1);

    /**
     * Destructor.
     */
    virtual ~Semaphore();

    /**
     * Decrement the counter, even if we must wait to do that.  If the counter
     * would decrement below zero, the calling thread must stop and
     * wait for another thread to call Semaphore::post on this semaphore.
     */
    void wait();


    /**
     * Decrement the counter, unless that would require waiting.  If the counter
     * would decrement below zero, this method simply returns without doing
     * anything.
     * @return true if the counter was decremented
     */
    bool check();

    /**
     * Increment the counter.  If another thread is waiting to decrement the
     * counter, it is woken up.
     */
    void post();


private:
    void *implementation;
};

#endif

