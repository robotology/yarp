// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
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
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>

#include <math.h>


#define STIMUL_THRESHOLD        1.0
#define WITH_RIORITY_DEBUG

namespace yarp {
    namespace os {
        namespace impl {
            class PriorityGroup;
            class PriorityCarrier;
#ifdef WITH_RIORITY_DEBUG
            class PriorityDebugThread;
#endif //WITH_RIORITY_DEBUG
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
    virtual ~PriorityGroup() {}
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader,
                                    PriorityCarrier *source);
};



#ifdef WITH_RIORITY_DEBUG
class yarp::os::impl::PriorityDebugThread : public yarp::os::RateThread {
public:
    PriorityDebugThread(PriorityCarrier* carrier);
    virtual ~PriorityDebugThread();
    void run();
    bool threadInit();
    void threadRelease();

public:
    int count;
    PriorityCarrier* pcarrier;
    String debugPortName;
    BufferedPort<yarp::sig::Vector> debugPort;
};
#endif //WITH_RIORITY_DEBUG


/**
 *
 * Allow priority-based message selection.  Under development.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.priority+level.15
 *
 */
class yarp::os::impl::PriorityCarrier : public yarp::os::impl::ModifyingCarrier {

#ifdef WITH_RIORITY_DEBUG
    friend class PriorityDebugThread;
#endif //WITH_RIORITY_DEBUG

public:
    PriorityCarrier()
#ifdef WITH_RIORITY_DEBUG
    : debugger(this)
#endif //WITH_RIORITY_DEBUG
    {
        group = 0/*NULL*/;
        timeConstant = timeArrival = 0;
        timeResting = 0;
        stimulation = 0;
        isVirtual = false;
        isActive = false;
        baias = 0;
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

    bool isResting(double priority){
        return ((timeResting > 0) &&
                ((priority < 0) || (priority >= STIMUL_THRESHOLD)));
    }


    void stimulate(double t) {

        temporalStimulation = getActualStimulation(t);
        // if it is in active sate, stimulates.
        if(!isResting(temporalStimulation))
        {
            temporalStimulation += stimulation;
            // respecting priority ceiling
            if(temporalStimulation >= STIMUL_THRESHOLD)
            {
                temporalStimulation = STIMUL_THRESHOLD;
                isActive = true;
            }
            // updating arrival time
            timeArrival = t;
        }
    }

    //
    // +P|   ---___
    //   |   |     -__
    //   |   |        -_
    //  0|------------------>  (Active state)
    //       Ta        Tc
    //
    //       Ta        Tr
    //  0|-------------_---->  (Resting state && P<0)
    //   |   |      __-
    //   |   |  ___-
    // -P|   ---
    //
    //
    // P(t) = Pi * (1-exp((t-Tc-Ta)/Tc*5) + exp(-5))
    // t:time, Pi: temporal Priority level
    // Tc: reset time, Ta: arrival time
    //
    double getActualStimulation(double t) {

        // we do not consider ports which has not seen any message
        // from them yet.
        //if(timeArrival == 0)
        //    return 0;

        double dt = t - timeArrival;
        // Temporal priority is inverted if this is a neuron model and the temporal
        // stimulation has already reached to STIMUL_THRESHOLD and waited for Tc.
        if((timeResting > 0)
           && (dt >= fabs(timeConstant))
           && (temporalStimulation >= STIMUL_THRESHOLD))
           temporalStimulation = -temporalStimulation;

        double actualStimulation;
        if(!isResting(temporalStimulation)) // behavior is in stimulation state
        {
            // After a gap bigger than Tc, the
            // priority is set to zero to avoid redundant calculation.
            if(dt > fabs(timeConstant))
                actualStimulation = 0;
            else
                actualStimulation = temporalStimulation *
                    (1.0 - exp((dt-timeConstant)/timeConstant*5.0) + exp(-5.0));
        }
        else // behavior is in resting state
        {
            // it is in waiting state for Tc
            if(temporalStimulation > 0)
                actualStimulation = temporalStimulation;
            else
            {
                dt -= fabs(timeConstant);
                // After a gap bigger than Tr, the
                // priority is set to zero to avoid redundant calculation.
                if(dt > fabs(timeResting))
                    actualStimulation = 0;
                else
                    actualStimulation = temporalStimulation *
                        (1.0 - exp((dt-timeResting)/timeResting*5.0) + exp(-5.0));
            }
        }

        if(actualStimulation <= 0)
            isActive = false;
        return actualStimulation;
    }

    double getActualInput(double t) {       //I(t)
        // calculating E(t) = Sum(e.I(t)) + b
        if(!isActive)
            return 0.0;

        double E = 0;
        for (PeerRecord::iterator it = group->peerSet.begin(); it!=group->peerSet.end(); it++)
        {
            PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(it);
            if(peer != this)
            {
                for(int i=0; i<peer->excitation.size(); i++)
                {
                    Value v = peer->excitation.get(i);
                    if(v.isList() && (v.asList()->size()>=2))
                    {
                        Bottle* b = v.asList();
                        // an exitatory to this priority carrier
                        if(sourceName == b->get(0).asString().c_str())
                            E += peer->getActualInput(t) * (b->get(1).asDouble()/10.0);
                    }
                }

            }
        }
        E += baias;
        double I = E * getActualStimulation(t);
        return ((I<0) ? 0 : I);     //I'(t)
    }

    virtual bool configure(yarp::os::impl::Protocol& proto);

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);

    virtual void setCarrierParams(const yarp::os::Property& params) {
        yarp::os::Property property = params;
        timeConstant = property.check("tc", Value(timeConstant)).asDouble();
        timeResting = property.check("tr", Value(timeResting)).asDouble();
        stimulation = property.check("st", Value(stimulation)).asDouble();
        baias = property.check("bs", Value(baias)).asDouble();
        isVirtual = property.check("virtual", Value(isVirtual)).asBool();
        if(property.check("ex"))
            excitation = property.findGroup("ex");
    }

    virtual void getCarrierParams(yarp::os::Property& params) {
        params.put("tc", timeConstant);
        params.put("tr", timeResting);
        params.put("st", stimulation);
        params.put("bs", baias);
        params.put("virtual", (int)isVirtual);
        params.put("ex", excitation.toString());
    }



public:
    double timeConstant;            // priority reset time (tc > 0)
    double timeResting;             // priority resting time (tr > 0)
    double stimulation;             // stimulation value of the message (s > 0)
    double temporalStimulation;     // current priority of the message S(t)
    double timeArrival;             // arrival time of the message
    bool isVirtual;                 // a virtual link does not carry any data
    bool isActive;                  // true if port is in active state X(t)
    double baias;                   // baias value for excitation
    Bottle excitation;              // a list of exitatory signals as (name, value)
    String sourceName;

private:
    String portName;
    PriorityGroup *group;

    static ElectionOf<PriorityCarrier,PriorityGroup> *peers;

    static ElectionOf<PriorityCarrier,PriorityGroup>& getPeers();

#ifdef WITH_RIORITY_DEBUG
private:
    PriorityDebugThread debugger;
#endif //WITH_RIORITY_DEBUG
};



#endif


