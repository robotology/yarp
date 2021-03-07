/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PUBLISHER_LATCHED_H
#define YARP_OS_PUBLISHER_LATCHED_H

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Publisher.h>

namespace yarp {
namespace os {

/**
 * A port specialized for publishing data of a constant type on a topic.
 * `Latched` means that the last published message is saved internally and automatically
 * sent to any future subscribers that connects to the publisher.
 *
 * \sa yarp::os::PublisherLatched
 */
template <class TT>
class PublisherLatched : public yarp::os::Publisher<TT>, public yarp::os::PeriodicThread
{
    private:
    size_t           m_currently_connected=0;
    yarp::os::Port*  m_p=nullptr;
    std::mutex       m_mutex;

    public:
    PublisherLatched(const std::string& name = "") : yarp::os::Publisher<TT>(name), yarp::os::PeriodicThread(0.010)
    {
        m_p = &(this->asPort());
        this->start();
    }

    virtual ~PublisherLatched()
    {
        this->stop();
    }

    TT& prepare()
    {
        std::lock_guard<std::mutex> lock (m_mutex);
        return this->buffer().prepare();
    }

    public:
    void run() override
    {
        size_t cc = m_p->getOutputCount();
        if (cc != m_currently_connected)
        {
            m_mutex.lock();
            TT& data = this->buffer().prepare();
            // bool b= this->unprepare();
            // if (b)
            if (cc != 0)
            {
               //yDebug() << "Latched!";
               this->write();
            }
            m_currently_connected = cc;
            m_mutex.unlock();
        }
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PUBLISHER_LATCHED_H
