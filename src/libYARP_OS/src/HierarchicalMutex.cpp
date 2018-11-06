/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/HierarchicalMutex.h>
#include <climits>

using yarp::os::HierarchicalMutex;

class HierarchicalMutex::Private
{
    std::mutex internal_mutex;
    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;
    static thread_local unsigned long this_thread_hierarchy_value;

    void check_for_hierarchy_violation()
    {
        if(this_thread_hierarchy_value <= hierarchy_value)
        {
            throw std::logic_error("mutex hierarchy violated");
        }
    }
    void update_hierarchy_value()
    {
        previous_hierarchy_value=this_thread_hierarchy_value;
        this_thread_hierarchy_value=hierarchy_value;
    }
public:
    explicit Private(unsigned long value):
        hierarchy_value(value),
        previous_hierarchy_value(0)
    {}
    void lock()
    {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        update_hierarchy_value();
    }
    void unlock()
    {
        this_thread_hierarchy_value=previous_hierarchy_value;
        internal_mutex.unlock();
    }
    bool try_lock()
    {
        check_for_hierarchy_violation();
        if(!internal_mutex.try_lock())
            return false;
        update_hierarchy_value();
        return true;
    }
};

thread_local unsigned long
    HierarchicalMutex::Private::this_thread_hierarchy_value(ULONG_MAX);

HierarchicalMutex::HierarchicalMutex(unsigned long value) :
        mPriv(new Private(value))
{
}

HierarchicalMutex::~HierarchicalMutex()
{
    delete mPriv;
}

void HierarchicalMutex::lock()
{
    mPriv->lock();
}

bool HierarchicalMutex::try_lock()
{
    return mPriv->try_lock();
}

void HierarchicalMutex::unlock()
{
    mPriv->unlock();
}
