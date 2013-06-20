// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */



#include <yarp/os/Time.h>
#include <yarp/os/Portable.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/impl/PortCoreOutputUnit.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Name.h>
#include <yarp/os/impl/Companion.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp::os::impl;
using namespace yarp::os;

bool PortCoreOutputUnit::start() {

    phase.wait();

    if (!threaded) {
        running = false;
        sending = false;
        runSimulation();
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


void PortCoreOutputUnit::run() {
    running = true;
    sending = false;

    // By default, we don't start up a thread for outputs.

    if (!threaded) {
        runSimulation();
        phase.post();
    } else {
        phase.post();
        Route r = getRoute();
        Logger log(r.toString().c_str(),Logger::get());
        while (!closing) {
            YARP_DEBUG(log, "PortCoreOutputUnit waiting");
            activate.wait();
            YARP_DEBUG(log, "PortCoreOutputUnit woken");
            if (!closing) {
                if (sending) {
                    YARP_DEBUG(log, "write something in background");
                    sendHelper();
                    YARP_DEBUG(log, "wrote something in background");
                    trackerMutex.wait();
                    if (cachedTracker!=NULL) {
                        void *t = cachedTracker;
                        cachedTracker = NULL;
                        sending = false;
                        getOwner().notifyCompletion(t);
                    } else {
                        sending = false;
                    }
                    trackerMutex.post();
                }
            }
            YARP_DEBUG(log, "wrote something in background");
        }
        YARP_DEBUG(log,
                   "PortCoreOutputUnit thread closing");
        sending = false;
    }
}



void PortCoreOutputUnit::runSimulation() {

    if (op!=NULL) {
        Route route = op->getRoute();
        setMode();
        getOwner().reportUnit(this,true);

        String msg = String("Sending output from ") +
            route.getFromName() + " to " + route.getToName() + " using " +
            route.getCarrierName();
        if (Name(route.getToName()).isRooted()) {
            if (Name(route.getFromName()).isRooted()) {
                YARP_INFO(Logger::get(),msg);
            }
        }

        // Report the new connection
        PortInfo info;
        info.message = msg.c_str();
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = false;
        info.created = true;
        info.sourceName = route.getFromName().c_str();
        info.targetName = route.getToName().c_str();
        info.portName = info.sourceName;
        info.carrierName = route.getCarrierName().c_str();
        getOwner().report(info);
    }

    // no thread component
    running = false;

    return;
}

void PortCoreOutputUnit::closeBasic() {
    bool waitForOther = false;
    if (op!=NULL) {
	op->prepareDisconnect();
        Route route = op->getRoute();
        if (op->getConnection().isConnectionless()||
            op->getConnection().isBroadcast()) {
            YARP_SPRINTF1(Logger::get(),
                         debug,
                         "output for route %s asking other side to close by out-of-band means",
                         route.toString().c_str());
            Companion::disconnectInput(route.getToName().c_str(),
                                       route.getFromName().c_str(),true);
        } else {
            if (op->getConnection().canEscape()) {
                BufferedConnectionWriter buf(op->getConnection().isTextMode());
                PortCommand pc('\0',String("q"));
                pc.write(buf);
                //printf("Asked for %s to close...\n",
                //     op->getRoute().toString().c_str());
                op->write(buf);
                waitForOther = true;
            }
        }

        String msg = String("Removing output from ") +
            route.getFromName() + " to " + route.getToName();

        if (Name(route.getToName()).isRooted()) {
            YARP_INFO(Logger::get(), msg);
        }

        getOwner().reportUnit(this,false);

        // Report the disappearing connection
        PortInfo info;
        info.message = msg.c_str();
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = false;
        info.created = false;
        info.sourceName = route.getFromName().c_str();
        info.targetName = route.getToName().c_str();
        info.portName = info.sourceName;
        info.carrierName = route.getCarrierName().c_str();
        getOwner().report(info);
    }


    if (op!=NULL) {
        if (waitForOther) {
            // quit is only acknowledged in certain conditions
            if (op->getConnection().isTextMode()&&
                op->getConnection().supportReply()) {
                InputStream& is = op->getInputStream();
                ManagedBytes dummy(1);
                is.read(dummy.bytes());
            }
        }
        op->close();
        delete op;
        op = NULL;
    }
}

void PortCoreOutputUnit::closeMain() {
    if (finished) return;

    YARP_DEBUG(Logger::get(),"PortCoreOutputUnit closing");

    if (running) {
        // give a kick (unfortunately unavoidable)

        if (op!=NULL) {
            op->interrupt();
        }

        closing = true;
        phase.post();
        activate.post();
        join();
    }

    YARP_DEBUG(Logger::get(),"PortCoreOutputUnit internal join");

    closeBasic();
    running = false;
    closing = false;
    finished = true;

    YARP_DEBUG(Logger::get(),"PortCoreOutputUnit closed");
}


Route PortCoreOutputUnit::getRoute() {
    if (op!=NULL) {
        return op->getRoute();
    }
    return PortCoreUnit::getRoute();
}

bool PortCoreOutputUnit::sendHelper() {
    bool replied = false;
    bool done = false;
    if (op!=NULL) {

        BufferedConnectionWriter buf(op->getConnection().isTextMode());
        if (cachedReader!=NULL) {
            buf.setReplyHandler(*cachedReader);
        }


        if (op->getConnection().isLocal()) {
            buf.setReference(dynamic_cast<yarp::os::Portable *>
                             (cachedWriter));
        } else {


            YARP_ASSERT(cachedWriter!=NULL);
            bool ok = cachedWriter->write(buf);
            if (!ok) {
                done = true;
            }

            bool suppressReply = (buf.getReplyHandler()==NULL);

            if (op->getConnection().canEscape()&&!done) {
                buf.addToHeader();

                if (cachedEnvelope!="") {
                    // this will be the new way to signal that replies
                    // are not expected
                    //PortCommand pc('\0', String(suppressReply?"D ":"d ") +
                    //             cachedEnvelope);
                    //pc.writeBlock(buf);

                    // This is the backwards-compatible method.
                    // To be used until YARP 2.1.2 is a "long time ago".
                    if (cachedEnvelope=="__ADMIN") {
                        PortCommand pc('a', "");
                        pc.write(buf);
                    } else {
                        PortCommand pc('\0', String(suppressReply?"do ":"d ") +
                                       cachedEnvelope);
                        pc.write(buf);
                    }

                } else {
                    // this will be the new way to signal that replies
                    // are not expected
                    //PortCommand pc(suppressReply?'D':'d',"");
                    //pc.writeBlock(buf);

                    // This is the backwards-compatible method.
                    // To be used until YARP 2.1.2 is a "long time ago".
                    if (suppressReply) {
                        PortCommand pc('\0', "do");
                        pc.write(buf);
                    } else {
                        PortCommand pc('d', "");
                        pc.write(buf);
                    }
                }
            }
        }

        if (!done) {
            if (op->getConnection().isActive()) {
                replied = op->write(buf);
            }
            if (!op->isOk()) {
                done = true;
            }
        }
        
        if (buf.dropRequested()) {
            done = true;
        }
    }
    if (done) {
        closeBasic();
        finished = true;
        closing = true;
        setDoomed(true);
    }
    return replied;
}

void *PortCoreOutputUnit::send(yarp::os::PortWriter& writer,
                               yarp::os::PortReader *reader,
                               yarp::os::PortWriter *callback,
                               void *tracker,
                               const String& envelopeString,
                               bool waitAfter,
                               bool waitBefore,
                               bool *gotReply) {
    bool replied = false;

    if (!waitBefore || !waitAfter) {
        if (running == false) {
            // we must have a thread if we're going to be skipping waits
            threaded = true;
            YARP_DEBUG(Logger::get(),"starting a thread for output");
            start();
            YARP_DEBUG(Logger::get(),"started a thread for output");
        }
    }

    if ((!waitBefore)&&waitAfter) {
        YARP_ERROR(Logger::get(), "chosen port wait combination not yet implemented");
    }
    if (!sending) {
        cachedWriter = &writer;
        cachedReader = reader;
        cachedCallback = callback;
        cachedEnvelope = envelopeString;

        sending = true;
        if (waitAfter==true) {
            replied = sendHelper();
            sending = false;
        } else {
            trackerMutex.wait();
            void *nextTracker = tracker;
            tracker = cachedTracker;
            cachedTracker = nextTracker;
            activate.post();
            trackerMutex.post();
        }
    } else {
        YARP_DEBUG(Logger::get(),
                   "skipping connection tagged as sending something");
    }

    if (waitAfter) {
        if (gotReply!=NULL) {
            *gotReply = replied;
        }
    }

    // return tracker that we no longer need
    return tracker;
}


void *PortCoreOutputUnit::takeTracker() {
    void *tracker = NULL;
    trackerMutex.wait();
    if (!sending) {
        tracker = cachedTracker;
        cachedTracker = NULL;
    }
    trackerMutex.post();
    return tracker;
}

bool PortCoreOutputUnit::isBusy() {
    return sending;
}



