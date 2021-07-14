/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "PriorityCarrier.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/ConnectionState.h>
#include <yarp/os/Route.h>

#include <yarp/math/Math.h>
#include <yarp/math/SVD.h>
#include <string>


using namespace yarp::os;
using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(PRIORITYCARRIER,
                   "yarp.carrier.priority",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)
}


/**
 * Class PriorityCarrier
 */

ElectionOf<PriorityGroup> *PriorityCarrier::peers = nullptr;

// Make a singleton manager for finding peer carriers.
ElectionOf<PriorityGroup>& PriorityCarrier::getPeers() {
    NetworkBase::lock();
    if (peers==nullptr) {
        peers = new ElectionOf<PriorityGroup>;
        NetworkBase::unlock();
        yCAssert(PRIORITYCARRIER, peers);
    } else {
        NetworkBase::unlock();
    }
    return *peers;
}

// Decide whether data should be accepted.
bool PriorityCarrier::acceptIncomingData(yarp::os::ConnectionReader& reader) {
    getPeers().lock();
    yCAssert(PRIORITYCARRIER, group);
    bool result = group->acceptIncomingData(reader,this);
    getPeers().unlock();
    return result;
}

// Read connection settings.
bool PriorityCarrier::configure(yarp::os::ConnectionState& proto) {
    portName = proto.getRoute().getToName();
    sourceName = proto.getRoute().getFromName();
    group = getPeers().add(portName,this);
    if (!group) {
        return false;
    }

    Property options;
    options.fromString(proto.getSenderSpecifier());

    timeConstant = fabs(options.check("tc",Value(1.0)).asFloat64());
    timeResting = fabs(options.check("tr",Value(0.0)).asFloat64());
    stimulation = fabs(options.check("st",Value(STIMUL_THRESHOLD*10)).asFloat64());
    // Zero stimulation is undefined and will be interpreted as S=Thresould.
    if (stimulation == 0) {
        stimulation = STIMUL_THRESHOLD * 10;
    }
    stimulation /= 10.0;

    bias = options.check("bs",Value(STIMUL_THRESHOLD*10)).asFloat64();
    bias /= 10.0;

    excitation = options.findGroup("ex");
    isVirtual = options.check("virtual");

#ifdef WITH_PRIORITY_DEBUG
    if(options.check("debug"))
    {
        std::string msg;
        char dummy[1024];
        std::snprintf(dummy, 1024, "\n%s:\n", sourceName.c_str());
        msg+= dummy;
        std::snprintf(dummy, 1024, "   stimulation: %.2f\n", stimulation);
        msg+= dummy;
        std::snprintf(dummy, 1024, "   bias: %.2f\n", bias);
        msg+= dummy;
        std::snprintf(dummy, 1024, "   tc: %.2fs\n", timeConstant);
        msg+= dummy;
        std::snprintf(dummy, 1024, "   tr: %.2fs\n", timeResting);
        msg+= dummy;
        std::snprintf(dummy, 1024, "   ex: ");
        msg+= dummy;
        for(size_t i=0; i<excitation.size(); i++)
        {
            Value v = excitation.get(i);
            if(v.isList() && (v.asList()->size()>=2))
            {
                Bottle* b = v.asList();
                std::snprintf(dummy, 1024, "(%s, %.2f) ",
                                b->get(0).asString().c_str(),
                                b->get(1).asFloat64()/10.0 );
                msg+= dummy;
            }
        }
        //std::snprintf(dummy, 1024, "\n");
        msg+= "\n";
        std::snprintf(dummy, 1024, "   virtual: %s\n",
                            (isVirtual)?"yes":"no");
        msg+= dummy;
        double rate = options.check("rate", Value(10)).asInt32() / 1000.0;
        std::snprintf(dummy, 1024, "   db.rate: %fs\n", rate);
        msg+= dummy;
        yCInfo(PRIORITYCARRIER, "%s", msg.c_str());
        debugger.stop();
        debugger.setPeriod(rate);
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
    if ((timeResting > 0)
        && (dt >= fabs(timeConstant))
        && (temporalStimulation >= STIMUL_THRESHOLD)) {
        temporalStimulation = -temporalStimulation;
    }

    double actualStimulation;
    if(!isResting(temporalStimulation)) // behavior is in stimulation state
    {
        // After a gap bigger than Tc, the
        // priority is set to zero to avoid redundant calculation.
        if (dt > fabs(timeConstant)) {
            actualStimulation = 0;
        } else {
            actualStimulation = temporalStimulation * (1.0 - exp((dt - timeConstant) / timeConstant * 5.0) + exp(-5.0));
        }
    }
    else // behavior is in resting state
    {
        // it is in waiting state for Tc
        if (temporalStimulation > 0) {
            actualStimulation = temporalStimulation;
        } else {
            dt -= fabs(timeConstant);
            // After a gap bigger than Tr, the
            // priority is set to zero to avoid redundant calculation.
            if (dt > fabs(timeResting)) {
                actualStimulation = 0;
            } else {
                actualStimulation = temporalStimulation * (1.0 - exp((dt - timeResting) / timeResting * 5.0) + exp(-5.0));
            }
        }
    }

    if (actualStimulation <= 0) {
        isActive = false;
    }

    return actualStimulation;
}

double PriorityCarrier::getActualInput(double t)
{
    // calculating E(t) = Sum(e.I(t)) + b
    if (!isActive) {
        return 0.0;
    }

    double E = 0;
    for (auto& it : group->peerSet)
    {
        PriorityCarrier *peer = it.first;
        if(peer != this)
        {
            for(size_t i=0; i<peer->excitation.size(); i++)
            {
                Value v = peer->excitation.get(i);
                if(v.isList() && (v.asList()->size()>=2))
                {
                    Bottle* b = v.asList();
                    // an exitatory to this priority carrier
                    if (sourceName == b->get(0).asString()) {
                        E += peer->getActualInput(t) * (b->get(1).asFloat64() / 10.0);
                    }
                }
            }

        }
    }
    E += bias;
    double I = E * getActualStimulation(t);
    return ((I<0) ? 0 : I);     //I'(t)
}


/**
 * Class PriorityGroup
 */

bool PriorityGroup::recalculate(double t)
{
    //TODO: find the correct way to get the size of peerSet
    int nConnections = 0;
    for (auto it = peerSet.begin(); it != peerSet.end(); it++) {
        nConnections++;
    }

    // calculate matrices X, B, InvA and Y
    X.resize(nConnections, 1);
    B.resize(nConnections, 1);
    Y.resize(nConnections, 1);
    InvA.resize(nConnections, nConnections);
    InvA.eye();

    int row = 0;
    for(auto& rowItr : peerSet)
    {
        PriorityCarrier* peer = rowItr.first;
        // call 'getActualStimulation' to update 'isActive'
        peer->getActualStimulation(t);
        double xi = (peer->isActive) ? STIMUL_THRESHOLD : 0.0;
        B(row,0) = peer->bias * xi;
        X(row,0) = xi;

        int col = 0;
        for(auto& it : peerSet)
        {
            PriorityCarrier *peerCol = it.first;
            for(size_t i=0; i<peerCol->excitation.size(); i++)
            {
                Value v = peerCol->excitation.get(i);
                if(v.isList() && (v.asList()->size()>=2))
                {
                    Bottle* b = v.asList();
                    // an exitatory link to this connection
                    if (peer->sourceName == b->get(0).asString()) {
                        InvA(row, col) = -(b->get(1).asFloat64() / 10.0) * xi;
                    }
                }
            }
            col++;
        }
        row++;
    }

    yCTrace(PRIORITYCARRIER, "A:\n %s", InvA.toString(1).c_str());

    // calclulating the determinant
    double determinant = yarp::math::det(InvA);
    if(determinant == 0)
    {
        yCError(PRIORITYCARRIER, "Inconsistent regulation! non-invertible weight matrix");
        return false;
    }

    // inverting the weight matrix
    InvA = yarp::math::luinv(InvA);
    Y = InvA * B;

    yCTrace(PRIORITYCARRIER, "X:\n %s", X.toString(1).c_str());
    yCTrace(PRIORITYCARRIER, "B:\n %s", B.toString(1).c_str());
    yCTrace(PRIORITYCARRIER, "Y:\n %s", Y.toString(1).c_str());

    return true;
}

// Decide whether data should be accepted, for real.
bool PriorityGroup::acceptIncomingData(yarp::os::ConnectionReader& reader,
                                       PriorityCarrier *source) {

    bool accept;
    // updates message's arrival time
    double tNow = yarp::os::Time::now();
    source->stimulate(tNow);

    if (!recalculate(tNow)) {
        return false;
    }

    int row = 0;
    PriorityCarrier *maxPeer = nullptr;
    double maxStimuli = 0.0;
    for(auto& it : peerSet)
    {
        PriorityCarrier *peer = it.first;
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

    // a virtual message will never be delivered. It will be only
    // used for the coordination
    if (source->isVirtual) {
        accept = false;
    }

    return accept;
}


/**
 * Class PriorityDebugThread
 */

#ifdef WITH_PRIORITY_DEBUG
PriorityDebugThread::PriorityDebugThread(PriorityCarrier* carrier) : PeriodicThread(0.01)
{
    pcarrier = carrier;
    count = 0;
}

PriorityDebugThread::~PriorityDebugThread()
{
    if (isRunning()) {
        stop();
    }
}

void PriorityDebugThread::run()
{
    yarp::sig::Vector& v = debugPort.prepare();
    v.resize(4);
    // a vector of [t, S(t), S'(t), I'(t)]
    double t = yarp::os::Time::now();
    v[0] = t;
    v[1] = pcarrier->getActualStimulation(t);
    v[2] = pcarrier->yi;
    debugPort.write();
}

bool PriorityDebugThread::threadInit()
{
    debugPortName = pcarrier->portName + pcarrier->sourceName + std::string(":debug");
    return debugPort.open(debugPortName);
}

void PriorityDebugThread::threadRelease()
{
    debugPort.close();
}

#endif //WITH_PRIORITY_DEBUG
