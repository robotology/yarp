/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreOutputUnit.h>

#include <yarp/os/Name.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/Portable.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PortCommand.h>

namespace {
YARP_OS_LOG_COMPONENT(PORTCOREOUTPUTUNIT, "yarp.os.impl.PortCoreOutputUnit")
} // namespace

using namespace yarp::os::impl;
using namespace yarp::os;

PortCoreOutputUnit::PortCoreOutputUnit(PortCore& owner, int index, OutputProtocol* op) :
        PortCoreUnit(owner, index),
        op(op), // shared_ptr takes ownership of the raw pointer
        closing(false),
        finished(false),
        running(false),
        threaded(false),
        sending(false),
        name(owner.getName()),
        phase(1),
        activate(0),
        trackerMutex(),
        cachedWriter(nullptr),
        cachedReader(nullptr),
        cachedCallback(nullptr),
        cachedTracker(nullptr)
{
    yCIAssert(PORTCOREOUTPUTUNIT, getName(), op != nullptr);
}

PortCoreOutputUnit::~PortCoreOutputUnit()
{
    closeMain();
}


bool PortCoreOutputUnit::start()
{
    phase.wait();

    if (!threaded) {
        running = false;
        sending = false;
        runSingleThreaded();
        phase.post();
        return true;
    }

    bool result = PortCoreUnit::start();
    if (result) {
        phase.wait();
        phase.post();
    } else {
        phase.post();
    }

    return result;
}


void PortCoreOutputUnit::run()
{
    running = true;
    sending = false;

    // By default, we don't start up a thread for outputs.

    if (!threaded) {
        runSingleThreaded();
        phase.post();
    } else {
        phase.post();
        Route r = getRoute();
        while (!closing) {
            yCIDebug(PORTCOREOUTPUTUNIT, getName(), "waiting");
            activate.wait();
            yCIDebug(PORTCOREOUTPUTUNIT, getName(), "woken");
            if (!closing) {
                if (sending) {
                    yCIDebug(PORTCOREOUTPUTUNIT, getName(), "write something in background");
                    sendHelper();
                    yCIDebug(PORTCOREOUTPUTUNIT, getName(), "wrote something in background");
                    trackerMutex.lock();
                    if (cachedTracker != nullptr) {
                        void* t = cachedTracker;
                        cachedTracker = nullptr;
                        sending = false;
                        getOwner().notifyCompletion(t);
                    } else {
                        sending = false;
                    }
                    trackerMutex.unlock();
                }
            }
            yCIDebug(PORTCOREOUTPUTUNIT, getName(), "wrote something in background");
        }
        yCIDebug(PORTCOREOUTPUTUNIT, getName(), "thread closing");
        sending = false;
    }
}


void PortCoreOutputUnit::runSingleThreaded()
{
    // Local copy: guarantees the object stays alive for the duration of
    // this function, independent of concurrent modifications to `op`.
    std::shared_ptr<OutputProtocol> localOp = op;

    if (localOp) {
        Route route = localOp->getRoute();
        setMode();
        getOwner().reportUnit(this, true);

        std::string msg = std::string("Sending output from ") + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
        if (Name(route.getToName()).isRooted()) {
            if (Name(route.getFromName()).isRooted()) {
                yCIInfo(PORTCOREOUTPUTUNIT, getName(), "%s", msg.c_str());
            }
        }

        // Report the new connection
        PortInfo info;
        info.message = msg;
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = false;
        info.created = true;
        info.sourceName = route.getFromName();
        info.targetName = route.getToName();
        info.portName = info.sourceName;
        info.carrierName = route.getCarrierName();
        getOwner().report(info);
    }

    // no thread component
    running = false;
}

void PortCoreOutputUnit::closeBasic()
{
    bool waitForOther = false;

    // Take a local, private, strong reference. This keeps the pointed-to
    // OutputProtocol alive for the whole function, even if another thread
    // concurrently resets the shared `op` member (see closeMain()).
    std::shared_ptr<OutputProtocol> localOp = op;

    if (localOp) {
        localOp->getConnection().prepareDisconnect();
        Route route = localOp->getRoute();
        if (localOp->getConnection().isConnectionless() || localOp->getConnection().isBroadcast()) {
            yCIInfo(PORTCOREOUTPUTUNIT, getName(), "output for route %s asking other side to close by out-of-band means",
                       route.toString().c_str());
            NetworkBase::disconnectInput(route.getToName(),
                                         route.getFromName(),
                                         true);
        } else {
            if (localOp->getConnection().canEscape()) {
                BufferedConnectionWriter buf(localOp->getConnection().isTextMode(),
                                             localOp->getConnection().isBareMode());
                PortCommand pc('\0', std::string("q"));
                pc.write(buf);
                //printf("Asked for %s to close...\n",
                //     op->getRoute().toString().c_str());
                waitForOther = localOp->write(buf);
            }
        }

        std::string msg = std::string("Removing output from ") + route.getFromName() + " to " + route.getToName();

        if (Name(route.getToName()).isRooted()) {
            if (Name(route.getFromName()).isRooted()) {
                yCIInfo(PORTCOREOUTPUTUNIT, getName(), "%s", msg.c_str());
            }
        }

        getOwner().reportUnit(this, false);

        // Report the disappearing connection
        PortInfo info;
        info.message = msg;
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = false;
        info.created = false;
        info.sourceName = route.getFromName();
        info.targetName = route.getToName();
        info.portName = info.sourceName;
        info.carrierName = route.getCarrierName();
        getOwner().report(info);
    }


    if (localOp) {
        if (waitForOther) {
            // quit is only acknowledged in certain conditions
            if (localOp->getConnection().isTextMode() && localOp->getConnection().supportReply()) {
                InputStream& is = localOp->getInputStream();
                ManagedBytes dummy(1);
                is.read(dummy.bytes());
            }
        }
        localOp->close();

        // Release our copy and clear the shared member. The underlying
        // OutputProtocol is only actually destroyed once every other
        // outstanding shared_ptr copy (e.g. localOp here, or one held
        // briefly by another thread in closeMain()/getRoute()) has also
        // gone out of scope. No explicit delete or additional locking is
        // needed: shared_ptr's atomic refcounting handles this safely.
        op.reset();
    }
}

void PortCoreOutputUnit::closeMain()
{
    if (finished) {
        return;
    }

    yCIDebug(PORTCOREOUTPUTUNIT, getName(), "closing");

    if (running) {
        // give a kick (unfortunately unavoidable)

        // Local copy so that even if another thread concurrently clears
        // `op` inside closeBasic(), the object we call interrupt() on
        // (if any) remains valid for the duration of this call.
        std::shared_ptr<OutputProtocol> localOp = op;
        if (localOp) {
            localOp->interrupt();
        }

        closing = true;
        phase.post();
        activate.post();
        join();
    }

    yCIDebug(PORTCOREOUTPUTUNIT, getName(), "internal join");

    closeBasic();
    running = false;
    closing = false;
    finished = true;

    yCIDebug(PORTCOREOUTPUTUNIT, getName(), "closed");
}


Route PortCoreOutputUnit::getRoute()
{
    std::shared_ptr<OutputProtocol> localOp = op;
    if (localOp) {
        Route r = localOp->getRoute();
        localOp->beginWrite();
        return r;
    }
    return PortCoreUnit::getRoute();
}

bool PortCoreOutputUnit::sendHelper()
{
    bool replied = false;

    // Local copy for the whole helper, for the same reason as elsewhere.
    std::shared_ptr<OutputProtocol> localOp = op;

    if (localOp) {
        bool done = false;
        BufferedConnectionWriter buf(localOp->getConnection().isTextMode(),
                                     localOp->getConnection().isBareMode());
        if (cachedReader != nullptr) {
            buf.setReplyHandler(*cachedReader);
        }

        if (localOp->getSender().modifiesOutgoingData()) {
            if (localOp->getSender().acceptOutgoingData(*cachedWriter)) {
                cachedWriter = &localOp->getSender().modifyOutgoingData(*cachedWriter);
            } else {
                return (done = true);
            }
        }

        if (localOp->getConnection().isLocal()) {
            // WARNING Cast away const qualifier.
            //         This may actually cause bugs when using the local carrier
            //         with something that is actually const (i.e. that is using
            //         some parts of memory that cannot be written.
            auto* pw = const_cast<yarp::os::PortWriter*>(cachedWriter);
            auto* p = dynamic_cast<yarp::os::Portable*>(pw);
            if (p == nullptr) {
                yCIError(PORTCOREOUTPUTUNIT, getName(), "cast failed.");
                return false;
            }
            buf.setReference(p);
        } else {
            yCIAssert(PORTCOREOUTPUTUNIT, getName(), cachedWriter != nullptr);
            bool ok = cachedWriter->write(buf);
            if (!ok) {
                done = true;
            }

            bool suppressReply = (buf.getReplyHandler() == nullptr);

            if (!done) {
                if (!localOp->getConnection().canEscape()) {
                    if (!cachedEnvelope.empty()) {
                        localOp->getConnection().handleEnvelope(cachedEnvelope);
                    }
                } else {
                    buf.addToHeader();

                    if (!cachedEnvelope.empty()) {
                        if (cachedEnvelope == "__ADMIN") {
                            PortCommand pc('a', "");
                            pc.write(buf);
                        } else {
                            PortCommand pc('\0', std::string(suppressReply ? "D " : "d ") + cachedEnvelope);
                            pc.write(buf);
                        }
                    } else {
                        PortCommand pc(suppressReply ? 'D' : 'd', "");
                        pc.write(buf);
                    }
                }
            }
        }

        if (!done) {
            if (localOp->getConnection().isActive()) {
                replied = localOp->write(buf);
                if (replied && localOp->getSender().modifiesReply() && cachedReader != nullptr) {
                    cachedReader = &localOp->getSender().modifyReply(*cachedReader);
                }
            }
            if (!localOp->isOk()) {
                done = true;
            }
        }

        if (buf.dropRequested()) {
            done = true;
        }
        if (done) {
            closeBasic();
            closing = true;
            finished = true;
            setDoomed();
        }
    }


    return replied;
}

void* PortCoreOutputUnit::send(const yarp::os::PortWriter& writer,
                               yarp::os::PortReader* reader,
                               const yarp::os::PortWriter* callback,
                               void* tracker,
                               const std::string& envelopeString,
                               bool waitAfter,
                               bool waitBefore,
                               bool* gotReply)
{
    bool replied = false;

    {
        std::shared_ptr<OutputProtocol> localOp = op;
        if (localOp) {
            if (!localOp->getConnection().isActive()) {
                return tracker;
            }
        }
    }

    if (!waitBefore || !waitAfter) {
        if (!running) {
            // we must have a thread if we're going to be skipping waits
            threaded = true;
            yCIDebug(PORTCOREOUTPUTUNIT, getName(), "starting a thread for output");
            start();
            yCIDebug(PORTCOREOUTPUTUNIT, getName(), "started a thread for output");
        }
    }

    if ((!waitBefore) && waitAfter) {
        yCIError(PORTCOREOUTPUTUNIT, getName(), "chosen port wait combination not yet implemented");
    }
    if (!sending) {
        cachedWriter = &writer;
        cachedReader = reader;
        cachedCallback = callback;
        cachedEnvelope = envelopeString;

        sending = true;
        if (waitAfter) {
            replied = sendHelper();
            sending = false;
        } else {
            trackerMutex.lock();
            void* nextTracker = tracker;
            tracker = cachedTracker;
            cachedTracker = nextTracker;
            activate.post();
            trackerMutex.unlock();
        }
    } else {
        yCIDebug(PORTCOREOUTPUTUNIT, getName(), "skipping connection tagged as sending something");
    }

    if (waitAfter) {
        if (gotReply != nullptr) {
            *gotReply = replied;
        }
    }

    // return tracker that we no longer need
    return tracker;
}


void* PortCoreOutputUnit::takeTracker()
{
    void* tracker = nullptr;
    trackerMutex.lock();
    if (!sending) {
        tracker = cachedTracker;
        cachedTracker = nullptr;
    }
    trackerMutex.unlock();
    return tracker;
}

bool PortCoreOutputUnit::isBusy()
{
    return sending;
}

void PortCoreOutputUnit::setCarrierParams(const yarp::os::Property& params)
{
    std::shared_ptr<OutputProtocol> localOp = op;
    if (localOp) {
        localOp->getConnection().setCarrierParams(params);
    }
}

void PortCoreOutputUnit::getCarrierParams(yarp::os::Property& params)
{
    std::shared_ptr<OutputProtocol> localOp = op;
    if (localOp) {
        localOp->getConnection().getCarrierParams(params);
    }
}

OutputProtocol* PortCoreOutputUnit::getOutPutProtocol()
{
    // Callers (e.g. PortCore::getTypeOfService()) treat this as a
    // transient, "use immediately" raw pointer; they do not store it
    // long-term. Returning op.get() preserves the existing API/ABI while
    // the shared_ptr keeps the object's lifetime managed safely elsewhere.
    return op.get();
}
