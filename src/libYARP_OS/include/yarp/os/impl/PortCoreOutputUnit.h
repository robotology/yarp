// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTCOREOUTPUTUNIT_
#define _YARP2_PORTCOREOUTPUTUNIT_

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/OutputProtocol.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCoreOutputUnit;
        }
    }
}

/**
 * Manager for a single output from a port.  Associated
 * with a PortCore object.
 */
class yarp::os::impl::PortCoreOutputUnit : public PortCoreUnit {
public:
    // specifically for managing input connections

    PortCoreOutputUnit(PortCore& owner, int index, OutputProtocol *op) :
        PortCoreUnit(owner,index), op(op), phase(1), activate(0), trackerMutex(1) {

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

    virtual void *send(yarp::os::PortWriter& writer,
                       yarp::os::PortReader *reader,
                       yarp::os::PortWriter *callback,
                       void *tracker,
                       const String& envelope,
                       bool waitAfter,
                       bool waitBefore,
                       bool *gotReply);

    virtual void *takeTracker();

    virtual bool isBusy();

    void setCarrierParams(const yarp::os::Property& params) {
        if(op)
            op->setOutputCarrierParams(params);
    }

    void getCarrierParams(yarp::os::Property& params) { 
        if(op)
            op->getOutputCarrierParams(params);
    }


private:
    OutputProtocol *op;
    bool closing, finished, running, threaded, sending;
    String name;
    SemaphoreImpl phase, activate, trackerMutex;
    yarp::os::PortWriter *cachedWriter;
    yarp::os::PortReader *cachedReader;
    yarp::os::PortWriter *cachedCallback;
    void *cachedTracker;
    String cachedEnvelope;

    bool sendHelper();

    void closeBasic();

    void closeMain();
};

#endif
