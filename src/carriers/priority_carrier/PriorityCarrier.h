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
#include <yarp/os/Time.h>
#include <math.h>


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
        priorityLevel = timeConstant = timeArrival = 0;
        stimulation = 0;
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
  
    void depress(double t, double depression) {
        temporalPriority = getActualPriority(t);
        if(isResting(temporalPriority) 
           && (temporalPriority < 0))
        {
            temporalPriority += fabs(depression);
            if(temporalPriority > 0)
                temporalPriority = 0;
        }
    }

    void stimulate(double t) {

        temporalPriority = getActualPriority(t);
        if(!isResting(temporalPriority))
        {
            temporalPriority += fabs(stimulation);
            // respecting priority ceiling
            if(temporalPriority > priorityLevel)
                temporalPriority = priorityLevel;
            // updating arrival time 
            timeArrival = t;
        }
    }

    //
    // P(t) = Pi * (-exp((t-Tc-Ta)/Tc*5) + 1.0)
    // t:time, Pi: temporal Priority level
    // Tc: reset time, Ta: arrival time
    // 
    double getActualPriority(double t) {
        double actualPriority;
        double dt = t - timeArrival;
        
        if(!isResting(temporalPriority)) // a time variant message 
        {
            // we do not consider sources which have not seen any 
            // message from them yet
            if((timeArrival == 0) || (dt > fabs(timeConstant)))
                actualPriority = 0;
            else
                actualPriority = temporalPriority * 
                    (-exp((dt-fabs(timeConstant))/fabs(timeConstant)*5.0) + 1.0);        
        }
        else // a time invariant message 
        {
            if((dt > fabs(timeConstant)) && (temporalPriority > 0))
                actualPriority = -temporalPriority;
            else
                actualPriority = temporalPriority;
        }

       return actualPriority;
    }

    bool isResting( double priority){    
        return ((timeConstant < 0) 
                && ((priority < 0) || (priority >= priorityLevel)));
    }

    virtual bool configure(yarp::os::impl::Protocol& proto);

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);

    
public:
    double priorityLevel;           // priority ceiling (P)   
    double timeConstant;            // priority reset time (tc)
    double stimulation;             // stimulation value of the message (s)
    double temporalPriority;        // current priority of the message
    double timeArrival;             // arrival time of the message 
    bool isInhibitory;              // is an inhibitory message (s<0)
    String sourceName;

private:
    String portName;
    PriorityGroup *group;

    static ElectionOf<PriorityCarrier,PriorityGroup> *peers;

    static ElectionOf<PriorityCarrier,PriorityGroup>& getPeers();
};

#endif


