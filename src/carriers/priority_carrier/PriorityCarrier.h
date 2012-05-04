// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef PRIORITYCARRIER_INC
#define PRIORITYCARRIER_INC

#include <yarp/os/impl/ModifyingCarrier.h>
#include <yarp/os/impl/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/Semaphore.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PriorityGroup;
            class PriorityCarrier;
        }
    }
}


/**
 *
 * Manager for priority-aware inputs to a given port.
 *
 */
class yarp::os::impl::PriorityGroup : public PeerRecord {
public:
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader,
                                    PriorityCarrier *source);
};


/**
 *
 * Allow priority-based message selection.  Under development.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.priority+level.15
 *
 */
class yarp::os::impl::PriorityCarrier : public yarp::os::impl::ModifyingCarrier {
public:
    PriorityCarrier() {
        group = 0/*NULL*/;
        priorityLevel = timeConstant = 0;
        isInhibitory = false;
    }

    virtual ~PriorityCarrier() {
        if (portName!="") {
            // let peer carriers know I'm gone.
            getPeers().remove(portName,this);
        }
    }

    virtual Carrier *create() {
        return new PriorityCarrier();
    }

    virtual String getName() {
        return "priority";
    }

    virtual String toString() {
        return "priority_carrier";
    }

    virtual bool configure(yarp::os::impl::Protocol& proto);

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);

public:
    double priorityLevel;
    double timeConstant;
    bool isInhibitory;
    String sourceName;

private:
    String portName;
    PriorityGroup *group;

    static ElectionOf<PriorityCarrier,PriorityGroup> *peers;

    static ElectionOf<PriorityCarrier,PriorityGroup>& getPeers();
};


#endif
