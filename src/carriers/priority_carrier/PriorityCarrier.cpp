// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Log.h>
#include <yarp/os/ConstString.h>

#ifdef WITH_YARPMATH 
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>
using namespace yarp::math;
#endif 

#include "PriorityCarrier.h"


using namespace yarp::os;

#ifdef _MSC_VER
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif 


/**
 * Class PriorityCarrier
 */

ElectionOf<PriorityGroup> *PriorityCarrier::peers = NULL;

// Make a singleton manager for finding peer carriers.
ElectionOf<PriorityGroup>& PriorityCarrier::getPeers() {
    NetworkBase::lock();
    if (peers==NULL) {
        peers = new ElectionOf<PriorityGroup>;
        NetworkBase::unlock();
        yAssert(peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted.
bool PriorityCarrier::acceptIncomingData(yarp::os::ConnectionReader& reader) {
    getPeers().lock();
    yAssert(group);
    bool result = group->acceptIncomingData(reader,this);
    getPeers().unlock();
    return result;
}

// Read connection settings.
bool PriorityCarrier::configure(yarp::os::ConnectionState& proto) {
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    group = getPeers().add(portName,this);
    if (!group) return false;

    Property options;
    options.fromString(proto.getSenderSpecifier().c_str());

    timeConstant = fabs(options.check("tc",Value(1.0)).asDouble());
    timeResting = fabs(options.check("tr",Value(0.0)).asDouble());
    stimulation = fabs(options.check("st",Value(STIMUL_THRESHOLD*10)).asDouble());
    // Zero stimulation is undefined and will be interpreted as S=Thresould.
    if(stimulation == 0)
        stimulation = STIMUL_THRESHOLD*10;
    stimulation /= 10.0;

    baias = options.check("bs",Value(STIMUL_THRESHOLD*10)).asDouble();
    baias /= 10.0;

    excitation = options.findGroup("ex");
    isVirtual = options.check("virtual");

#ifdef WITH_PRIORITY_DEBUG
    if(options.check("debug"))
    {
        yarp::os::ConstString msg;
        char dummy[1024];
        safe_printf(dummy, 1024, "\n%s:\n", sourceName.c_str());
        msg+= dummy;
        safe_printf(dummy, 1024, "   stimulation: %.2f\n", stimulation);
        msg+= dummy;
        safe_printf(dummy, 1024, "   bias: %.2f\n", baias);
        msg+= dummy;
        safe_printf(dummy, 1024, "   tc: %.2fs\n", timeConstant);
        msg+= dummy;
        safe_printf(dummy, 1024, "   tr: %.2fs\n", timeResting);
        msg+= dummy;
        safe_printf(dummy, 1024, "   ex: ");
        msg+= dummy;
        for(int i=0; i<excitation.size(); i++)
        {
            Value v = excitation.get(i);
            if(v.isList() && (v.asList()->size()>=2))
            {
                Bottle* b = v.asList();
                safe_printf(dummy, 1024, "(%s, %.2f) ",
                                b->get(0).asString().c_str(),
                                b->get(1).asDouble()/10.0 );
                msg+= dummy;
            }
        }
        //safe_printf(dummy, 1024, "\n");
        msg+= "\n";
        safe_printf(dummy, 1024, "   virtual: %s\n",
                            (isVirtual)?"yes":"no");
        msg+= dummy;
        int rate = options.check("rate", Value(10)).asInt();
        safe_printf(dummy, 1024, "   db.rate: %dms\n", rate);
        msg+= dummy;
        yInfo(msg.c_str());
        debugger.stop();
        debugger.setRate(rate);
        debugger.start();
    }
#endif
    return true;
}


double PriorityCarrier::getActualStimulation(double t) 
{
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

double PriorityCarrier::getActualInput(double t) 
{  
    // calculating E(t) = Sum(e.I(t)) + b
    if(!isActive)
        return 0.0;

    double E = 0;
    for (PriorityGroup::iterator it = group->peerSet.begin(); it!=group->peerSet.end(); it++)
    {
        PriorityCarrier *peer = it->first;
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


/**
 * Class PriorityGroup
 */

bool PriorityGroup::recalculate(double t) 
{
#ifdef WITH_YARPMATH    
    //TODO: find the correct way to get the size of peerSet
    int nConnections = 0;    
    for(PriorityGroup::iterator it=peerSet.begin(); it!=peerSet.end(); it++)
        nConnections++;

    // calculate matrices X, B, InvA and Y
    X.resize(nConnections, 1);
    B.resize(nConnections, 1);
    Y.resize(nConnections, 1);
    InvA.resize(nConnections, nConnections);
    InvA.eye();

    int row = 0;
    for(PriorityGroup::iterator rowItr=peerSet.begin(); rowItr!=peerSet.end(); rowItr++)
    {
        PriorityCarrier *peer = rowItr->first;
        // call 'getActualStimulation' to update 'isActive'
        peer->getActualStimulation(t); 
        double xi = (peer->isActive) ? STIMUL_THRESHOLD : 0.0;
        B(row,0) = peer->baias * xi;
        X(row,0) = xi;

        int col = 0;
        for(PriorityGroup::iterator colItr=peerSet.begin(); colItr!=peerSet.end(); colItr++)
        {
            PriorityCarrier *peerCol = colItr->first;
            for(int i=0; i<peerCol->excitation.size(); i++)
            {
                Value v = peerCol->excitation.get(i);
                if(v.isList() && (v.asList()->size()>=2))
                {
                    Bottle* b = v.asList();
                    // an exitatory link to this connection
                    if(peer->sourceName == b->get(0).asString().c_str())
                        InvA(row,col) = -(b->get(1).asDouble()/10.0)*xi;
                }
            }
            col++;
        }
        row++;
    }

    //fprintf(stdout, "A:\n %s\n", InvA.toString(1).c_str());

    // calclulating the determinant
    double determinant = yarp::math::det(InvA);
    if(determinant == 0)
    {
        yError("Inconsistent regulation! non-invertible weight matrix");
        return false;
    }

    // inverting the weigth matrix 
    InvA = yarp::math::luinv(InvA);
    Y = InvA * B;

    //fprintf(stdout, "X:\n %s\n", X.toString(1).c_str());
    //fprintf(stdout, "B:\n %s\n", B.toString(1).c_str());
    //fprintf(stdout, "Y:\n %s\n", Y.toString(1).c_str());
    
    return true;
#else
    return false;
#endif

}

// Decide whether data should be accepted, for real.
bool PriorityGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PriorityCarrier *source) {

    bool accept;
    // updates message's arrival time
    double tNow = yarp::os::Time::now();
    source->stimulate(tNow);

#ifdef WITH_YARPMATH   
    if(!recalculate(tNow))
        return false;

    int row = 0;
    PriorityCarrier *maxPeer = NULL;
    double maxStimuli = 0.0;
    for(PriorityGroup::iterator it=peerSet.begin(); it!=peerSet.end(); it++)
    {
        PriorityCarrier *peer = it->first;
        double output = Y(row,0) * X(row,0);
        peer->yi = output;      // only for debug purpose

        if(!peer->isVirtual)
        {
            if(output > maxStimuli)
            {   
                maxStimuli = output;
                maxPeer = peer;
            }
        }            
        row++;
    }
    accept = (maxPeer == source);
    
#else
    // first checks whether actual input signal is positive or not
    double actualInput = source->getActualInput(tNow);
    accept = (actualInput > 0);
    if(accept)
    {
        for (PriorityGroup::iterator it = peerSet.begin(); it!=peerSet.end(); it++)
        {
            PriorityCarrier *peer = it->first;
            if(peer != source)
            {
                if(actualInput < peer->getActualInput(tNow))
                {
                    accept = false;
                    break;
                }
            }
        }
    }
#endif

    // a virtual message will never be delivered. It will be only
    // used for the coordination
    if(source->isVirtual)
        accept = false;

    return accept;
}


/**
 * Class PriorityDebugThread
 */

#ifdef WITH_PRIORITY_DEBUG
PriorityDebugThread::PriorityDebugThread(PriorityCarrier* carrier) : RateThread(10)
{
    pcarrier = carrier;
    count = 0;
}

PriorityDebugThread::~PriorityDebugThread()
{
    if(isRunning()) stop();
}

void PriorityDebugThread::run()
{
    yarp::sig::Vector& v = debugPort.prepare();
    v.resize(4);
    // a vector of [t, S(t), S'(t), I'(t)]
    double t = yarp::os::Time::now();
    v[0] = t;
    v[1] = pcarrier->getActualStimulation(t);
#ifdef WITH_YARPMATH 
    v[2] = pcarrier->yi;
#else
    v[2] = pcarrier->getActualInput(t);
#endif    
    debugPort.write();
}

bool PriorityDebugThread::threadInit()
{
    debugPortName = pcarrier->portName + pcarrier->sourceName + ConstString(":debug");
    return debugPort.open(debugPortName.c_str());
}

void PriorityDebugThread::threadRelease()
{
    debugPort.close();
}

#endif //WITH_PRIORITY_DEBUG



