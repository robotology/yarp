/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOREINPUTUNIT_H
#define YARP_OS_IMPL_PORTCOREINPUTUNIT_H

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
class yarp::os::impl::PortCoreInputUnit : public PortCoreUnit
{
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
    PortCoreInputUnit(PortCore& owner,
                      int index,
                      InputProtocol *ip,
                      bool reversed);

    /**
     * Destructor.
     */
    virtual ~PortCoreInputUnit();

    /**
     *
     * Start a thread running to serve this input.
     *
     */
    virtual bool start() override;

    /**
     *
     * The body of the thread associated with this input. Accepts
     * and processes administrative input, and makes sure regular
     * data gets to the user
     *
     */
    virtual void run() override;

    virtual bool isInput() override;

    virtual void close() override;

    virtual bool isFinished() override;

    const std::string& getName();

    virtual Route getRoute() override;

    virtual bool interrupt() override;

    virtual void setCarrierParams(const yarp::os::Property& params) override;

    virtual void getCarrierParams(yarp::os::Property& params) override;

    // return the protocol object
    InputProtocol* getInPutProtocol();

    virtual bool isBusy() override;

private:
    InputProtocol *ip;
    SemaphoreImpl phase, access;
    bool closing, finished, running;
    std::string name;
    yarp::os::PortReader *localReader;
    Route officialRoute;
    bool reversed;

    void closeMain();

    bool skipIncomingData(yarp::os::ConnectionReader& reader);

    static void envelopeReadCallback(void* data, const Bytes& envelope);
};

#endif // YARP_OS_IMPL_PORTCOREINPUTUNIT_H
