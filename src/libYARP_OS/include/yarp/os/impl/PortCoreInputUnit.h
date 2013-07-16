// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTCOREINPUTUNIT_
#define _YARP2_PORTCOREINPUTUNIT_

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/InputProtocol.h>

namespace yarp {
    namespace os {
        namespace impl {
            class PortCoreInputUnit;
        }
    }
}

/**
 * Manager for a single input to a port.  Associated
 * with a PortCore object.
 */
class yarp::os::impl::PortCoreInputUnit : public PortCoreUnit {
public:
    // specifically for managing input connections

    PortCoreInputUnit(PortCore& owner, int index, InputProtocol *ip,
                      bool autoHandshake, bool reversed) :
        PortCoreUnit(owner,index), ip(ip), phase(1), access(1),
        autoHandshake(autoHandshake), reversed(reversed) {

        YARP_ASSERT(ip!=NULL);
        closing = false;
        finished = false;
        running = false;
        name = owner.getName();
        yarp::os::PortReaderCreator *creator = owner.getReadCreator();
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

    virtual bool interrupt();

    void setCarrierParams(const yarp::os::Property& params) {
        if(ip)
            ip->getReceiver().setCarrierParams(params);
    }

    void getCarrierParams(yarp::os::Property& params) { 
        if(ip)
            ip->getReceiver().getCarrierParams(params);
    }


private:
    InputProtocol *ip;
    SemaphoreImpl phase, access;
    bool autoHandshake;
    bool closing, finished, running;
    String name;
    yarp::os::PortReader *localReader;
    Route officialRoute;
    bool reversed;

    void closeMain();

    bool skipIncomingData(yarp::os::ConnectionReader& reader);
};

#endif
