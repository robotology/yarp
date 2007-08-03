// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/Election.h>
#include <yarp/Logger.h>

using namespace yarp;


Election::Election() {
    ct = 0;
}


Election::~Election() {
}


Election::PeerRecord *Election::getRecord(const String& key, bool create) {
    ACE_Hash_Map_Entry<String,Election::PeerRecord> *entry = NULL;
    int result = nameMap.find(key,entry);
    if (result==-1 && create) {
        nameMap.bind(key,Election::PeerRecord());
        result = nameMap.find(key,entry);
    }
    if (result==-1) {
        return NULL;
    }
    return &(entry->int_id_);
}

void Election::add(const String& key, void *entity) {
    ct++;
    //printf("add called with %s\n", key.c_str());
    PeerRecord *rec = getRecord(key,true);
    YARP_ASSERT(rec!=NULL);
    rec->add(entity);
}


void Election::remove(const String& key, void *entity) {
    ct++;
    PeerRecord *rec = getRecord(key,false);
    YARP_ASSERT(rec!=NULL);
    rec->remove(entity);
    if (rec->getFirst()==NULL) {
        //nameMap.unbind(key); // ACE wants a lot before this can happen
    }
}


void *Election::getElect(const String& key) {
    PeerRecord *rec = getRecord(key,false);
    if (rec!=NULL) {
        return rec->getFirst();
    }
    return NULL;
}


long Election::getEventCount() {
    return ct;
}


