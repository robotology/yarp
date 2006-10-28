// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */



#include <yarp/os/Time.h>
#include <yarp/PortCoreOutputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/Name.h>
#include <yarp/Companion.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;


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
        while (!closing) {
            activate.wait();
            //YARP_DEBUG(Logger::get(), "request to write");
            if (!closing) {
                if (sending) {
                    YARP_DEBUG(Logger::get(), "write something in background");
                    sendHelper();
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
            YARP_DEBUG(Logger::get(), "wrote something in background");
        }
        YARP_DEBUG(Logger::get(),
                   "PortCoreOutputUnit thread closing");
        sending = false;
    }
}



void PortCoreOutputUnit::runSimulation() {

    if (op!=NULL) {
        Route route = op->getRoute();
        if (Name(route.getToName()).isRooted()) {
            YARP_INFO(Logger::get(),String("Sending output from ") + 
                      route.getFromName() + " to " + route.getToName() + " using " +
                      route.getCarrierName());
        }
    }

    // no thread component at the moment
    running = false;
    return;
}

void PortCoreOutputUnit::closeBasic() {
    if (op!=NULL) {
        Route route = op->getRoute();
        if (op->isConnectionless()) {
            YARP_DEBUG(Logger::get(),"asking other side to close, it is connectionless");
            try {
                Companion::disconnectInput(route.getToName().c_str(),
                                           route.getFromName().c_str(),true);
            } catch (IOException e) {
                YARP_DEBUG(Logger::get(),e.toString() + 
                           " <<< exception during request to close input");
            }
        }
        if (Name(route.getToName()).isRooted()) {
            YARP_INFO(Logger::get(),String("Removing output from ") + 
                      route.getFromName() + " to " + route.getToName());
        }
    }


    if (op!=NULL) {
        try {
            op->close();
        } catch (IOException e) { /*ok*/ }
        try {
            delete op;
        } catch (IOException e) { /*ok*/ }
        op = NULL;
    }
}

void PortCoreOutputUnit::closeMain() {

    if (running) {
        // give a kick (unfortunately unavoidable)
        /*
          if (op!=NULL) {
          op->interrupt();
          }
        */
        closing = true;
        phase.post();
        activate.post();
        join();
    }

    closeBasic();
    running = false;
    closing = false;
    finished = true;
}


Route PortCoreOutputUnit::getRoute() {
    if (op!=NULL) {
        return op->getRoute();
    }
    return PortCoreUnit::getRoute();
}

void PortCoreOutputUnit::sendHelper() {
    try {
        if (op!=NULL) {
            BufferedConnectionWriter buf(op->isTextMode());
            if (cachedReader!=NULL) {
                buf.setReplyHandler(*cachedReader);
            }
            YARP_ASSERT(cachedWriter!=NULL);
            bool ok = cachedWriter->write(buf);
            if (!ok) {
                throw IOException("writer failed");
            }

            buf.addToHeader();

            if (cachedEnvelope!="") {
                //printf("ENVELOPE IS [%s]\n", cachedEnvelope.c_str());
                PortCommand pc('\0',String("d ") + cachedEnvelope);
                pc.writeBlock(buf);
            } else {
                PortCommand pc('d',"");
                pc.writeBlock(buf);
            }

            op->write(buf);
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(), e.toString() + " <<< output exception");
        closeBasic();
        finished = true;
        closing = true;
        setDoomed(true);
    }
}

void *PortCoreOutputUnit::send(Writable& writer, 
                               Readable *reader,
                               void *tracker,
                               const String& envelopeString,
                               bool waitAfter,
                               bool waitBefore) {
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
        cachedEnvelope = envelopeString;

        sending = true;
        if (waitAfter==true) {
            sendHelper();
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



