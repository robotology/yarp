// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ELECTION_
#define _YARP2_ELECTION_

#include <yarp/os/ConstString.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>

#include <map>

namespace yarp {
    namespace os {
        template <class T> class PeerRecord;
        template <class PR> class ElectionOf;
    }
}

template <class T> 
class yarp::os::PeerRecord {
public:
    typedef T peer_type;
    typedef std::map<T *, bool> map_type;
    map_type peerSet;
    typedef typename map_type::iterator iterator;
    typedef typename map_type::const_iterator const_iterator;
 
    PeerRecord() {
    }
    
    PeerRecord(const PeerRecord& alt) {
    }
    
    void add(T *entity) {
        peerSet[entity] = true;
    }
    
    void remove(T *entity) {
        peerSet.erase(entity);
    }
    
    T *getFirst() {
        if (peerSet.begin()!=peerSet.end()) {
            return peerSet.begin()->first;
        }
        return 0 /*NULL */;
    }
};


/**
 * Pick one of a set of peers to be "active".  Used for situations
 * where a manager is required for some resource used by several
 * peers, but it doesn't matter which peer plays that role.
 *
 * PR should be a subclass of PeerRecord.
 */
template <class PR> 
class yarp::os::ElectionOf {
private:
    typedef void *voidPtr;

    yarp::os::Semaphore mutex;

    typedef typename std::map<yarp::os::ConstString,PR> map_type;
    map_type nameMap;
    long ct;

    PR *getRecordRaw(const yarp::os::ConstString& key, bool create = false) {
        typename map_type::iterator entry = nameMap.find(key);
        if (entry == nameMap.end() && create) {
            nameMap[key] = PR();
            entry = nameMap.find(key);
        }
        if (entry == nameMap.end()) {
            return 0 /*NULL*/;
        }
        return &(entry->second);
    }
 public:
    ElectionOf() : mutex(1) {
        ct = 0;
    }
    virtual ~ElectionOf() {}

    PR *add(const yarp::os::ConstString& key, typename PR::peer_type *entity) {
        mutex.wait();
        ct++;
        PR *rec = getRecordRaw(key,true);
        YARP_ASSERT(rec);
        rec->add(entity);
        mutex.post();
        return rec;
    }
    void remove(const yarp::os::ConstString& key, typename PR::peer_type *entity) {
        mutex.wait();
        ct++;
        PR *rec = getRecordRaw(key,false);
        YARP_ASSERT(rec);
        rec->remove(entity);
        mutex.post();    
    }

    typename PR::peer_type *getElect(const yarp::os::ConstString& key) {
        mutex.wait();
        PR *rec = getRecordRaw(key,false);
        mutex.post();
        if (rec) {
            return rec->getFirst();
        }
        return NULL;
    }

    PR *getRecord(const yarp::os::ConstString& key) {
        mutex.wait();
        PR *rec = getRecordRaw(key,false);
        mutex.post();
        return rec;
    }

    long getEventCount() {
        return ct;
    }

    void lock() { mutex.wait(); }
    void unlock() { mutex.post(); }
};


/**
 * Type-safe wrapper for the Election class.
 */
/*
template <class T, class PR>
class yarp::os::ElectionOf : protected Election<PR> {
public:
    PR *add(const yarp::os::ConstString& key, T *entity) {
        return Election<PR>::add(key, entity);
    }

    void remove(const yarp::os::ConstString& key, T *entity) {
        Election<PR>::remove(key, entity);
    }

    T *getElect(const yarp::os::ConstString& key) {
        return (T *)Election<PR>::getElect(key);
    }

    PR *getRecord(const yarp::os::ConstString& key) {
        return Election<PR>::getRecord(key);
    }

    void lock() { Election<PR>::lock(); }

    void unlock() { Election<PR>::unlock(); }
};
*/

#endif

