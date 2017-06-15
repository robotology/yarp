/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H
#define YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H

#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/PortCoreUnit.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/OutputProtocol.h>

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
class yarp::os::impl::PortCoreOutputUnit : public PortCoreUnit
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
    PortCoreOutputUnit(PortCore& owner, int index, OutputProtocol *op) :
            PortCoreUnit(owner, index),
            op(op),
            closing(false),
            finished(false),
            running(false),
            threaded(false),
            sending(false),
            phase(1),
            activate(0),
            trackerMutex(1),
            cachedWriter(YARP_NULLPTR),
            cachedReader(YARP_NULLPTR),
            cachedCallback(YARP_NULLPTR),
            cachedTracker(YARP_NULLPTR)
    {
        yAssert(op!=YARP_NULLPTR);
    }

    /**
     * Destructor.
     */
    virtual ~PortCoreOutputUnit()
    {
        closeMain();
    }

    /**
     * Prepare to serve this output.  A thread will start if a call
     * to send() has been made with options that require a thread.
     */
    virtual bool start() override;

    /**
     * The body of a thread managing background sends.
     */
    virtual void run() override;

    /**
     * Perform send operations without a separate thread.
     */
    virtual void runSingleThreaded();

    // documented in PortCoreUnit
    virtual bool isOutput() override
    {
        return true;
    }

    // documented in PortCoreUnit
    virtual void close() override
    {
        closeMain();
    }

    // documented in PortCoreUnit
    virtual bool isFinished() override
    {
        return finished;
    }

    // documented in PortCoreUnit
    virtual Route getRoute() override;

    // documented in PortCoreUnit
    virtual void *send(yarp::os::PortWriter& writer,
                       yarp::os::PortReader *reader,
                       yarp::os::PortWriter *callback,
                       void *tracker,
                       const ConstString& envelopeString,
                       bool waitAfter,
                       bool waitBefore,
                       bool *gotReply) override;

    // documented in PortCoreUnit
    virtual void *takeTracker() override;

    // documented in PortCoreUnit
    virtual bool isBusy() override;

    // documented in PortCoreUnit
    void setCarrierParams(const yarp::os::Property& params) override
    {
        if (op)
            op->getConnection().setCarrierParams(params);
    }

    // documented in PortCoreUnit
    void getCarrierParams(yarp::os::Property& params) override
    {
        if (op)
            op->getConnection().getCarrierParams(params);
    }

    // return the protocol object
    OutputProtocol* getOutPutProtocol()
    {
        return op;
    }

private:
    OutputProtocol *op; ///< protocol object for writing/reading
    bool closing;       ///< should this connection close
    bool finished;      ///< has this connection finished
    bool running;       ///< is a thread running
    bool threaded;      ///< do we need a thread for background writing
    bool sending;       ///< are we sending something right now
    SemaphoreImpl phase;        ///< let main thread kick sending thread
    SemaphoreImpl activate;     ///< signal when we have a new tracker
    SemaphoreImpl trackerMutex; ///< protect the tracker during outside access
    yarp::os::PortWriter *cachedWriter;   ///< the message the send
    yarp::os::PortReader *cachedReader;   ///< where to put a reply
    yarp::os::PortWriter *cachedCallback; ///< where to sent commencement and
                                          ///< completion events
    void *cachedTracker;        ///< memory tracker for current message
    ConstString cachedEnvelope;      ///< some text to pass along with the message

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

#endif // YARP_OS_IMPL_PORTCOREOUTPUTUNIT_H
