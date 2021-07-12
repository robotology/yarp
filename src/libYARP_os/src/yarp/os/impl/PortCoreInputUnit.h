/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCOREINPUTUNIT_H
#define YARP_OS_IMPL_PORTCOREINPUTUNIT_H

#include <yarp/os/InputProtocol.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Manager for a single input to a port.  Associated
 * with a PortCore object.
 */
class PortCoreInputUnit :
        public PortCoreUnit
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
                      InputProtocol* ip,
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
    bool start() override;

    /**
     *
     * The body of the thread associated with this input. Accepts
     * and processes administrative input, and makes sure regular
     * data gets to the user
     *
     */
    void run() override;

    bool isInput() override;

    void close() override;

    bool isFinished() override;

    const std::string& getName();

    Route getRoute() override;

    bool interrupt() override;

    void setCarrierParams(const yarp::os::Property& params) override;

    void getCarrierParams(yarp::os::Property& params) override;

    // return the protocol object
    InputProtocol* getInPutProtocol();

    bool isBusy() override;

private:
    InputProtocol* ip;
    yarp::os::Semaphore phase, access;
    bool closing, finished, running;
    std::string name;
    yarp::os::PortReader* localReader;
    Route officialRoute;
    bool reversed;

    void closeMain();

    bool skipIncomingData(yarp::os::ConnectionReader& reader);

    static void envelopeReadCallback(void* data, const Bytes& envelope);
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREINPUTUNIT_H
