/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTCOREINPUTUNIT
#define YARP2_PORTCOREINPUTUNIT

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/InputProtocol.h>

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
    /**
     * Constructor.
     *
     * @param owner the port we call home
     * @param index an id for this connection
     * @param ip the protocol object used to read/write to connection
     * @param reversed true if this input connection was originally 
     * an output which was then reversed
     *
     */
    PortCoreInputUnit(PortCore& owner, int index, InputProtocol *ip,
                      bool reversed) :
        PortCoreUnit(owner,index),
        ip(ip),
        phase(1),
        access(1),
        reversed(reversed) {

        yAssert(ip!=YARP_NULLPTR);
        closing = false;
        finished = false;
        running = false;
        name = owner.getName();
        yarp::os::PortReaderCreator *creator = owner.getReadCreator();
        localReader = YARP_NULLPTR;
        if (creator!=YARP_NULLPTR) {
            localReader = creator->create();
        }
    }

    /**
     * Destructor.
     */
    virtual ~PortCoreInputUnit() {
        closeMain();
        if (localReader!=YARP_NULLPTR) {
            delete localReader;
            localReader = YARP_NULLPTR;
        }
    }

    /**
     *
     * Start a thread running to serve this input.
     *
     */
    virtual bool start();

    /**
     *
     * The body of the thread associated with this input. Accepts
     * and processes administrative input, and makes sure regular
     * data gets to the user
     *
     */
    virtual void run();

    virtual bool isInput() {
        return true;
    }

    virtual void close() {
        closeMain();
    }

    virtual bool isFinished() {
        return finished;
    }

    const ConstString& getName() {
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

    // return the protocol object
    InputProtocol* getInPutProtocol() { return ip; }

    virtual bool isBusy();

private:
    InputProtocol *ip;
    SemaphoreImpl phase, access;
    bool closing, finished, running;
    ConstString name;
    yarp::os::PortReader *localReader;
    Route officialRoute;
    bool reversed;

    void closeMain();

    bool skipIncomingData(yarp::os::ConnectionReader& reader);

    static void envelopeReadCallback(void* data, const Bytes& envelope);
};

#endif
