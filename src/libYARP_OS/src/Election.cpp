// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/Election.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;
using namespace yarp::os::impl;


Election::Election() : mutex(1) {
    ct = 0;
}


Election::~Election() {
}


Election::PeerRecord *Election::getRecord(const String& key, bool create) {
    ACE_Hash_Map_Entry<YARP_KEYED_STRING,Election::PeerRecord> *entry = NULL;

    /*
    // debug code - remove after september 2010
    static int code = 0;
    printf("[");
    int c = 0;
    int d = code;
    code = (code+1)%10;
    for (ACE_Hash_Map_Manager<YARP_KEYED_STRING,PeerRecord,ACE_Null_Mutex>::iterator it=nameMap.begin(); it!=nameMap.end(); it++) {
        printf("%d.%d.%s ", c, d, (*it).ext_id_.c_str());
        c++;
    }
    printf("]");
    */

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
    mutex.wait();
    ct++;
    //printf("add called with %s %ld\n", key.c_str(), (long int)entity);
    PeerRecord *rec = getRecord(key,true);
    YARP_ASSERT(rec!=NULL);
    rec->add(entity);
    mutex.post();
}


void Election::remove(const String& key, void *entity) {
    mutex.wait();
    ct++;
    //printf("remove called with %s %ld\n", key.c_str(), (long int)entity);
    PeerRecord *rec = getRecord(key,false);
    YARP_ASSERT(rec!=NULL);
    rec->remove(entity);
    if (rec->getFirst()==NULL) {
        //nameMap.unbind(key); // ACE wants a lot before this can happen
    }
    mutex.post();
}


void *Election::getElect(const String& key) {
    mutex.wait();
    PeerRecord *rec = getRecord(key,false);
    mutex.post();
    if (rec!=NULL) {
        return rec->getFirst();
    }
    return NULL;
}


long Election::getEventCount() {
    return ct;
}


