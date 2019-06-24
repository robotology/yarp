/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H
#define YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H

#include <yarp/os/Mutex.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>

namespace yarp {
namespace os {
namespace impl {

/**
 * Manager for a single output from a port.  Associated
 * with a PortCore object.
 */
class PortCoreOutputUnit :
        public PortCoreUnit
{
public:
    /**
     *
     * Constructor.
     * @param owner the port we call home
     * @param index an id for this connection
     * @param op the protocol object used to read/write to connection
     *
     */
    PortCoreOutputUnit(PortCore& owner, int index, OutputProtocol* op);

    /**
     * Destructor.
     */
    virtual ~PortCoreOutputUnit();

    /**
     * Prepare to serve this output.  A thread will start if a call
     * to send() has been made with options that require a thread.
     */
    bool start() override;

    /**
     * The body of a thread managing background sends.
     */
    void run() override;

    /**
     * Perform send operations without a separate thread.
     */
    virtual void runSingleThreaded();

    // documented in PortCoreUnit
    bool isOutput() override
    {
        return true;
    }

    // documented in PortCoreUnit
    void close() override
    {
        closeMain();
    }

    // documented in PortCoreUnit
    bool isFinished() override
    {
        return finished;
    }

    // documented in PortCoreUnit
    Route getRoute() override;

    // documented in PortCoreUnit
    virtual void* send(const yarp::os::PortWriter& writer,
                       yarp::os::PortReader* reader,
                       const yarp::os::PortWriter* callback,
                       void* tracker,
                       const std::string& envelopeString,
                       bool waitAfter,
                       bool waitBefore,
                       bool* gotReply) override;

    // documented in PortCoreUnit
    void* takeTracker() override;

    // documented in PortCoreUnit
    bool isBusy() override;

    // documented in PortCoreUnit
    void setCarrierParams(const yarp::os::Property& params) override;

    // documented in PortCoreUnit
    void getCarrierParams(yarp::os::Property& params) override;

    // return the protocol object
    OutputProtocol* getOutPutProtocol();

private:
    OutputProtocol *op; ///< protocol object for writing/reading
    bool closing;       ///< should this connection close
    bool finished;      ///< has this connection finished
    bool running;       ///< is a thread running
    bool threaded;      ///< do we need a thread for background writing
    bool sending;       ///< are we sending something right now
    yarp::os::Semaphore phase;        ///< let main thread kick sending thread
    yarp::os::Semaphore activate;     ///< signal when we have a new tracker
    yarp::os::Mutex trackerMutex; ///< protect the tracker during outside access
    const yarp::os::PortWriter* cachedWriter;   ///< the message the send
    yarp::os::PortReader *cachedReader;   ///< where to put a reply
    const yarp::os::PortWriter* cachedCallback; ///< where to sent commencement and
                                          ///< completion events
    void *cachedTracker;        ///< memory tracker for current message
    std::string cachedEnvelope;      ///< some text to pass along with the message

    /**
     * The core logic for sending a message.
     */
    bool sendHelper();

    /**
     * Try to close the connection, but not very hard.
     */
    void closeBasic();

    /**
     * Try harder to close the connection.
     */
    void closeMain();
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H
