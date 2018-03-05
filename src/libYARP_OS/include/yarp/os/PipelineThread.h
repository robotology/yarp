/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PIPELINETHREAD_H
#define YARP_OS_PIPELINETHREAD_H
#include <yarp/os/Thread.h>
#include <yarp/os/AtomicBuffer.h>

namespace yarp {
    namespace os {
    template <class T1, class T2> class PipelineThread;
    }
}

namespace yarp {
namespace os {

/**
 * \ingroup key_class
 * Class designed for multithreading pipeline processing.
 */

template <class T1, class T2>
class PipelineThread : public Thread
{

public:

    /**
     * @brief PipelineThread constructor, to exploit the functionality that this class has been designed to,
     * bufferIn and bufferOut has to be shared between the different "PipelineThreads",
     * passing them by reference in the constructor.
     * The bufferOut of one thread has to be the bufferIn of the thread representing the next
     * step of the pipeline.
     * @param _bufferIn, input buffer.
     * @param _bufferOut, output buffer.
     */
    PipelineThread(AtomicBuffer<T1> &_bufferIn, AtomicBuffer<T2> &_bufferOut) :
        interrupted(false), countProcessed(0), bufferIn(&_bufferIn), bufferOut(&_bufferOut)
    {
    }

    ~PipelineThread()
    {
        if (!interrupted)
        {
            close();
        }
    }

    /**
     * @brief
     * @return return the number of element processed
     */
    unsigned int getCountProcessed(){
        return countProcessed;
    }

    /**
     * @brief Interrupt the input buffer.
     */

    virtual void interrupt() {
        interrupted = true;
        if (bufferIn)
        {
            bufferIn->interrupt();
        }
    }

    /**
     * @brief It is a working example for the case
     * one input buffer and one output buffer. If needed it can be
     * overridden for the specific use case, otherwise it will necessary
     * only to implement processData(...).
     */

    virtual void run()
    {
        // We continue to read, process and write data while it is not interrupted and there is data to process.
        // The interrupted flag is setted to true in the function interrupt().
        // interrupt() is called by thread.close() when we close the module.
        while(!interrupted) {
            T1 dataIn;
            if(bufferIn->read(dataIn)){
                // As soon as new data is ready, process the data and write it to the output buffer
                T2 dataOut;
                this->processData(dataIn, dataOut);
                bufferOut->write(dataOut);
                countProcessed++;
                yarp::os::Thread::yield();

            }

        }
    }

    /**
     * @brief close, stop the thread.
     */
    virtual void close() {
        interrupt();
        Thread::stop();
        countProcessed = 0;
        interrupted = false;
    }

protected:

    virtual void processData(T1& dataIn, T2& dataOut)
    {
        // This function has to be overridden, implementing the
        // own processData.
        YARP_UNUSED(dataIn);
        YARP_UNUSED(dataOut);
        return;
    }

protected:
    bool interrupted;
    unsigned int countProcessed;
    AtomicBuffer<T1> *bufferIn;
    AtomicBuffer<T2> *bufferOut;

};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PIPELINETHREAD_H
