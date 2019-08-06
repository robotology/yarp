/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MessageStack.h>

#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Log.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

#include <deque>
#include <list>
#include <mutex>

using namespace yarp::os;

class MessageStackHelper;
class MessageStackThread : public Thread
{
public:
    MessageStackHelper& helper;

    MessageStackThread(MessageStackHelper& helper) :
            helper(helper)
    {
    }

    void run() override;
};

class MessageStackHelper
{
private:
    std::list<MessageStackThread*> threads;
    std::deque<Bottle> msgs;
    std::mutex mutex;
    Semaphore produce;
    size_t max_threads;
    int available_threads;
    PortReader& owner;
    bool active;

public:
    MessageStackHelper(int max_threads, PortReader& owner) :
            mutex(), produce(0), owner(owner)
    {
        this->max_threads = (size_t)max_threads;
        available_threads = 0;
        active = true;
    }

    void clear()
    {
        active = false;
        for (size_t i = 0; i < threads.size(); i++) {
            produce.post();
        }
        for (auto& thread : threads) {
            thread->stop();
            delete thread;
            thread = nullptr;
        }
        threads.clear();
        msgs.clear();
        active = true;
    }

    void stack(PortWriter& msg, const std::string& tag)
    {
        mutex.lock();
        msgs.emplace_back();
        if (!tag.empty()) {
            Bottle b;
            b.read(msg);
            Bottle& back = msgs.back();
            back.clear();
            back.addString(tag);
            back.append(b);
        } else {
            msgs.back().read(msg);
        }
        if (available_threads == 0) {
            if (threads.size() < max_threads || max_threads == 0) {
                available_threads++;
                threads.push_back(new MessageStackThread(*this));
                threads.back()->start();
            }
        }
        available_threads--;
        mutex.unlock();
        produce.post();
    }

    bool process()
    {
        produce.wait();
        if (!active) {
            return false;
        }
        mutex.lock();
        Bottle b = msgs.front();
        msgs.pop_front();
        mutex.unlock();
        DummyConnector con;
        b.write(con.getWriter());
        owner.read(con.getReader());
        mutex.lock();
        available_threads++;
        mutex.unlock();
        return active;
    }

    bool isOwner(PortReader& owner)
    {
        return &(this->owner) == &owner;
    }
};

void MessageStackThread::run()
{
    while (helper.process()) {
        // forever
    }
}

#define HELPER(x) (*((MessageStackHelper*)(x)))

MessageStack::MessageStack(int max_threads)
{
    this->max_threads = max_threads;
    implementation = nullptr;
}

MessageStack::~MessageStack()
{
    if (implementation == nullptr) {
        return;
    }
    HELPER(implementation).clear();
    delete &HELPER(implementation);
    implementation = nullptr;
}

void MessageStack::attach(PortReader& owner)
{
    if (implementation != nullptr) {
        if (HELPER(implementation).isOwner(owner)) {
            return;
        }
        delete &HELPER(implementation);
        implementation = nullptr;
    }
    implementation = new MessageStackHelper(max_threads, owner);
    yAssert(implementation);
}

void MessageStack::stack(PortWriter& msg, const std::string& tag)
{
    if (implementation == nullptr) {
        return;
    }
    HELPER(implementation).stack(msg, tag);
}
