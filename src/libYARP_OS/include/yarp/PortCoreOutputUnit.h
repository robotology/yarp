// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCOREOUTPUTUNIT_
#define _YARP2_PORTCOREOUTPUTUNIT_

#include <yarp/PortCore.h>
#include <yarp/PortCoreUnit.h>
#include <yarp/Logger.h>
#include <yarp/OutputProtocol.h>

namespace yarp {
    class PortCoreOutputUnit;
}

/**
 * Manager for a single output from a port.  Associated
 * with a PortCore object.
 */
class yarp::PortCoreOutputUnit : public PortCoreUnit {
public:
    // specifically for managing input connections

    PortCoreOutputUnit(PortCore& owner, OutputProtocol *op) : 
        PortCoreUnit(owner), op(op), phase(1), activate(0), trackerMutex(1) {

        YARP_ASSERT(op!=NULL);
        closing = false;
        finished = false;
        running = false;
        threaded = false;
        sending = false;
        name = owner.getName();
        cachedWriter = NULL;
        cachedReader = NULL;
        cachedTracker = NULL;
    }

    virtual ~PortCoreOutputUnit() {
        closeMain();
    }

    virtual bool start();

    virtual void run();

    virtual bool isOutput() {
        return true;
    }

    // just for testing
    virtual void runSimulation();

    virtual void close() {
        closeMain();
    }

    virtual bool isFinished() {
        return finished;
    }

    const String& getName() {
        return name;
    }

    virtual Route getRoute();

    virtual void *send(Writable& writer, 
                       Readable *reader,
                       Writable *callback,
                       void *tracker,
                       const String& envelope,
                       bool waitAfter,
                       bool waitBefore);

    virtual void *takeTracker();

    virtual bool isBusy();

private:
    OutputProtocol *op;
    bool closing, finished, running, threaded, sending;
    String name;
    SemaphoreImpl phase, activate, trackerMutex;
    Writable *cachedWriter;
    Readable *cachedReader;
    Writable *cachedCallback;
    void *cachedTracker;
    String cachedEnvelope;

    void sendHelper();

    void closeBasic();

    void closeMain();
};

#endif

