// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ELECTION_
#define _YARP2_ELECTION_

#include <yarp/os/impl/String.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Election;
            template <class T> class ElectionOf;
        }
    }
}

/**
 * Pick one of a set of peers to be "active".  Used for situations
 * where a manager is required for some resource used by several
 * peers, but it doesn't matter which peer plays that role.
 */
class yarp::os::impl::Election {
private:
    typedef void *voidPtr;

    class PeerRecord {
    protected:
        ACE_Hash_Map_Manager<void *,bool,ACE_Null_Mutex> peerSet;

    public:
        PeerRecord() {
        }

        PeerRecord(const PeerRecord& alt) {
        }

        void add(void *entity) {
            peerSet.bind(entity,true);
        }

        void remove(void *entity) {
            peerSet.unbind(entity);
        }

        void *getFirst() {
            if (peerSet.begin()!=peerSet.end()) {
                return (*(peerSet.begin())).ext_id_;
            }
            return NULL;
        }
    };


    ACE_Hash_Map_Manager<YARP_KEYED_STRING,PeerRecord,ACE_Null_Mutex> nameMap;
    long ct;

    PeerRecord *getRecord(const String& key, bool create = false);

 
public:
    Election();
    virtual ~Election();

    void add(const String& key, void *entity);
    void remove(const String& key, void *entity);
    void *getElect(const String& key);
    long getEventCount();
};

/**
 * Type-safe wrapper for the Election class.
 */
template <class T>
class yarp::os::impl::ElectionOf : protected Election {
public:
    void add(const String& key, T *entity) {
        Election::add(key, entity);
    }

    void remove(const String& key, T *entity) {
        Election::remove(key, entity);
    }

    T *getElect(const String& key) {
        return (T *)Election::getElect(key);
    }

};

#endif

