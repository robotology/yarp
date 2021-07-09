/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PRIORITYCARRIER_INC
#define PRIORITYCARRIER_INC

#include <cmath>
#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#define STIMUL_THRESHOLD        1.0
#define WITH_PRIORITY_DEBUG

class PriorityCarrier;

/**
 * Manager for priority-aware inputs to a given port.
 */
class PriorityGroup :
        public yarp::os::PeerRecord<PriorityCarrier>
{
public:
    virtual ~PriorityGroup() {}
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader,
                                    PriorityCarrier *source);
    bool recalculate(double t);

public:
    yarp::sig::Matrix InvA;         // the inverse of matrix (I-A) in the equation y(t) = [(I-A)^(-1) * B] .*x(t)
    yarp::sig::Matrix B;            // matrix of biases B in the equation y(t) = [(I-A)^(-1) * B] .*x(t)
    yarp::sig::Matrix Y;            // matrix y(t)
    yarp::sig::Matrix X;            // matrix x(t)
    //yarp::os::Semaphore semDebug;   // this semaphor is used only when debug mode is active
                                    // to control the access to matrices from debug thread
};



#ifdef WITH_PRIORITY_DEBUG
class PriorityDebugThread :
        public yarp::os::PeriodicThread
{
public:
    PriorityDebugThread(PriorityCarrier* carrier);
    ~PriorityDebugThread() override;
    void run() override;
    bool threadInit() override;
    void threadRelease() override;

public:
    int count;
    PriorityCarrier* pcarrier;
    std::string debugPortName;
    yarp::os::BufferedPort<yarp::sig::Vector> debugPort;
};
#endif //WITH_PRIORITY_DEBUG


/**
 * Allow priority-based message selection.  Under development.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.priority+level.15
 */
class PriorityCarrier :
        public yarp::os::ModifyingCarrier
{

#ifdef WITH_PRIORITY_DEBUG
    friend class PriorityDebugThread;
#endif //WITH_PRIORITY_DEBUG

public:
    PriorityCarrier()
#ifdef WITH_PRIORITY_DEBUG
    : debugger(this)
#endif //WITH_PRIORITY_DEBUG
    {
        group = 0/*NULL*/;
        timeConstant = timeArrival = 0;
        timeResting = 0;
        stimulation = 0;
        temporalStimulation = 0;
        isVirtual = false;
        isActive = false;
        bias = 0;
        yi = 0;     // used in debug
    }

    virtual ~PriorityCarrier() {
        if (portName!="") {
            // let peer carriers know I'm gone.
            getPeers().remove(portName,this);
        }
    }

    Carrier *create() const override {
        return new PriorityCarrier();
    }

    std::string getName() const override {
        return "priority";
    }

    std::string toString() const override {
        return "priority_carrier";
    }

    bool isResting(double priority) {
        return ((timeResting > 0) &&
                ((priority < 0) || (priority >= STIMUL_THRESHOLD)));
    }


    void stimulate(double t) {

        temporalStimulation = getActualStimulation(t);
        // if it is in active state, stimulates.
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

    double getActualStimulation(double t);

    double getActualInput(double t);

    bool configure(yarp::os::ConnectionState& proto) override;

    bool acceptIncomingData(yarp::os::ConnectionReader& reader) override;

    void setCarrierParams(const yarp::os::Property& params) override {
        yarp::os::Property property = params;
        timeConstant = property.check("tc", yarp::os::Value(timeConstant)).asFloat64();
        timeResting = property.check("tr", yarp::os::Value(timeResting)).asFloat64();
        stimulation = property.check("st", yarp::os::Value(stimulation)).asFloat64();
        bias = property.check("bs", yarp::os::Value(bias)).asFloat64();
        isVirtual = property.check("virtual", yarp::os::Value(isVirtual)).asBool();
        if(property.check("ex")) {
            excitation = property.findGroup("ex");
        }
    }

    void getCarrierParams(yarp::os::Property& params) const override {
        params.put("tc", timeConstant);
        params.put("tr", timeResting);
        params.put("st", stimulation);
        params.put("bs", bias);
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
    double bias;                    // bias value for excitation
    yarp::os::Bottle excitation;    // a list of exitatory signals as (name, value)
    std::string sourceName;

    double yi;                      // this is set in the recalculate() for the debug purpose

private:
    std::string portName;
    PriorityGroup *group;

    static yarp::os::ElectionOf<PriorityGroup> *peers;

    static yarp::os::ElectionOf<PriorityGroup>& getPeers();

#ifdef WITH_PRIORITY_DEBUG
private:
    PriorityDebugThread debugger;
#endif //WITH_PRIORITY_DEBUG
};

#endif // PRIORITYCARRIER_INC
