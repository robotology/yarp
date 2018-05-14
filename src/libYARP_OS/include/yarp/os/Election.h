/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_ELECTION_H
#define YARP_OS_ELECTION_H

#include <string>
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
        return nullptr;
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

    typedef typename std::map<std::string, PR> map_type;
    map_type nameMap;
    long ct;

    PR *getRecordRaw(const std::string& key, bool create = false) {
        typename map_type::iterator entry = nameMap.find(key);
        if (entry == nameMap.end() && create) {
            nameMap[key] = PR();
            entry = nameMap.find(key);
        }
        if (entry == nameMap.end()) {
            return nullptr;
        }
        return &(entry->second);
    }
 public:
    ElectionOf() : mutex(1) {
        ct = 0;
    }
    virtual ~ElectionOf() {}

    PR *add(const std::string& key, typename PR::peer_type *entity) {
        mutex.wait();
        ct++;
        PR *rec = getRecordRaw(key, true);
        yAssert(rec);
        rec->add(entity);
        mutex.post();
        return rec;
    }
    void remove(const std::string& key, typename PR::peer_type *entity) {
        mutex.wait();
        ct++;
        PR *rec = getRecordRaw(key, false);
        yAssert(rec);
        rec->remove(entity);
        mutex.post();
    }

    typename PR::peer_type *getElect(const std::string& key) {
        mutex.wait();
        PR *rec = getRecordRaw(key, false);
        mutex.post();
        if (rec) {
            return rec->getFirst();
        }
        return nullptr;
    }

    PR *getRecord(const std::string& key) {
        mutex.wait();
        PR *rec = getRecordRaw(key, false);
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
    PR *add(const std::string& key, T *entity) {
        return Election<PR>::add(key, entity);
    }

    void remove(const std::string& key, T *entity) {
        Election<PR>::remove(key, entity);
    }

    T *getElect(const std::string& key) {
        return (T *)Election<PR>::getElect(key);
    }

    PR *getRecord(const std::string& key) {
        return Election<PR>::getRecord(key);
    }

    void lock() { Election<PR>::lock(); }

    void unlock() { Election<PR>::unlock(); }
};
*/

#endif // YARP_OS_ELECTION_H
