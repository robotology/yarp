/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Authors: Nicolò Genesio, Ali Paikan, Tariq Abuhashim
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_ATOMICBUFFER_H
#define YARP_OS_ATOMICBUFFER_H

#include <stdio.h>
#include <iostream>
#include <queue>
#include <yarp/os/Mutex.h>
#include <yarp/os/Semaphore.h>

#include <functional>

namespace yarp {
    namespace os {
    template <class T> class AtomicBuffer;
    template <class T, class TComparison> class AtomicPriorityBuffer;
    }
}

namespace yarp {
namespace os {

/**
 * Class designed for multithreading pipeline processing.
 * Buffer multi-thread safe, that incapsulate a queue.
 **/

template <class T>
class AtomicBuffer
{
protected:
    yarp::os::Mutex bufMutex; //mutex to lock the array, for avoiding race condition
    bool interrupted;
    yarp::os::Semaphore semArray;
    std::queue<T> array;

public:
    AtomicBuffer() : interrupted(false), semArray(0) {}
    virtual ~AtomicBuffer(){}

    /**
     * @brief This function is blocking until this.write() has not been invoked.
     * This means that until there is no new data in the buffer, the read
     * is blocked. Once read the data is removed from the queue.
     * @param data
     * @return true if data has been read successfully, false otherwise.
     */

    virtual bool read(T& data) {

        //wait for new data
        /*
         * Decrement the counter, even if we must wait to do that.  If the counter
         * would decrement below zero, the calling thread must stop and
         * wait for another thread to call Semaphore::post on this semaphore.
         */
        semArray.wait();

        if(interrupted) {
            return false;
        }

        // read
        bufMutex.lock();
        data=array.front();
        array.pop();
        bufMutex.unlock();
        return true;
    }
    /**
     * @brief write, afte pushing the data in the queue it triggers
     * the wake up of the thread that it is waiting on this.read(..)
     * @param data
     * @return true if the data has been written correctly the queue
     */

    virtual bool write(T& data) {
        //write
        bufMutex.lock();
        array.push(data);
        bufMutex.unlock();
        //signal that a new data is ready
        /*
         * Increment the counter.  If another thread is waiting to decrement the
         * counter, it is woken up.
         */
        semArray.post();
        return true;
    }

    /**
     * @brief getNumElements
     * @return the number of elements in the queue
     */
    size_t getNumElements()
    {
        size_t elemNum = 0;
        bufMutex.lock();
        elemNum = array.size();
        bufMutex.unlock();
        return elemNum;
    }

    void interrupt() {
        // This function has to be called in the interrupt() of the thread that uses
        // this buffer
        bufMutex.lock();
        interrupted = true;
        bufMutex.unlock();
        semArray.post();
    }

};

/**
 * Class designed for multithreading pipeline processing.
 * Buffer multi-thread safe, that incapsulate a priority queue.
 * the priority_queue needs the type, and the operator() to do
 * the comparison necessary for ordering the queue.
 **/

template <class T, class TComparison>
class AtomicPriorityBuffer : public AtomicBuffer<T> {

    typedef std::priority_queue<T, std::vector<T>, TComparison> mypq_type;

private:
    mypq_type p_array;

public:
    AtomicPriorityBuffer() : p_array(TComparison()) {}

    virtual bool read(T& data) {

        //wait
        AtomicBuffer<T>::semArray.wait();
        if(AtomicBuffer<T>::interrupted) {
            return false;
        }

        // read
        AtomicBuffer<T>::bufMutex.lock();
        data = p_array.top();
//        yDebug()<<"Priority access";//ok
        p_array.pop();
        AtomicBuffer<T>::bufMutex.unlock();
        return true;
    }

    virtual bool write(T& data) {
        //write
        AtomicBuffer<T>::bufMutex.lock();
        p_array.push(data);
        AtomicBuffer<T>::bufMutex.unlock();
        //signal
        AtomicBuffer<T>::semArray.post();
        return true;
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ATOMICBUFFER_H
