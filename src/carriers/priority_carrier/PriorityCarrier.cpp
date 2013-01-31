// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
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

    timeConstant = fabs(options.check("tc",Value(0.0)).asDouble());
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
        fprintf(stdout, "   st: %.2f\n", stimulation);
        fprintf(stdout, "   bs: %.2f\n", baias);
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

    // a virtual message will never be delivered. It will be only
    // used for the coordination
    if(source->isVirtual)
        return false;

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



