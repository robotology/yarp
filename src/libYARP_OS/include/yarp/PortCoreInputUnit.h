// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCOREINPUTUNIT_
#define _YARP2_PORTCOREINPUTUNIT_

#include <yarp/PortCore.h>
#include <yarp/PortCoreUnit.h>
#include <yarp/Logger.h>
#include <yarp/InputProtocol.h>

namespace yarp {
    class PortCoreInputUnit;
}

/**
 * Manager for a single input to a port.  Associated
 * with a PortCore object.
 */
class yarp::PortCoreInputUnit : public PortCoreUnit {
public:
    // specifically for managing input connections

    PortCoreInputUnit(PortCore& owner, InputProtocol *ip, 
                      bool autoHandshake) : 
        PortCoreUnit(owner), ip(ip), phase(1), access(1),
        autoHandshake(autoHandshake) {

        YARP_ASSERT(ip!=NULL);
        closing = false;
        finished = false;
        running = false;
        name = owner.getName();
        ReadableCreator *creator = owner.getReadCreator();
        localReader = NULL;
        if (creator!=NULL) {
            localReader = creator->create();
        }
    }

    virtual ~PortCoreInputUnit() {
        closeMain();
        if (localReader!=NULL) {
            delete localReader;
            localReader = NULL;
        }
    }

    virtual bool start();

    virtual void run();

    virtual bool isInput() {
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


private:
    InputProtocol *ip;
    SemaphoreImpl phase, access;
    bool autoHandshake;
    bool closing, finished, running;
    String name;
    Readable *localReader;

    void closeMain();
};

#endif

