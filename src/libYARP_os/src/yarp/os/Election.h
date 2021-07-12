/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_ELECTION_H
#define YARP_OS_ELECTION_H

#include <yarp/os/Log.h>

#include <map>
#include <mutex>
#include <string>

namespace yarp {
namespace os {

template <class T>
class PeerRecord
{
public:
    typedef T peer_type;
    typedef std::map<T*, bool> map_type;
    map_type peerSet;
    typedef typename map_type::iterator iterator;
    typedef typename map_type::const_iterator const_iterator;

    PeerRecord() = default;
    PeerRecord(const PeerRecord& alt) = default;

    void add(T* entity)
    {
        peerSet[entity] = true;
    }

    void remove(T* entity)
    {
        peerSet.erase(entity);
    }

    T* getFirst()
    {
        if (peerSet.begin() != peerSet.end()) {
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
class ElectionOf
{
private:
    typedef void* voidPtr;

    std::mutex mutex;

    typedef typename std::map<std::string, PR> map_type;
    map_type nameMap;
    long ct{0};

    PR* getRecordRaw(const std::string& key, bool create = false)
    {
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
    ElectionOf() = default;
    virtual ~ElectionOf() = default;

    PR* add(const std::string& key, typename PR::peer_type* entity)
    {
        mutex.lock();
        ct++;
        PR* rec = getRecordRaw(key, true);
        yAssert(rec);
        rec->add(entity);
        mutex.unlock();
        return rec;
    }

    void remove(const std::string& key, typename PR::peer_type* entity)
    {
        mutex.lock();
        ct++;
        PR* rec = getRecordRaw(key, false);
        yAssert(rec);
        rec->remove(entity);
        mutex.unlock();
    }

    typename PR::peer_type* getElect(const std::string& key)
    {
        mutex.lock();
        PR* rec = getRecordRaw(key, false);
        mutex.unlock();
        if (rec) {
            return rec->getFirst();
        }
        return nullptr;
    }

    PR* getRecord(const std::string& key)
    {
        mutex.lock();
        PR* rec = getRecordRaw(key, false);
        mutex.unlock();
        return rec;
    }

    long getEventCount()
    {
        return ct;
    }

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_ELECTION_H
