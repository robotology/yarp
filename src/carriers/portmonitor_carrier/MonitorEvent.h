// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef MONITOREVENT_INC
#define MONITOREVENT_INC

#include <yarp/os/ConstString.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <vector>

/*
namespace yarp {
    namespace os {
        class MonitorEvent;
        class MonitorEventRecord;
    }
}
*/

class MonitorBinding;

class MonitorEvent {
public:    
    MonitorEvent(const char* _name, MonitorBinding* _owner, double lf=-1.0) {
        if(_name) name = _name;
        owner = _owner;
        lifetime = lf;              // default: infinit life time
        create_time = yarp::os::Time::now();
    }
    
    MonitorBinding* owner;          // event's owner
    yarp::os::ConstString name;     // event's symbolic name
    double lifetime;                // event's life time in second. negative value means infinit life time. 
    double create_time;             // event's creation or updating time
};


/**
 *  A singleton class to record the port monitor events 
 */
class MonitorEventRecord {
public:
    typedef std::vector<MonitorEvent> vector_type;
    typedef vector_type::iterator iterator;
    typedef vector_type::const_iterator const_iterator;
     
    void setEvent(const char* name, MonitorBinding* owner, double lifetime=-1.0) {
        // if event already exisits just update the create_time and lifetime
        MonitorEventRecord::iterator itr = findEvent(name, owner);
        if(itr != events.end()) 
        {
            (*itr).create_time = yarp::os::Time::now();
            (*itr).lifetime = lifetime;
            return;
        }   
        events.push_back(MonitorEvent(name, owner, lifetime));
        return;
    }

    void unsetEvent(const char* name, MonitorBinding* owner) {
        MonitorEventRecord::iterator itr = findEvent(name, owner);
        if(itr == events.end())
            return;
        events.erase(itr);
        return;
    }

    bool hasEvent(const char* name){
        MonitorEventRecord::iterator itr;
        for(itr=events.begin(); itr<events.end(); itr++)
            if((*itr).name == name)
            {
                if((*itr).lifetime < 0.0)
                    return true;
                if((yarp::os::Time::now() - (*itr).create_time) < (*itr).lifetime)
                    return true;
                events.erase(itr);          // remove expired event
                return false;
            }
        return false;       
    }

    void lock() { mutex.wait(); }
    void unlock() { mutex.post(); }

    static MonitorEventRecord& getInstance(void) {
        static MonitorEventRecord __instance_MonitorEventRecord;
        return __instance_MonitorEventRecord;
    }

private:
    MonitorEventRecord() { }
    MonitorEventRecord(MonitorEventRecord const &);
    void operator=(MonitorEventRecord const &);

    MonitorEventRecord::iterator findEvent(const char* name, MonitorBinding* owner) {
        MonitorEventRecord::iterator itr;
        for(itr=events.begin(); itr<events.end(); itr++)
            if (((*itr).name == name) && ((*itr).owner == owner))
                return itr;
        return events.end();
    }
    

private:
    vector_type events;
    yarp::os::Semaphore mutex; 

};

#endif //MONITOREVENT_INC


