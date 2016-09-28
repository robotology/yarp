/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/numeric.h>
#include <yarp/os/MessageStack.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>

#include <list>
#include <deque>

using namespace yarp::os;

class MessageStackHelper;
class MessageStackThread : public Thread {
public:
    MessageStackHelper& helper;

    MessageStackThread(MessageStackHelper& helper) : helper(helper) {
    }

    void run();
};

class MessageStackHelper {
private:
    std::list<MessageStackThread *> threads;
    std::deque<Bottle> msgs;
    Semaphore mutex;
    Semaphore produce;
    size_t max_threads;
    int available_threads;
    PortReader& owner;
    bool active;

public:
    MessageStackHelper(int max_threads, PortReader& owner) : mutex(1), produce(0), owner(owner) {
        this->max_threads = (size_t)max_threads;
        available_threads = 0;
        active = true;
    }

    void clear() {
        active = false;
        for (size_t i=0; i<threads.size(); i++) {
            produce.post();
        }
        for (std::list<MessageStackThread *>::iterator it = threads.begin();
             it != threads.end(); it++) {
            (*it)->stop();
            delete (*it);
            *it = YARP_NULLPTR;
        }
        threads.clear();
        msgs.clear();
        active = true;
    }

    void stack(PortWriter& msg, const ConstString& tag) {
        mutex.wait();
        msgs.push_back(Bottle());
        if (tag!="") {
            Bottle b;
            b.read(msg);
            Bottle& back = msgs.back();
            back.clear();
            back.addString(tag);
            back.append(b);
        } else {
            msgs.back().read(msg);
        }
        if (available_threads==0) {
            if (threads.size()<max_threads || max_threads == 0) {
                available_threads++;
                threads.push_back(new MessageStackThread(*this));
                threads.back()->start();
            }
        }
        available_threads--;
        mutex.post();
        produce.post();
    }

    bool process() {
        produce.wait();
        if (!active) return false;
        mutex.wait();
        Bottle b = msgs.front();
        msgs.pop_front();
        mutex.post();
        DummyConnector con;
        b.write(con.getWriter());
        owner.read(con.getReader());
        mutex.wait();
        available_threads++;
        mutex.post();
        return active;
    }

    bool isOwner(PortReader& owner) {
        return &(this->owner) == &owner;
    }
};

void MessageStackThread::run() {
    while (helper.process()) {
        // forever
    }
}

#define HELPER(x) (*((MessageStackHelper *) x))

MessageStack::MessageStack(int max_threads) {
    this->max_threads = max_threads;
    implementation = YARP_NULLPTR;
}

MessageStack::~MessageStack() {
    if (!implementation) return;
    HELPER(implementation).clear();
    delete &HELPER(implementation);
    implementation = YARP_NULLPTR;
}

void MessageStack::attach(PortReader& owner) {
    if (implementation) {
        if (HELPER(implementation).isOwner(owner)) return;
        delete &HELPER(implementation);
        implementation = YARP_NULLPTR;
    }
    implementation = new MessageStackHelper(max_threads,owner);
    yAssert(implementation);
}

void MessageStack::stack(PortWriter& msg, const ConstString& tag) {
    if (!implementation) return;
    HELPER(implementation).stack(msg,tag);
}
