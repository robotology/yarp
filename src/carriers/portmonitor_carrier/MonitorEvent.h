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
    MonitorEvent(const char* _name, MonitorBinding* _owner) {
        if(_name) name = _name;
        owner = _owner;
    }
    
    MonitorBinding* owner;
    yarp::os::ConstString name;
};


/**
 *  A singleton class to record the port monitor events 
 */
class MonitorEventRecord {
public:
    typedef std::vector<MonitorEvent> vector_type;
    typedef vector_type::iterator iterator;
    typedef vector_type::const_iterator const_iterator;
     
    bool setEvent(const char* name, MonitorBinding* owner) {
        if(findEvent(name, owner) != events.end())
            return false;        
        events.push_back(MonitorEvent(name, owner));
        return true;
    }

    bool unsetEvent(const char* name, MonitorBinding* owner) {
        MonitorEventRecord::iterator itr = findEvent(name, owner);
        if(itr == events.end())
            return true;
        events.erase(itr);
        return true;
    }

    bool hasEvent(const char* name){
        MonitorEventRecord::iterator itr;
        for(itr=events.begin(); itr<events.end(); itr++)
            if((*itr).name == name)
                return true;
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


