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
    stimulation = options.check("s",Value(priorityLevel)).asDouble();
    // Zero stimulation is undefined and will be interpreted as S=P. 
    if(stimulation == 0)
        stimulation = priorityLevel;

    // Negative stimulation will be interpreted as an inhibition 
    isInhibitory = ( stimulation < 0);

    return true;
}

// Decide whether data should be accepted, for real.
bool PriorityGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PriorityCarrier *source) {
   /*
    printf("===============================================================\n");
    printf("Message is from %s\n", source->sourceName.c_str());
    printf("level:%.2f, s:%.2f, tc:%.2f\n", 
            source->priorityLevel, source->stimulation, source->timeConstant);
    */

    // updates message's arrival time 
    double tNow = yarp::os::Time::now();

    // stimulate and update message temporal priority 
    source->stimulate(tNow);

    // now compete! 
    bool accept = (fabs(source->temporalPriority) >= source->priorityLevel);
    for (PeerRecord::iterator it = peerSet.begin(); it!=peerSet.end(); it++) 
    {
        PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(it);
        if( peer != source)
        {
            // depressing other priorities 
            peer->depress(tNow, source->stimulation);
            if(fabs(source->temporalPriority) < fabs(peer->getActualPriority(tNow)))
                accept = false;
       }
    }
    
    // an inhibitory message will never be delivered. It will inhibit 
    // future messages if it win priority fray. 
    if(source->isInhibitory)
        return false; 
    
    /*
    if (accept) {
        printf("\nThis message wins!\n");
    } else {
        printf("\nThis message loses!\n");
    }
    */
    return accept;
}

