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
#include <yarp/os/impl/PlatformMap.h>
#include <yarp/os/Log.h>
#include <stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PeerRecord;
            template <class T> class Election;
            template <class T, class PR> class ElectionOf;
        }
    }
}

class yarp::os::impl::PeerRecord {
public:
    PLATFORM_MAP(void *,bool) peerSet;
    typedef PLATFORM_MAP(void *,bool)::iterator iterator;
    typedef PLATFORM_MAP(void *,bool)::const_iterator const_iterator;
    
    PeerRecord() {
    }
    
    PeerRecord(const PeerRecord& alt) {
    }
    
    void add(void *entity) {
        PLATFORM_MAP_SET(peerSet,entity,true);
    }
    
    void remove(void *entity) {
        PLATFORM_MAP_UNSET(peerSet,entity);
    }
    
    void *getFirst() {
        if (peerSet.begin()!=peerSet.end()) {
            return PLATFORM_MAP_ITERATOR_FIRST(peerSet.begin());
        }
        return NULL;
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
class yarp::os::impl::Election {
private:
    typedef void *voidPtr;

    yarp::os::Semaphore mutex;

    PLATFORM_MAP(yarp::os::ConstString,PR) nameMap;
    long ct;

    PR *getRecordRaw(const yarp::os::ConstString& key, bool create = false) {
        PLATFORM_MAP_ITERATOR_IN_TEMPLATE(yarp::os::ConstString,PR,entry);
        int result = PLATFORM_MAP_FIND(nameMap,key,entry);
        if (result==-1 && create) {
            PLATFORM_MAP_SET(nameMap,key,PR());
            result = PLATFORM_MAP_FIND(nameMap,key,entry);
        }
        if (result==-1) {
            return NULL;
        }
        return &(PLATFORM_MAP_ITERATOR_SECOND(entry));
    }
 public:
    Election() : mutex(1) {
        ct = 0;
    }
    virtual ~Election() {}

    PR *add(const yarp::os::ConstString& key, void *entity) {
        mutex.wait();
        ct++;
        PR *rec = getRecordRaw(key,true);
        YARP_ASSERT(rec!=NULL);
        rec->add(entity);
        mutex.post();
        return rec;
    }
    void remove(const yarp::os::ConstString& key, void *entity) {
        mutex.wait();
        ct++;
        PeerRecord *rec = getRecordRaw(key,false);
        YARP_ASSERT(rec!=NULL);
        rec->remove(entity);
        if (rec->getFirst()==NULL) {
            // PeerRecords persist regardless
        }
        mutex.post();    
    }

    void *getElect(const yarp::os::ConstString& key) {
        mutex.wait();
        PeerRecord *rec = getRecordRaw(key,false);
        mutex.post();
        if (rec!=NULL) {
            return rec->getFirst();
        }
        return NULL;
    }

    PR *getRecord(const yarp::os::ConstString& key) {
        mutex.wait();
        PeerRecord *rec = getRecordRaw(key,false);
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
template <class T, class PR>
class yarp::os::impl::ElectionOf : protected Election<PR> {
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

#endif

