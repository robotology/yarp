/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/MessageStack.h>

#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Log.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>

#include <condition_variable>
#include <deque>
#include <list>
#include <mutex>

using namespace yarp::os;

namespace {

class MessageStackHelper;

class MessageStackThread : public Thread
{
public:
    MessageStackHelper& helper;

    explicit MessageStackThread(MessageStackHelper& helper) :
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
    std::condition_variable cv;
    size_t max_threads;
    int available_threads;
    PortReader& owner;
    bool active;

public:
    MessageStackHelper(size_t max_threads, PortReader& owner) :
            owner(owner)
    {
        this->max_threads = max_threads;
        available_threads = 0;
        active = true;
    }

    void clear()
    {
        active = false;
        cv.notify_all();
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
        std::unique_lock<std::mutex> lock(mutex);
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
        cv.notify_one();
    }

    bool process()
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]{return !msgs.empty() || !active;});
        if (!active) {
            return false;
        }
        Bottle b = msgs.front();
        msgs.pop_front();
        lock.unlock();
        DummyConnector con;
        b.write(con.getWriter());
        owner.read(con.getReader());
        lock.lock();
        available_threads++;
        lock.unlock();
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


} // namespace



class MessageStack::Private
{
public:
    size_t max_threads{0};
    MessageStackHelper* helper = nullptr;

    explicit Private(size_t max_threads) :
            max_threads(max_threads)
    {
    }

    ~Private()
    {
        if (helper == nullptr) {
            return;
        }
        helper->clear();
        delete helper;
    }

    void attach(PortReader& owner) {
        if (helper != nullptr) {
            if (helper->isOwner(owner)) {
                return;
            }
            delete helper;
            helper = nullptr;
        }
        helper = new MessageStackHelper(max_threads, owner);
    }

    void stack(PortWriter& msg, const std::string& tag)
    {
        if (helper == nullptr) {
            return;
        }
        helper->stack(msg, tag);
    }
};



MessageStack::MessageStack(size_t max_threads) :
        mPriv(new Private(max_threads))
{
}

MessageStack::~MessageStack()
{
    delete mPriv;
}

void MessageStack::attach(PortReader& owner)
{
    mPriv->attach(owner);
}

void MessageStack::stack(PortWriter& msg, const std::string& tag)
{
    mPriv->stack(msg, tag);
}
