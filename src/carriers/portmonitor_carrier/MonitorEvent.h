/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MONITOREVENT_INC
#define MONITOREVENT_INC

#include <string>
#include <mutex>
#include <yarp/os/Time.h>
#include <vector>


class MonitorBinding;

class MonitorEvent
{
public:
    MonitorEvent(const char* _name, MonitorBinding* _owner, double lf=-1.0) :
            owner(_owner),
            name(_name ? _name : ""),
            lifetime(lf),  // default: infinite life time
            create_time(yarp::os::Time::now())
    {
    }

    MonitorBinding* owner;          // event's owner
    std::string name;     // event's symbolic name
    double lifetime;                // event's life time in second. negative value means infinite life time.
    double create_time;             // event's creation or updating time
};


/**
 *  A singleton class to record the port monitor events
 */
class MonitorEventRecord
{
public:
    typedef std::vector<MonitorEvent> vector_type;
    typedef vector_type::iterator iterator;
    typedef vector_type::const_iterator const_iterator;

    void setEvent(const char* name, MonitorBinding* owner, double lifetime=-1.0)
    {
        // if event already exists just update the create_time and lifetime
        MonitorEventRecord::iterator itr = findEvent(name, owner);
        if(itr != events.end())
        {
            (*itr).create_time = yarp::os::Time::now();
            (*itr).lifetime = lifetime;
            return;
        }
        events.push_back(MonitorEvent(name, owner, lifetime));
    }

    void unsetEvent(const char* name, MonitorBinding* owner)
    {
        MonitorEventRecord::iterator itr = findEvent(name, owner);
        if(itr == events.end()) {
            return;
        }
        events.erase(itr);
    }

    bool hasEvent(const char* name)
    {
        MonitorEventRecord::iterator itr;
        for(itr=events.begin(); itr<events.end(); itr++) {
            if((*itr).name == name)
            {
                if((*itr).lifetime < 0.0) {
                    return true;
                }
                if((yarp::os::Time::now() - (*itr).create_time) < (*itr).lifetime) {
                    return true;
                }
                events.erase(itr);          // remove expired event
                return false;
            }
        }
        return false;
    }

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
    }

    static MonitorEventRecord& getInstance()
    {
        static MonitorEventRecord __instance_MonitorEventRecord;
        return __instance_MonitorEventRecord;
    }

private:
    MonitorEventRecord() = default;
    MonitorEventRecord(MonitorEventRecord const &) = delete;
    void operator=(MonitorEventRecord const &) = delete;

    MonitorEventRecord::iterator findEvent(const char* name, MonitorBinding* owner)
    {
        MonitorEventRecord::iterator itr;
        for(itr=events.begin(); itr<events.end(); itr++) {
            if (((*itr).name == name) && ((*itr).owner == owner)) {
                return itr;
            }
        }
        return events.end();
    }


private:
    vector_type events;
    std::mutex mutex;
};

#endif // MONITOREVENT_INC
