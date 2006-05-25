// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/InputProtocol.h>
#include <yarp/Logger.h>
#include <yarp/PortCore.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/NameClient.h>
#include <yarp/PortCoreInputUnit.h>
#include <yarp/PortCoreOutputUnit.h>
#include <yarp/Name.h>

#include <ace/OS_NS_stdio.h>


//#define YMSG(x) ACE_OS::printf x;
//#define YTRACE(x) YMSG(("at %s\n",x))

#define YMSG(x) 
#define YTRACE(x) 

using namespace yarp;

/*
  Phases:
  dormant
  listening
  running
*/

PortCore::~PortCore() {
    closeMain();
}


bool PortCore::listen(const Address& address) {
    bool success = false;

    YTRACE("PortCore::listen");

    if (!address.isValid()) {
        YARP_ERROR(log, "Port does not have a valid address");
        return false;
    }

    YARP_ASSERT(address.isValid());
    
    // try to enter listening phase
    stateMutex.wait();
    YARP_ASSERT(listening==false);
    YARP_ASSERT(running==false);
    YARP_ASSERT(closing==false);
    YARP_ASSERT(finished==false);
    YARP_ASSERT(face==NULL);
    this->address = address;
    setName(address.getRegName());

    try {
        face = Carriers::listen(address);
        if (face==NULL) {
            throw IOException("no carrier");
        }
    } catch (IOException e) {
        //YMSG(("listen failed: %s\n",e.toString().c_str()));
        if (face!=NULL) {
            face->close();
            delete face;
        }
        stateMutex.post();
        throw e;
    }
    if (face!=NULL) {
        listening = true;
        success = true;
    }

    if (success) {
        log.setPrefix(address.getRegName().c_str());
    }

    stateMutex.post();

    // we have either entered listening phase (face=valid, listening=true)
    // or remained in dormant phase

    return success;
}


void PortCore::setReadHandler(Readable& reader) {
    YARP_ASSERT(running==false);
    YARP_ASSERT(this->reader==NULL);
    this->reader = &reader;
}



void PortCore::run() {
    YTRACE("PortCore::run");

    // enter running phase
    YARP_ASSERT(listening==true);
    YARP_ASSERT(running==false);
    YARP_ASSERT(closing==false);
    YARP_ASSERT(finished==false);
    YARP_ASSERT(starting==true); // can only run if called from start
    running = true;
    starting = false;
    stateMutex.post();

    YTRACE("PortCore::run running");

    // main loop
    bool shouldStop = false;
    while (!shouldStop) {

        // block and wait for an event
        InputProtocol *ip = NULL;
        try {
            ip = face->read();
            YARP_DEBUG(log,"PortCore got something");
        } catch (IOException e) {
            YMSG(("read failed: %s\n",e.toString().c_str()));
        }

        // got an event, but before processing it, we check whether
        // we should shut down
        stateMutex.wait();
        shouldStop |= closing;
        events++;
        //YMSG(("*** event count boost to %d\n", events));
        stateMutex.post();

        if (!shouldStop) {
            // process event
            //YMSG(("PortCore::run got something, but no processing yet\n"));
            addInput(ip);
            ip = NULL;
        }

        // the event normally gets handed off.  If it remains, delete it.
        if (ip!=NULL) {
            try {
                ip->close();
                delete ip;
            } catch (IOException e) {
                YMSG(("input protocol close failed: %s\n",e.toString().c_str()));
            }
            ip = NULL;
        }
        reapUnits();
    }


    YTRACE("PortCore::run closing");

    // closing phase
    stateMutex.wait();
    finished = true;
    stateMutex.post();
}


void PortCore::close() {
    closeMain();
}


bool PortCore::start() {
    YTRACE("PortCore::start");

    stateMutex.wait();
    YARP_ASSERT(listening==true);
    YARP_ASSERT(running==false);
    YARP_ASSERT(starting==false);
    YARP_ASSERT(finished==false);
    YARP_ASSERT(closing==false);
    starting = true;
    bool started = ThreadImpl::start();
    if (!started) {
        // run() won't be happening
        stateMutex.post();
    } else {
        // wait for run() to change state
        stateMutex.wait();
        YARP_ASSERT(running==true);
        stateMutex.post();
    }
    return started;
}



void PortCore::closeMain() {
    YTRACE("PortCore::closeMain");

    stateMutex.wait();
    bool stopRunning = running;
    stateMutex.post();

    if (stopRunning) {
        // we need to stop the thread
        stateMutex.wait();
        closing = true;
        stateMutex.post();
        try {
            // wake it up
            OutputProtocol *op = face->write(address);
            if (op!=NULL) {
                op->close();
                delete op;
            }
        } catch (IOException e) {
            // no problem
        }
        join();

        // should be finished
        stateMutex.wait();
        YARP_ASSERT(finished==true);
        stateMutex.post();
    
        // should down units - this is the only time it is valid to do this
        closeUnits();

        stateMutex.wait();
        finished = false;
        closing = false;
        running = false;
        stateMutex.post();

        String name = getName();
        if (name!=String("")) {
            NameClient::getNameClient().unregisterName(name);
        }
    }

    // there should be no other threads at this point
    // can stop listening

    if (listening) {
        YARP_ASSERT(face!=NULL);
        try {
            face->close();
            delete face;
        } catch (IOException e) {
            YMSG(("face close failed: %s\n",e.toString().c_str()));
        }
        face = NULL;
        listening = false;
    }

    // fresh as a daisy
    YARP_ASSERT(listening==false);
    YARP_ASSERT(running==false);
    YARP_ASSERT(starting==false);
    YARP_ASSERT(closing==false);
    YARP_ASSERT(finished==false);
    YARP_ASSERT(face==NULL);
}


int PortCore::getEventCount() {
    stateMutex.wait();
    int ct = events;
    stateMutex.post();
    return ct;
}


void PortCore::closeUnits() {
    stateMutex.wait();
    YARP_ASSERT(finished==true); // this is the only valid phase for this
    stateMutex.post();

    // in the "finished" phase, nobody else touches the units,
    // so we can go ahead and shut them down and delete them

    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit!=NULL) {
            YARP_DEBUG(log,"closing a unit");
            unit->close();
            YARP_DEBUG(log,"joining a unit");
            unit->join();
            delete unit;
            YARP_DEBUG(log,"deleting a unit");
            units[i] = NULL;
        }
    }
    units.clear();
    //YMSG(("closeUnits: there are now %d units\n", units.size()));
}

void PortCore::reapUnits() {
    stateMutex.wait();
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=NULL) {
                if (unit->isDoomed()&&!unit->isFinished()) {	
                    YARP_DEBUG(log,"REAPING a unit");
                    unit->close();
                    unit->join();
                    YARP_DEBUG(log,"done REAPING a unit");
                }
            }
        }
    }
    stateMutex.post();
    cleanUnits();
}

void PortCore::cleanUnits() {
    YARP_DEBUG(log,"CLEANING scan");
    stateMutex.wait();
    if (!finished) {
    
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=NULL) {
                YARP_DEBUG(log,String("checking ") + unit->getRoute().toString());
                if (unit->isFinished()) {
                    YARP_DEBUG(log,"CLEANING a unit");
                    try {
                        unit->close();
                        unit->join();
                    } catch (IOException e) {
                        YARP_DEBUG(log,e.toString() + " <<< cleanUnits error");
                    }
                    delete unit;
                    units[i] = NULL;
                    YARP_DEBUG(log,"done CLEANING a unit");
                }
            }
        }
        unsigned int rem = 0;
        for (unsigned int i2=0; i2<units.size(); i2++) {
            if (units[i2]!=NULL) {
                if (rem<i2) {
                    units[rem] = units[i2];
                    units[i2] = NULL;
                }
                rem++;
            }
        }
        for (unsigned int i3=0; i3<units.size()-rem; i3++) {
            units.pop_back();
        }
        //YMSG(("cleanUnits: there are now %d units\n", units.size()));
    }
    stateMutex.post();
    YARP_DEBUG(log,"CLEANING scan done");
}


// only called by manager, in running phase
void PortCore::addInput(InputProtocol *ip) {
    YARP_ASSERT(ip!=NULL);
    stateMutex.wait();
    PortCoreUnit *unit = new PortCoreInputUnit(*this,ip,autoHandshake);
    YARP_ASSERT(unit!=NULL);
    unit->start();
  
    units.push_back(unit);
    YMSG(("there are now %d units\n", units.size()));
    stateMutex.post();
}


void PortCore::addOutput(OutputProtocol *op) {
    YARP_ASSERT(op!=NULL);
    stateMutex.wait();
    if (!finished) {
        PortCoreUnit *unit = new PortCoreOutputUnit(*this,op);
        YARP_ASSERT(unit!=NULL);
    
        unit->start();
    
        units.push_back(unit);
        //YMSG(("there are now %d units\n", units.size()));
    }
    stateMutex.post();
}


bool PortCore::removeUnit(const Route& route) {
    // a request to remove a unit
    // this is the trickiest case, since any thread could here
    // affect any other thread

    // how about waking up the manager to do this?
    stateMutex.wait();
    bool needReap = false;
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=NULL) {
                Route alt = unit->getRoute();
                String wild = "*";
                bool ok = true;
                if (route.getFromName()!=wild) {
                    ok &= route.getFromName()==alt.getFromName();
                }
                if (route.getToName()!=wild) {
                    ok &= route.getToName()==alt.getToName();
                }
                if (route.getCarrierName()!=wild) {
                    ok &= route.getCarrierName()==alt.getCarrierName();
                }
	
                if (ok) {
                    YARP_DEBUG(log, String("removing unit ") + alt.toString());
                    unit->setDoomed();
                    needReap = true;
                }
            }
        }
    }
    stateMutex.post();
    YARP_DEBUG(log,"should I reap?");
    if (needReap) {
        YARP_DEBUG(log,"reaping...");
        // death will happen in due course; we can speed it up a bit
        // by waking up the grim reaper
        try {
            OutputProtocol *op = face->write(address);
            if (op!=NULL) {
                op->close();
                delete op;
            }
        } catch (IOException e) {
            // no problem
        }
    }
    return needReap;
}




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// PortManager interface
//


void PortCore::addOutput(const String& dest, void *id, OutputStream *os) {
    BufferedConnectionWriter bw(true);

    Address parts = Name(dest).toAddress();
    Address address = NameClient::getNameClient().queryName(parts.getRegName());
    if (address.isValid()) {
        bw.appendLine(String("Adding output to ") + dest);
        OutputProtocol *op = NULL;
        try {
            op = Carriers::connect(address);
            if (op!=NULL) {
                op->open(Route(getName(),address.getRegName(),
                               parts.hasCarrierName()?parts.getCarrierName():"tcp"));
            }
        } catch (IOException e) { /* ok */ }
        if (op!=NULL) {
            addOutput(op);
        } else {
            bw.appendLine(String("error - cannot connect to ") + dest);
        }
    } else {
        bw.appendLine(String("error - do not know how to connect to ") + dest);
    }

    if(os!=NULL) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::removeOutput(const String& dest, void *id, OutputStream *os) {
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route("*",dest,"*"))) {
        bw.appendLine(String("Removing connection from ") + getName() +
                      " to " + dest);
    } else {
        bw.appendLine(String("Could not find an outgoing connection to ") +
                      dest);
    }
    if(os!=NULL) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::removeInput(const String& dest, void *id, OutputStream *os) {
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route(dest,"*","*"))) {
        bw.appendLine(String("Removing connection from ") + dest + " to " +
                      getName());
    } else {
        bw.appendLine(String("Could not find an incoming connection from ") +
                      dest);
    }
    if(os!=NULL) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::describe(void *id, OutputStream *os) {
    cleanUnits();

    BufferedConnectionWriter bw(true);

    stateMutex.wait();

    bw.appendLine(String("This is ") + address.getRegName() + " at " + 
                  address.toString());

    int oct = 0;
    int ict = 0;
    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit!=NULL) {
            if (unit->isOutput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                String msg = "There is an output connection from " + 
                    route.getFromName() +
                    " to " + route.getToName() + " using " + 
                    route.getCarrierName();
                bw.appendLine(msg);
                oct++;
            }
        }
    }
    if (oct<1) {
        bw.appendLine("There are no outgoing connections");
    } 
    for (unsigned int i2=0; i2<units.size(); i2++) {
        PortCoreUnit *unit = units[i2];
        if (unit!=NULL) {
            if (unit->isInput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                String msg = "There is an input connection from " + 
                    route.getFromName() +
                    " to " + route.getToName() + " using " + 
                    route.getCarrierName();
                bw.appendLine(msg);
                ict++;
            }
        }
    }
    if (ict<1) {
        bw.appendLine("There are no incoming connections");
    } 

    stateMutex.post();

    if (os!=NULL) {
        bw.write(*os);
    }
}

void PortCore::readBlock(ConnectionReader& reader, void *id, OutputStream *os) {

    // pass the data on out

    // we are in the context of one of the input threads,
    // so our contact with the PortCore must be absolutely minimal.
    //
    // it is safe to pick up the address of the reader since this is 
    // constant over the lifetime of the input threads.

    if (this->reader!=NULL) {
        this->reader->read(reader);
    }
}


void PortCore::send(Writable& writer, Readable *reader) {

    // pass the data to all output units.
    // for efficiency, it should be converted to block form first.
    // some ports may want text-mode, some may want binary, so there
    // may need to be two caches.

    // for now, just doing a sequential send with no caching.
    YMSG(("------- send in real\n"));

    stateMutex.wait();

    YMSG(("------- send in\n"));
    // The whole darned port is blocked on this operation.
    // How long the operation lasts will depend on these flags:
    //   waitAfterSend and waitBeforeSend,
    // set by setWaitAfterSend() and setWaitBeforeSend()
    if (!finished) {
        PortCorePacket *packet = packets.getFreePacket();
        packet->setContent(&writer);
        YARP_ASSERT(packet!=NULL);
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=NULL) {
                if (unit->isOutput() && !unit->isFinished()) {
                    YMSG(("------- -- inc\n"));
                    packet->inc();
                    YMSG(("------- -- presend\n"));
                    void *out = unit->send(writer,reader,(void *)packet,
                                           waitAfterSend,waitBeforeSend);
                    YMSG(("------- -- send\n"));
                    if (out!=NULL) {
                        ((PortCorePacket *)out)->dec();
                        packets.checkPacket((PortCorePacket *)out);
                    }
                    YMSG(("------- -- dec\n"));
                }
            }
        }
        packet->dec();
        packets.checkPacket(packet);
    }
    //writer.onCompletion();
    YMSG(("------- send out\n"));
    stateMutex.post();
    YMSG(("------- send out real\n"));

}



bool PortCore::isWriting() {
    bool writing = false;

    stateMutex.wait();

    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=NULL) {
                if (unit->isOutput() && !unit->isFinished()) {
                    if (unit->isBusy()) {
                        writing = true;
                    } else {
                        void *tracker = unit->takeTracker();
                        if (tracker!=NULL) {
                            //YARP_INFO(log,"tracker returned...");
                            ((PortCorePacket *)tracker)->dec();
                            packets.checkPacket((PortCorePacket *)tracker);
                        }
                    }
                }
            }
        }
    }

    stateMutex.post();

    return writing;
}

