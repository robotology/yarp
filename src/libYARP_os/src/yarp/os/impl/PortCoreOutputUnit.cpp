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
        op(op),
        closing(false),
        finished(false),
        running(false),
        threaded(false),
        sending(false),
        phase(1),
        activate(0),
        trackerMutex(),
        cachedWriter(nullptr),
        cachedReader(nullptr),
        cachedCallback(nullptr),
        cachedTracker(nullptr)
{
    yCAssert(PORTCOREOUTPUTUNIT, op != nullptr);
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
            yCDebug(PORTCOREOUTPUTUNIT, "waiting");
            activate.wait();
            yCDebug(PORTCOREOUTPUTUNIT, "woken");
            if (!closing) {
                if (sending) {
                    yCDebug(PORTCOREOUTPUTUNIT, "write something in background");
                    sendHelper();
                    yCDebug(PORTCOREOUTPUTUNIT, "wrote something in background");
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
            yCDebug(PORTCOREOUTPUTUNIT, "wrote something in background");
        }
        yCDebug(PORTCOREOUTPUTUNIT, "thread closing");
        sending = false;
    }
}


void PortCoreOutputUnit::runSingleThreaded()
{
    if (op != nullptr) {
        Route route = op->getRoute();
        setMode();
        getOwner().reportUnit(this, true);

        std::string msg = std::string("Sending output from ") + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
        if (Name(route.getToName()).isRooted()) {
            if (Name(route.getFromName()).isRooted()) {
                yCInfo(PORTCOREOUTPUTUNIT, "%s", msg.c_str());
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
    if (op != nullptr) {
        op->getConnection().prepareDisconnect();
        Route route = op->getRoute();
        if (op->getConnection().isConnectionless() || op->getConnection().isBroadcast()) {
            yCInfo(PORTCOREOUTPUTUNIT, "output for route %s asking other side to close by out-of-band means",
                       route.toString().c_str());
            NetworkBase::disconnectInput(route.getToName(),
                                         route.getFromName(),
                                         true);
        } else {
            if (op->getConnection().canEscape()) {
                BufferedConnectionWriter buf(op->getConnection().isTextMode(),
                                             op->getConnection().isBareMode());
                PortCommand pc('\0', std::string("q"));
                pc.write(buf);
                //printf("Asked for %s to close...\n",
                //     op->getRoute().toString().c_str());
                waitForOther = op->write(buf);
            }
        }

        std::string msg = std::string("Removing output from ") + route.getFromName() + " to " + route.getToName();

        if (Name(route.getToName()).isRooted()) {
            if (Name(route.getFromName()).isRooted()) {
                yCInfo(PORTCOREOUTPUTUNIT, "%s", msg.c_str());
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


    if (op != nullptr) {
        if (waitForOther) {
            // quit is only acknowledged in certain conditions
            if (op->getConnection().isTextMode() && op->getConnection().supportReply()) {
                InputStream& is = op->getInputStream();
                ManagedBytes dummy(1);
                is.read(dummy.bytes());
            }
        }
        op->close();
        delete op;
        op = nullptr;
    }
}

void PortCoreOutputUnit::closeMain()
{
    if (finished) {
        return;
    }

    yCDebug(PORTCOREOUTPUTUNIT, "closing");

    if (running) {
        // give a kick (unfortunately unavoidable)

        if (op != nullptr) {
            op->interrupt();
        }

        closing = true;
        phase.post();
        activate.post();
        join();
    }

    yCDebug(PORTCOREOUTPUTUNIT, "internal join");

    closeBasic();
    running = false;
    closing = false;
    finished = true;

    yCDebug(PORTCOREOUTPUTUNIT, "closed");
}


Route PortCoreOutputUnit::getRoute()
{
    if (op != nullptr) {
        Route r = op->getRoute();
        op->beginWrite();
        return r;
    }
    return PortCoreUnit::getRoute();
}

bool PortCoreOutputUnit::sendHelper()
{
    bool replied = false;
    if (op != nullptr) {
        bool done = false;
        BufferedConnectionWriter buf(op->getConnection().isTextMode(),
                                     op->getConnection().isBareMode());
        if (cachedReader != nullptr) {
            buf.setReplyHandler(*cachedReader);
        }

        if (op->getSender().modifiesOutgoingData()) {
            if (op->getSender().acceptOutgoingData(*cachedWriter)) {
                cachedWriter = &op->getSender().modifyOutgoingData(*cachedWriter);
            } else {
                return (done = true);
            }
        }

        if (op->getConnection().isLocal()) {
            // WARNING Cast away const qualifier.
            //         This may actually cause bugs when using the local carrier
            //         with something that is actually const (i.e. that is using
            //         some parts of memory that cannot be written.
            auto* pw = const_cast<yarp::os::PortWriter*>(cachedWriter);
            auto* p = dynamic_cast<yarp::os::Portable*>(pw);
            if (p == nullptr) {
                yCError(PORTCOREOUTPUTUNIT, "cast failed.");
                return false;
            }
            buf.setReference(p);
        } else {
            yCAssert(PORTCOREOUTPUTUNIT, cachedWriter != nullptr);
            bool ok = cachedWriter->write(buf);
            if (!ok) {
                done = true;
            }

            bool suppressReply = (buf.getReplyHandler() == nullptr);

            if (!done) {
                if (!op->getConnection().canEscape()) {
                    if (!cachedEnvelope.empty()) {
                        op->getConnection().handleEnvelope(cachedEnvelope);
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
            if (op->getConnection().isActive()) {
                replied = op->write(buf);
                if (replied && op->getSender().modifiesReply() && cachedReader != nullptr) {
                    cachedReader = &op->getSender().modifyReply(*cachedReader);
                }
            }
            if (!op->isOk()) {
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

    if (op != nullptr) {
        if (!op->getConnection().isActive()) {
            return tracker;
        }
    }

    if (!waitBefore || !waitAfter) {
        if (!running) {
            // we must have a thread if we're going to be skipping waits
            threaded = true;
            yCDebug(PORTCOREOUTPUTUNIT, "starting a thread for output");
            start();
            yCDebug(PORTCOREOUTPUTUNIT, "started a thread for output");
        }
    }

    if ((!waitBefore) && waitAfter) {
        yCError(PORTCOREOUTPUTUNIT, "chosen port wait combination not yet implemented");
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
        yCDebug(PORTCOREOUTPUTUNIT, "skipping connection tagged as sending something");
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
    if (op != nullptr) {
        op->getConnection().setCarrierParams(params);
    }
}

void PortCoreOutputUnit::getCarrierParams(yarp::os::Property& params)
{
    if (op != nullptr) {
        op->getConnection().getCarrierParams(params);
    }
}

OutputProtocol* PortCoreOutputUnit::getOutPutProtocol()
{
    return op;
}
