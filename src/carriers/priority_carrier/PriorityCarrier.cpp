// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "PriorityCarrier.h"

#ifdef WITH_YARPMATH 
#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>
#endif 

using namespace yarp::math;

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

#ifdef WITH_RIORITY_DEBUG
    if(options.check("debug"))
    {
        fprintf(stdout, "\n%s:\n", sourceName.c_str());
        fprintf(stdout, "   stimulation: %.2f\n", stimulation);
        fprintf(stdout, "   bias: %.2f\n", baias);
        fprintf(stdout, "   tc: %.2fs\n", timeConstant);
        fprintf(stdout, "   tr: %.2fs\n", timeResting);
        fprintf(stdout, "   ex: ");
        for(int i=0; i<excitation.size(); i++)
        {
            Value v = excitation.get(i);
            if(v.isList() && (v.asList()->size()>=2))
            {
                Bottle* b = v.asList();
                fprintf(stdout, "(%s, %.2f) ",
                                b->get(0).asString().c_str(),
                                b->get(1).asDouble()/10.0 );
            }
        }
        fprintf(stdout, "\n");
        fprintf(stdout, "   virtual: %s\n",
                            (isVirtual)?"yes":"no");
        int rate = options.check("rate", Value(10)).asInt();
        fprintf(stdout, "   db.rate: %dms\n", rate);
        debugger.stop();
        debugger.setRate(rate);
        debugger.start();
    }
#endif
    return true;
}

// Decide whether data should be accepted, for real.
bool PriorityGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PriorityCarrier *source) {


    // updates message's arrival time
    double tNow = yarp::os::Time::now();

    // stimulate
    source->stimulate(tNow);

#ifdef WITH_YARPMATH   
    
    //TODO: find the correct way to get the size of peerSet
    int nConnections = 0;    
    for(PeerRecord::iterator it=peerSet.begin(); it!=peerSet.end(); it++)
        nConnections++;

    // calculate matrices X, B, InvA and Y
    X.resize(nConnections, 1);
    B.resize(nConnections, 1);
    Y.resize(nConnections, 1);
    InvA.resize(nConnections, nConnections);
    InvA.eye();

    int row = 0;
    for(PeerRecord::iterator rowItr=peerSet.begin(); rowItr!=peerSet.end(); rowItr++)
    {
        PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(rowItr);
        double xi =(peer->isActive) ? 1.0 : 0.0; 
        B(row,0) = peer->baias * xi;
        X(row,0) = xi;

        int col = 0;
        for(PeerRecord::iterator colItr=peerSet.begin(); colItr!=peerSet.end(); colItr++)
        {
            PriorityCarrier *peerCol = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(colItr);
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
        //TODO: set an error flag or prompt correctly
        fprintf(stdout, "determinant of the weight matrix is zero!\n");
        return false;
    }

    // inverting the weigth matrix 
    InvA = yarp::math::luinv(InvA);
    Y = InvA * B;

    //fprintf(stdout, "X:\n %s\n", X.toString(1).c_str());
    //fprintf(stdout, "B:\n %s\n", B.toString(1).c_str());
    //fprintf(stdout, "Y:\n %s\n", Y.toString(1).c_str());

    row = 0;
    PriorityCarrier *maxPeer = NULL;
    double maxStimuli = 0.0;
    for(PeerRecord::iterator it=peerSet.begin(); it!=peerSet.end(); it++)
    {
        PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(it);
        if(!peer->isVirtual)
        {
            double output = Y(row,0) * X(row,0);
            if(output > maxStimuli)
            {   
                maxStimuli = output;
                maxPeer = peer;
            }
        }            
        row++;
    }

    bool accept = (maxPeer == source);
    

#else
    // first checks whether actual input signal is positive or not
    double actualInput = source->getActualInput(tNow);
    bool accept = (actualInput > 0);
    if(accept)
    {
        for (PeerRecord::iterator it = peerSet.begin(); it!=peerSet.end(); it++)
        {
            PriorityCarrier *peer = (PriorityCarrier *)PLATFORM_MAP_ITERATOR_FIRST(it);
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



#ifdef WITH_RIORITY_DEBUG
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
    v[2] = (pcarrier->isActive) ? STIMUL_THRESHOLD : 0.0;
    v[3] = pcarrier->getActualInput(t);
    debugPort.write();
}

bool PriorityDebugThread::threadInit()
{
    debugPortName = pcarrier->portName + pcarrier->sourceName + String(":debug");
    return debugPort.open(debugPortName.c_str());
}

void PriorityDebugThread::threadRelease()
{
    debugPort.close();
}

#endif //WITH_RIORITY_DEBUG



