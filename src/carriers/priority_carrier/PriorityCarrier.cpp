// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "PriorityCarrier.h"

using namespace yarp::os;
using namespace yarp::os::impl;

ElectionOf<PriorityCarrier,PriorityGroup> *PriorityCarrier::peers = NULL;

// Make a singleton manager for finding peer carriers.
ElectionOf<PriorityCarrier,PriorityGroup>& PriorityCarrier::getPeers() {
    NetworkBase::lock();
    if (peers==NULL) {
        peers = new ElectionOf<PriorityCarrier,PriorityGroup>;
        NetworkBase::unlock();
        YARP_ASSERT(peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted.
bool PriorityCarrier::acceptIncomingData(yarp::os::ConnectionReader& reader) {
    getPeers().lock();
    YARP_ASSERT(group);
    bool result = group->acceptIncomingData(reader,this);
    getPeers().unlock();
    return result;
}

// Read connection settings.
bool PriorityCarrier::configure(yarp::os::impl::Protocol& proto) {
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    group = getPeers().add(portName,this);
    if (!group) return false;

    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    priorityLevel = options.check("level",Value(1.0)).asDouble();
    timeConstant = options.check("tc",Value(1.0)).asDouble();
    isInhibitory = options.check("inhibit",Value(0)).asInt()!=0;
    return true;
}

// Decide whether data should be accepted, for real.
bool PriorityGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PriorityCarrier *source) {
    printf("===============================================================\n");
    printf("A message has arrived. Thinking about whether to let it through\n");
    printf("Message is from %s\n", source->sourceName.c_str());
    printf("Priority level %g\n", source->priorityLevel);
    printf("All connections:");
    for (PeerRecord::iterator it = peerSet.begin(); it!=peerSet.end(); it++) {
        PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(it);
        printf(" %s:%g", peer->sourceName.c_str(), peer->priorityLevel);
    }
    printf("\n");
    printf("Eh, I'm going to ignore all that and just let every second message through :-)\n");
    
    static bool accept = true;
    accept = !accept;
    if (accept) {
        printf("This message wins!\n");
    } else {
        printf("This message loses!\n");
    }
    return accept;
}

