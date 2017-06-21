/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/InputProtocol.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCoreInputUnit.h>
#include <yarp/os/impl/PortCoreOutputUnit.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/Name.h>

#include <yarp/os/impl/Companion.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/DummyConnector.h>

#include <vector>
#include <cstdio>

#ifdef YARP_HAS_ACE
#  include <ace/INET_Addr.h>
#  include <ace/Sched_Params.h>
#endif

#if defined(__linux__) // used for configuring scheduling properties
#  include <sys/types.h>
#  include <unistd.h>
#  include <dirent.h>
#endif


//#define YMSG(x) printf x;
//#define YTRACE(x) YMSG(("at %s\n", x))

#define YMSG(x)
#define YTRACE(x)

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

PortCore::~PortCore() {
    close();
    removeCallbackLock();
}


bool PortCore::listen(const Contact& address, bool shouldAnnounce) {
    // If we're using ACE, we really need to have it initialized before
    // this point.
    if (!NetworkBase::initialized()) {
        YARP_ERROR(log, "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    YTRACE("PortCore::listen");

    stateMutex.wait();

    // This method assumes we are not already on the network.
    // We can assume this because it is not a user-facing class,
    // and we carefully never call this method again without
    // calling close().
    yAssert(listening==false);
    yAssert(running==false);
    yAssert(closing==false);
    yAssert(finished==false);
    yAssert(face==YARP_NULLPTR);

    // Try to put the port on the network, using the user-supplied
    // address (which may be incomplete).  You can think of
    // this as getting a server socket.
    this->address = address;
    setName(address.getRegName());
    if (timeout>0) {
        this->address.setTimeout(timeout);
    }
    face = Carriers::listen(this->address);

    // We failed, abort.
    if (face==YARP_NULLPTR) {
        stateMutex.post();
        return false;
    }

    // Update our address if it was incomplete.
    if (this->address.getPort()<=0) {
        this->address = face->getLocalAddress();
        if (this->address.getRegName()=="...") {
            this->address.setName(ConstString("/") + this->address.getHost() + "_" + NetType::toString(this->address.getPort()));
            setName(this->address.getRegName());
        }

    }

    // Move into listening phase
    listening = true;
    log.setPrefix(address.getRegName().c_str());
    stateMutex.post();

    // Now that we are on the network, we can let the name server know this.
    if (shouldAnnounce) {
        if (!(NetworkBase::getLocalMode()&&NetworkBase::getQueryBypass()==YARP_NULLPTR)) {
            ConstString portName = address.getRegName().c_str();
            Bottle cmd, reply;
            cmd.addString("announce");
            cmd.addString(portName.c_str());
            ContactStyle style;
            NetworkBase::writeToNameServer(cmd, reply, style);
        }
    }

    // Success!
    return true;
}


void PortCore::setReadHandler(PortReader& reader) {
    // Don't even try to do this when the port is hot, it'll burn you
    yAssert(running==false);
    yAssert(this->reader==YARP_NULLPTR);
    this->reader = &reader;
}

void PortCore::setAdminReadHandler(PortReader& reader) {
    // Don't even try to do this when the port is hot, it'll burn you
    yAssert(running==false);
    yAssert(this->adminReader==YARP_NULLPTR);
    this->adminReader = &reader;
}

void PortCore::setReadCreator(PortReaderCreator& creator) {
    // Don't even try to do this when the port is hot, it'll burn you
    yAssert(running==false);
    yAssert(this->readableCreator==YARP_NULLPTR);
    this->readableCreator = &creator;
}



void PortCore::run() {
    YTRACE("PortCore::run");

    // This is the server thread for the port.  We listen on
    // the network and handle any incoming connections.
    // We don't touch those connections, just shove them
    // in a list and move on.  It is important that this
    // thread doesn't make a connecting client wait just
    // because some other client is slow.

    // We assume that listen() has succeeded and that
    // start() has been called.
    yAssert(listening==true);
    yAssert(running==false);
    yAssert(closing==false);
    yAssert(finished==false);
    yAssert(starting==true);

    // Enter running phase
    running = true;
    starting = false;

    // This post is matched with a wait in start()
    stateMutex.post();

    YTRACE("PortCore::run running");

    // Enter main loop, where we block on incoming connections.
    // The loop is exited when PortCore#closing is set.  One last
    // connection will be needed to de-block this thread and ensure
    // that it checks PortCore#closing.
    bool shouldStop = false;
    while (!shouldStop) {

        // Block and wait for a connection
        InputProtocol *ip = YARP_NULLPTR;
        ip = face->read();

        stateMutex.wait();

        // Attach the connection to this port and update its timeout setting
        if (ip!=YARP_NULLPTR) {
            ip->attachPort(contactable);
            YARP_DEBUG(log, "PortCore received something");
            if (timeout>0) {
                ip->setTimeout(timeout);
            }
        }

        // Check whether we should shut down
        shouldStop |= closing;

        // Increment a global count of connection events
        events++;

        stateMutex.post();

        // It we are not shutting down, spin off the connection.
        // It starts life as an input connection (although it
        // may later morph into an output).
        if (!shouldStop) {
            if (ip!=YARP_NULLPTR) {
                addInput(ip);
            }
            YARP_DEBUG(log, "PortCore spun off a connection");
            ip = YARP_NULLPTR;
        }

        // If the connection wasn't spun off, just shut it down.
        if (ip!=YARP_NULLPTR) {
            ip->close();
            delete ip;
            ip = YARP_NULLPTR;
        }

        // Remove any defunct connections.
        reapUnits();

        // Notify anyone listening for connection changes.
        // This should be using a condition variable once we have them,
        // this is not solid TODO
        stateMutex.wait();
        for (int i=0; i<connectionListeners; i++) {
            connectionChange.post();
        }
        connectionListeners = 0;
        stateMutex.post();
    }

    YTRACE("PortCore::run closing");

    // The server thread is shutting down.
    stateMutex.wait();
    for (int i=0; i<connectionListeners; i++) {
        connectionChange.post();
    }
    connectionListeners = 0;
    finished = true;
    stateMutex.post();
}


void PortCore::close() {
    closeMain();

    if (prop) {
        delete prop;
        prop = YARP_NULLPTR;
    }
    modifier.releaseOutModifier();
    modifier.releaseInModifier();
}


bool PortCore::start() {
    YTRACE("PortCore::start");

    // This wait will, on success, be matched by a post in run()
    stateMutex.wait();

    // We assume that listen() has been called.
    yAssert(listening==true);
    yAssert(running==false);
    yAssert(starting==false);
    yAssert(finished==false);
    yAssert(closing==false);
    starting = true;

    // Start the server thread.
    bool started = ThreadImpl::start();
    if (!started) {
        // run() won't be happening
        stateMutex.post();
    } else {
        // run() will signal stateMutex once it is active
        stateMutex.wait();
        yAssert(running==true);

        // release stateMutex for its normal task of controlling access to state
        stateMutex.post();
    }
    return started;
}


bool PortCore::manualStart(const char *sourceName) {
    // This variant of start() does not create a server thread.
    // That is appropriate if we never expect to receive incoming
    // connections for any reason.  No incoming data, no requests
    // for state information, no requests to change connections,
    // nothing.  We set the port's name to something fake, and
    // act like nothing is wrong.
    interruptible = false;
    manual = true;
    setName(sourceName);
    return true;
}


void PortCore::resume() {
    // We are no longer interrupted.
    interrupted = false;
}

void PortCore::interrupt() {
    // This is a no-op if there is no server thread.
    if (!listening) return;

    // Ignore any future incoming data
    interrupted = true;

    // What about data that is already coming in?
    // If interruptible is not currently set, no worries, the user
    // did not or will not end up blocked on a read.
    if (!interruptible) return;

    // Since interruptible is set, it is possible that the user
    // may be blocked on a read.  We send an empty message,
    // which is reserved for giving blocking readers a chance to
    // update their state.
    stateMutex.wait();
    if (reader!=YARP_NULLPTR) {
        YARP_DEBUG(log, "sending update-state message to listener");
        StreamConnectionReader sbr;
        lockCallback();
        reader->read(sbr);
        unlockCallback();
    }
    stateMutex.post();
}


void PortCore::closeMain() {
    YTRACE("PortCore::closeMain");

    stateMutex.wait();

    // We may not have anything to do.
    if (finishing||!(running||manual)) {
        YTRACE("PortCore::closeMainNothingToDo");
        stateMutex.post();
        return;
    }

    YTRACE("PortCore::closeMainCentral");

    // Move into official "finishing" phase.
    finishing = true;
    YARP_DEBUG(log, "now preparing to shut down port");
    stateMutex.post();

    // Start disconnecting inputs.  We ask the other side of the
    // connection to do this, so it won't come as a surprise.
    // The details of how disconnection works vary by carrier.
    // While we are doing this, the server thread may be still running.
    // This is because other ports may need to talk to the server
    // to organize details of how a connection should be broken down.
    bool done = false;
    ConstString prevName = "";
    while (!done) {
        done = true;
        ConstString removeName = "";
        stateMutex.wait();
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                if (unit->isInput()) {
                    if (!unit->isDoomed()) {
                        Route r = unit->getRoute();
                        ConstString s = r.getFromName();
                        if (s.length()>=1) {
                            if (s[0]=='/') {
                                if (s!=getName()) {
                                    if (s!=prevName) {
                                        removeName = s;
                                        done = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        stateMutex.post();
        if (!done) {
            YARP_DEBUG(log, ConstString("requesting removal of connection from ")+
                       removeName);
            int result = Companion::disconnect(removeName.c_str(),
                                               getName().c_str(),
                                               true);
            if (result!=0) {
                Companion::disconnectInput(getName().c_str(),
                                           removeName.c_str(),
                                           true);
            }
            prevName = removeName;
        }
    }

    // Start disconnecting outputs.  We don't negotiate with the
    // other side, we just break them down.
    done = false;
    while (!done) {
        done = true;
        Route removeRoute;
        stateMutex.wait();
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                if (unit->isOutput()&&!unit->isFinished()) {
                    removeRoute = unit->getRoute();
                    if (removeRoute.getFromName()==getName()) {
                        done = false;
                        break;
                    }
                }
            }
        }
        stateMutex.post();
        if (!done) {
            removeUnit(removeRoute, true);
        }
    }

    stateMutex.wait();
    bool stopRunning = running;
    stateMutex.post();

    // If the server thread is still running, we need to bring it down.
    if (stopRunning) {
        // Let the server thread know we no longer need its services.
        stateMutex.wait();
        closing = true;
        stateMutex.post();

        // Wake up the server thread the only way we can, by sending
        // a message to it.  Then join it, it is done.
        if (!manual) {
            OutputProtocol *op = face->write(address);
            if (op!=YARP_NULLPTR) {
                op->close();
                delete op;
            }
            join();
        }

        // We should be finished now.
        stateMutex.wait();
        yAssert(finished==true);
        stateMutex.post();

        // Clean up our connection list. We couldn't do this earlier,
        // because the server thread was running.
        closeUnits();

        // Reset some state flags.
        stateMutex.wait();
        finished = false;
        closing = false;
        running = false;
        stateMutex.post();
    }

    // There should be no other threads at this point and we
    // can stop listening on the network.
    if (listening) {
        yAssert(face!=YARP_NULLPTR);
        face->close();
        delete face;
        face = YARP_NULLPTR;
        listening = false;
    }

    // Check if the client is waiting for input.  If so, wake them up
    // with the bad news.  An empty message signifies a request to
    // check the port state.
    if (reader!=YARP_NULLPTR) {
        YARP_DEBUG(log, "sending end-of-port message to listener");
        StreamConnectionReader sbr;
        reader->read(sbr);
        reader = YARP_NULLPTR;
    }

    // We may need to unregister the port with the name server.
    if (stopRunning) {
        ConstString name = getName();
        if (name!=ConstString("")) {
            if (controlRegistration) {
                NetworkBase::unregisterName(name.c_str());
            }
        }
    }

    // We are done with the finishing process.
    finishing = false;

    // We are fresh as a daisy.
    yAssert(listening==false);
    yAssert(running==false);
    yAssert(starting==false);
    yAssert(closing==false);
    yAssert(finished==false);
    yAssert(finishing==false);
    yAssert(face==YARP_NULLPTR);
}


int PortCore::getEventCount() {
    // How many times has the server thread spun off a connection.
    stateMutex.wait();
    int ct = events;
    stateMutex.post();
    return ct;
}


void PortCore::closeUnits() {
    // Empty the PortCore#units list. This is only possible when
    // the server thread is finished.
    stateMutex.wait();
    yAssert(finished==true);
    stateMutex.post();

    // In the "finished" phase, nobody else touches the units,
    // so we can go ahead and shut them down and delete them.
    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit!=YARP_NULLPTR) {
            YARP_DEBUG(log, "closing a unit");
            unit->close();
            YARP_DEBUG(log, "joining a unit");
            unit->join();
            delete unit;
            YARP_DEBUG(log, "deleting a unit");
            units[i] = YARP_NULLPTR;
        }
    }

    // Get rid of all our nulls.  Done!
    units.clear();
}

void PortCore::reapUnits() {
    // Connections that should be shut down get tagged as "doomed"
    // but aren't otherwise touched until it is safe to do so.
    stateMutex.wait();
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                if (unit->isDoomed()&&!unit->isFinished()) {
                    ConstString s = unit->getRoute().toString();
                    YARP_DEBUG(log, ConstString("Informing connection ") +
                               s + " that it is doomed");
                    unit->close();
                    YARP_DEBUG(log, ConstString("Closed connection ") +
                               s);
                    unit->join();
                    YARP_DEBUG(log, ConstString("Joined thread of connection ") +
                               s);
                }
            }
        }
    }
    stateMutex.post();
    cleanUnits();
}

void PortCore::cleanUnits(bool blocking) {
    // We will remove any connections that have finished operating from
    // the PortCore#units list.

    // Depending on urgency, either wait for a safe time to do this
    // or skip if unsafe.
    if (blocking) {
        stateMutex.wait();
    } else {
        blocking = stateMutex.check();
        if (!blocking) return;
    }

    // We will update our connection counts as a by-product.
    int updatedInputCount = 0;
    int updatedOutputCount = 0;
    int updatedDataOutputCount = 0;
    YARP_DEBUG(log, "/ routine check of connections to this port begins");
    if (!finished) {

        // First, we delete and null out any defunct entries in the list.
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                YARP_DEBUG(log, ConstString("| checking connection ") + unit->getRoute().toString() + " " + unit->getMode());
                if (unit->isFinished()) {
                    ConstString con = unit->getRoute().toString();
                    YARP_DEBUG(log, ConstString("|   removing connection ") + con);
                    unit->close();
                    unit->join();
                    delete unit;
                    units[i] = YARP_NULLPTR;
                    YARP_DEBUG(log, ConstString("|   removed connection ") + con);
                } else {
                    // No work to do except updating connection counts.
                    if (!unit->isDoomed()) {
                        if (unit->isOutput()) {
                            updatedOutputCount++;
                            if (unit->getMode()=="") {
                                updatedDataOutputCount++;
                            }
                        }
                        if (unit->isInput()) {
                            if (unit->getRoute().getFromName()!="admin") {
                                updatedInputCount++;
                            }
                        }
                    }
                }
            }
        }

        // Now we do some awkward shuffling (list class may be from ACE
        // or STL, if ACE it is quite limited).  We move the nulls to
        // the end of the list ...
        unsigned int rem = 0;
        for (unsigned int i2=0; i2<units.size(); i2++) {
            if (units[i2]!=YARP_NULLPTR) {
                if (rem<i2) {
                    units[rem] = units[i2];
                    units[i2] = YARP_NULLPTR;
                }
                rem++;
            }
        }

        // ... Now we get rid of the null entries
        for (unsigned int i3=0; i3<units.size()-rem; i3++) {
            units.pop_back();
        }
    }

    // Finalize the connection counts.
    dataOutputCount = updatedDataOutputCount;
    stateMutex.post();
    packetMutex.wait();
    inputCount = updatedInputCount;
    outputCount = updatedOutputCount;
    packetMutex.post();
    YARP_DEBUG(log, "\\ routine check of connections to this port ends");
}


void PortCore::addInput(InputProtocol *ip) {
    // This method is only called by the server thread in its running phase.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will have its own thread associated
    // with it.

    yAssert(ip!=YARP_NULLPTR);
    stateMutex.wait();
    PortCoreUnit *unit = new PortCoreInputUnit(*this,
                                               getNextIndex(),
                                               ip,
                                               false);
    yAssert(unit!=YARP_NULLPTR);
    unit->start();
    units.push_back(unit);
    YMSG(("there are now %d units\n", (int)units.size()));
    stateMutex.post();
}


void PortCore::addOutput(OutputProtocol *op) {
    // This method is called from threads associated with input
    // connections.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will start with its own thread
    // associated with it, but that thread will be very short-lived
    // if the port is not configured to do background writes.

    yAssert(op!=YARP_NULLPTR);
    stateMutex.wait();
    if (!finished) {
        PortCoreUnit *unit = new PortCoreOutputUnit(*this, getNextIndex(), op);
        yAssert(unit!=YARP_NULLPTR);
        unit->start();
        units.push_back(unit);
    }
    stateMutex.post();
}


bool PortCore::isUnit(const Route& route, int index) {
    // Check if a connection with a specified route (and optional ID) is present
    bool needReap = false;
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                Route alt = unit->getRoute();
                ConstString wild = "*";
                bool ok = true;
                if (index>=0) {
                    ok = ok && (unit->getIndex()==index);
                }
                if (ok) {
                    if (route.getFromName()!=wild) {
                        ok = ok && (route.getFromName()==alt.getFromName());
                    }
                    if (route.getToName()!=wild) {
                        ok = ok && (route.getToName()==alt.getToName());
                    }
                    if (route.getCarrierName()!=wild) {
                        ok = ok && (route.getCarrierName()==alt.getCarrierName());
                    }
                }
                if (ok) {
                    needReap = true;
                    break;
                }
            }
        }
    }
    return needReap;
}


bool PortCore::removeUnit(const Route& route, bool synch, bool *except) {
    // This is a request to remove a connection.  It could arise from any
    // input thread.

    if (except!=YARP_NULLPTR) {
        YARP_DEBUG(log, ConstString("asked to remove connection in the way of ") + route.toString());
        *except = false;
    } else {
        YARP_DEBUG(log, ConstString("asked to remove connection ") + route.toString());
    }

    // Scan for units that match the given route, and collect their IDs.
    // Mark them as "doomed".
    std::vector<int> removals;
    stateMutex.wait();
    bool needReap = false;
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                Route alt = unit->getRoute();
                ConstString wild = "*";
                bool ok = true;
                if (route.getFromName()!=wild) {
                    ok = ok && (route.getFromName()==alt.getFromName());
                }
                if (route.getToName()!=wild) {
                    ok = ok && (route.getToName()==alt.getToName());
                }
                if (route.getCarrierName()!=wild) {
                    if (except==YARP_NULLPTR) {
                        ok = ok && (route.getCarrierName()==alt.getCarrierName());
                    } else {
                        if (route.getCarrierName()==alt.getCarrierName()) {
                            *except = true;
                            ok = false;
                        }
                    }
                }

                if (ok) {
                    YARP_DEBUG(log,
                               ConstString("removing connection ") + alt.toString());
                    removals.push_back(unit->getIndex());
                    unit->setDoomed();
                    needReap = true;
                }
            }
        }
    }
    stateMutex.post();

    // If we find one or more matches, we need to do some work.
    // We've marked those matches as "doomed" so they'll get cleared
    // up eventually, but we can speed this up by waking up the
    // server thread.
    if (needReap) {
        YARP_DEBUG(log, "one or more connections need prodding to die");

        if (manual) {
            // No server thread, no problems.
            reapUnits();
        } else {
            // Send a blank message to make up server thread.
            OutputProtocol *op = face->write(address);
            if (op!=YARP_NULLPTR) {
                op->close();
                delete op;
            }
            YARP_DEBUG(log, "sent message to prod connection death");

            if (synch) {
                // Wait for connections to be cleaned up.
                YARP_DEBUG(log, "synchronizing with connection death");
                bool cont = false;
                do {
                    stateMutex.wait();
                    for (int i=0; i<(int)removals.size(); i++) {
                        cont = isUnit(route, removals[i]);
                        if (cont) break;
                    }
                    if (cont) {
                        connectionListeners++;
                    }
                    stateMutex.post();
                    if (cont) {
                        connectionChange.wait();
                    }
                } while (cont);
            }
        }
    }
    return needReap;
}


bool PortCore::addOutput(const ConstString& dest, void *id, OutputStream *os,
                         bool onlyIfNeeded) {
    YARP_DEBUG(log, ConstString("asked to add output to ")+dest);

    // Buffer to store text describing outcome (successful connection,
    // or a failure).
    BufferedConnectionWriter bw(true);

    // Look up the address we'll be connectioning to.
    Contact parts = Name(dest).toAddress();
    Contact contact = NetworkBase::queryName(parts.getRegName().c_str());
    Contact address = contact;

    // If we can't find it, say so and abort.
    if (!address.isValid()) {
        bw.appendLine(ConstString("Do not know how to connect to ") + dest);
        if (os!=YARP_NULLPTR) bw.write(*os);
        return false;
    }

    // We clean all existing connections to the desired destination,
    // optionally stopping if we find one with the right carrier.
    if (onlyIfNeeded) {
        // Remove any existing connections between source and destination
        // with a different carrier.  If we find a connection already
        // present with the correct carrier, then we are done.
        bool except = false;
        removeUnit(Route(getName(), address.getRegName(),
                         address.getCarrier()), true, &except);
        if (except) {
            // Connection already present.
            YARP_DEBUG(log, ConstString("output already present to ")+
                       dest);
            bw.appendLine(ConstString("Desired connection already present from ") + getName() + " to " + dest);
            if (os!=YARP_NULLPTR) bw.write(*os);
            return true;
        }
    } else {
        // Remove any existing connections between source and destination.
        removeUnit(Route(getName(), address.getRegName(), "*"), true);
    }

    // Set up a named route for this connection.
    ConstString aname = address.getRegName();
    if (aname=="") {
        aname = address.toURI(false);
    }
    Route r(getName(),
            aname,
            ((parts.getCarrier()!="") ? parts.getCarrier() : address.getCarrier()));
    r.setToContact(contact);

    // Check for any restrictions on the port.  Perhaps it can only
    // read, or write.
    bool allowed = true;
    ConstString err = "";
    ConstString append = "";
    int f = getFlags();
    bool allow_output = (f&PORTCORE_IS_OUTPUT);
    bool rpc = (f&PORTCORE_IS_RPC);
    Name name(r.getCarrierName() + ConstString("://test"));
    ConstString mode = name.getCarrierModifier("log");
    bool is_log = (mode!="");
    if (is_log) {
        if (mode!="in") {
            err = "Logger configured as log." + mode + ", but only log.in is supported";
            allowed = false;
        } else {
            append = "; " + r.getFromName() + " will forward messages and replies (if any) to " + r.getToName();
        }
    }
    if (!allow_output) {
        if (!is_log) {
            bool push = false;
            Carrier *c = Carriers::getCarrierTemplate(r.getCarrierName());
            if (c) {
                push = c->isPush();
            }
            if (push) {
                err = "Outputs not allowed";
                allowed = false;
            }
        }
    } else if (rpc) {
        if (dataOutputCount>=1 && !is_log) {
            err = "RPC output already connected";
            allowed = false;
        }
    }

    // If we found a relevant restriction, abort.
    if (!allowed) {
        bw.appendLine(err);
        if (os!=YARP_NULLPTR) bw.write(*os);
        return false;
    }

    // Ok! We can go ahead and make a connection.
    OutputProtocol *op = YARP_NULLPTR;
    if (timeout>0) {
        address.setTimeout(timeout);
    }
    op = Carriers::connect(address);
    if (op!=YARP_NULLPTR) {
        op->attachPort(contactable);
        if (timeout>0) {
            op->setTimeout(timeout);
        }

        bool ok = op->open(r);
        if (!ok) {
            YARP_DEBUG(log, "open route error");
            delete op;
            op = YARP_NULLPTR;
        }
    }

    // No connection, abort.
    if (op==YARP_NULLPTR) {
        bw.appendLine(ConstString("Cannot connect to ") + dest);
        if (os!=YARP_NULLPTR) bw.write(*os);
        return false;
    }

    // Ok, we have a connection, now add it to PortCore#units
    if (op->getConnection().isPush()) {
        // This is the normal case
        addOutput(op);
    } else {
        // This is the case for connections that are initiated
        // in the opposite direction to native YARP connections.
        // Native YARP has push connections, initiated by the
        // sender.  HTTP and ROS have pull connections, initiated
        // by the receiver.
        // We invert the route, flip the protocol direction, and add.
        r.swapNames();
        op->rename(r);
        InputProtocol *ip =  &(op->getInput());
        stateMutex.wait();
        if (!finished) {
            PortCoreUnit *unit = new PortCoreInputUnit(*this,
                                                       getNextIndex(),
                                                       ip,
                                                       true);
            yAssert(unit!=YARP_NULLPTR);
            unit->start();
            units.push_back(unit);
        }
        stateMutex.post();
    }

    // Communicated the good news.
    bw.appendLine(ConstString("Added connection from ") + getName() + " to " + dest + append);
    if (os!=YARP_NULLPTR) bw.write(*os);
    cleanUnits();
    return true;
}


void PortCore::removeOutput(const ConstString& dest, void *id, OutputStream *os) {
    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route("*", dest, "*"), true)) {
        bw.appendLine(ConstString("Removed connection from ") + getName() +
                      " to " + dest);
    } else {
        bw.appendLine(ConstString("Could not find an outgoing connection to ") +
                      dest);
    }
    if (os!=YARP_NULLPTR) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::removeInput(const ConstString& dest, void *id, OutputStream *os) {
    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route(dest, "*", "*"), true)) {
        bw.appendLine(ConstString("Removing connection from ") + dest + " to " +
                      getName());
    } else {
        bw.appendLine(ConstString("Could not find an incoming connection from ") +
                      dest);
    }
    if (os!=YARP_NULLPTR) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::describe(void *id, OutputStream *os) {
    cleanUnits();

    // Buffer to store a human-readable description of the port's
    // state.
    BufferedConnectionWriter bw(true);

    stateMutex.wait();

    // Report name and address.
    bw.appendLine(ConstString("This is ") + address.getRegName() + " at " +
                  address.toURI());

    // Report outgoing connections.
    int oct = 0;
    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit!=YARP_NULLPTR) {
            if (unit->isOutput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                ConstString msg = "There is an output connection from " +
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

    // Report incoming connections.
    int ict = 0;
    for (unsigned int i2=0; i2<units.size(); i2++) {
        PortCoreUnit *unit = units[i2];
        if (unit!=YARP_NULLPTR) {
            if (unit->isInput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                if (route.getCarrierName()!="") {
                    ConstString msg = "There is an input connection from " +
                        route.getFromName() +
                        " to " + route.getToName() + " using " +
                        route.getCarrierName();
                    bw.appendLine(msg);
                    ict++;
                }
            }
        }
    }
    if (ict<1) {
        bw.appendLine("There are no incoming connections");
    }

    stateMutex.post();

    // Send description across network, or print it.
    if (os!=YARP_NULLPTR) {
        bw.write(*os);
    } else {
        StringOutputStream sos;
        bw.write(sos);
        printf("%s\n", sos.toString().c_str());
    }
}


void PortCore::describe(PortReport& reporter) {
    cleanUnits();

    stateMutex.wait();

    // Report name and address of port.
    PortInfo baseInfo;
    baseInfo.tag = yarp::os::PortInfo::PORTINFO_MISC;
    ConstString portName = address.getRegName().c_str();
    baseInfo.message = (ConstString("This is ") + portName.c_str() + " at " +
                        address.toURI()).c_str();
    reporter.report(baseInfo);

    // Report outgoing connections.
    int oct = 0;
    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit!=YARP_NULLPTR) {
            if (unit->isOutput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                ConstString msg = "There is an output connection from " +
                    route.getFromName() +
                    " to " + route.getToName() + " using " +
                    route.getCarrierName();
                PortInfo info;
                info.message = msg.c_str();
                info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
                info.incoming = false;
                info.portName = portName;
                info.sourceName = route.getFromName().c_str();
                info.targetName = route.getToName().c_str();
                info.carrierName = route.getCarrierName().c_str();
                reporter.report(info);
                oct++;
            }
        }
    }
    if (oct<1) {
        PortInfo info;
        info.tag = yarp::os::PortInfo::PORTINFO_MISC;
        info.message = "There are no outgoing connections";
        reporter.report(info);
    }

    // Report incoming connections.
    int ict = 0;
    for (unsigned int i2=0; i2<units.size(); i2++) {
        PortCoreUnit *unit = units[i2];
        if (unit!=YARP_NULLPTR) {
            if (unit->isInput()&&!unit->isFinished()) {
                Route route = unit->getRoute();
                ConstString msg = "There is an input connection from " +
                    route.getFromName() +
                    " to " + route.getToName() + " using " +
                    route.getCarrierName();
                PortInfo info;
                info.message = msg.c_str();
                info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
                info.incoming = true;
                info.portName = portName;
                info.sourceName = route.getFromName().c_str();
                info.targetName = route.getToName().c_str();
                info.carrierName = route.getCarrierName().c_str();
                reporter.report(info);
                ict++;
            }
        }
    }
    if (ict<1) {
        PortInfo info;
        info.tag = yarp::os::PortInfo::PORTINFO_MISC;
        info.message = "There are no incoming connections";
        reporter.report(info);
    }

    stateMutex.post();
}


void PortCore::setReportCallback(yarp::os::PortReport *reporter) {
   stateMutex.wait();
   if (reporter!=YARP_NULLPTR) {
       eventReporter = reporter;
   }
   stateMutex.post();
}

void PortCore::resetReportCallback() {
    stateMutex.wait();
    eventReporter = YARP_NULLPTR;
    stateMutex.post();
}

void PortCore::report(const PortInfo& info) {
    // We are in the context of one of the input or output threads,
    // so our contact with the PortCore must be absolutely minimal.
    //
    // It is safe to pick up the address of the reporter if this is
    // kept constant over the lifetime of the input/output threads.

    if (eventReporter!=YARP_NULLPTR) {
        eventReporter->report(info);
    }
}




bool PortCore::readBlock(ConnectionReader& reader, void *id, OutputStream *os) {
    bool result = true;

    // We are in the context of one of the input threads,
    // so our contact with the PortCore must be absolutely minimal.
    //
    // It is safe to pick up the address of the reader since this is
    // constant over the lifetime of the input threads.

    if (this->reader!=YARP_NULLPTR && !interrupted) {
        interruptible = false; // No mutexing; user of interrupt() has to be
                               // careful.

        bool haveOutputs = (outputCount!=0); // No mutexing, but failure
                                             // modes are benign.

        if (logNeeded&&haveOutputs) {
            // Normally, yarp doesn't pay attention to the content of
            // messages received by the client.  Likewise, the content
            // of replies are not monitored.  However it may sometimes
            // be useful to log this traffic.

            ConnectionRecorder recorder;
            recorder.init(&reader);
            lockCallback();
            result = this->reader->read(recorder);
            unlockCallback();
            recorder.fini();
            // send off a log of this transaction to whoever wants it
            sendHelper(recorder, PORTCORE_SEND_LOG);
        } else {
            // YARP is not needed as a middleman
            lockCallback();
            result = this->reader->read(reader);
            unlockCallback();
        }

        interruptible = true;
    } else {
        // Read and ignore message, there is no where to send it.
        YARP_DEBUG(Logger::get(), "data received in PortCore, no reader for it");
        Bottle b;
        result = b.read(reader);
    }
    return result;
}


bool PortCore::send(PortWriter& writer, PortReader *reader,
                    PortWriter *callback) {
    // check if there is any modifier
    // we need to protect this part while the modifier
    // plugin is loading or unloading!
    modifier.outputMutex.lock();
    if (modifier.outputModifier) {
        if (!modifier.outputModifier->acceptOutgoingData(writer)) {
            modifier.outputMutex.unlock();
            return false;
        }
        modifier.outputModifier->modifyOutgoingData(writer);
    }
    modifier.outputMutex.unlock();
    if (!logNeeded) {
        return sendHelper(writer, PORTCORE_SEND_NORMAL, reader, callback);
    }
    // logging is desired, so we need to wrap up and log this send
    // (and any reply it gets)  -- TODO not yet supported
    return sendHelper(writer, PORTCORE_SEND_NORMAL, reader, callback);
}

bool PortCore::sendHelper(PortWriter& writer,
                          int mode, PortReader *reader, PortWriter *callback) {
    if (interrupted||finishing) return false;

    bool all_ok = true;
    bool gotReply = false;
    int logCount = 0;
    ConstString envelopeString = envelope;

    // Pass a message to all output units for sending on.  We could
    // be doing more here to cache the serialization of the message
    // and reuse it across output connections.  However, one key
    // optimization is present: external blocks written by
    // yarp::os::ConnectionWriter::appendExternalBlock are never
    // copied.  So for example the core image array in a yarp::sig::Image
    // is untouched by the port communications code.

    YMSG(("------- send in real\n"));

    // Give user the chance to know that this object is about to be
    // written.
    writer.onCommencement();

    // All user-facing parts of this port will be blocked on this
    // operation, so we'll want to be snappy. How long the
    // operation lasts will depend on these flags:
    //   * waitAfterSend
    //   * waitBeforeSend
    // set by setWaitAfterSend() and setWaitBeforeSend().
    stateMutex.wait();

    // If the port is shutting down, abort.
    if (finished) {
        stateMutex.post();
        return false;
    }

    YMSG(("------- send in\n"));
    // Prepare a "packet" for tracking a single message which
    // may travel by multiple outputs.
    packetMutex.wait();
    PortCorePacket *packet = packets.getFreePacket();
    yAssert(packet!=YARP_NULLPTR);
    packet->setContent(&writer, false, callback);
    packetMutex.post();

    // Scan connections, placing message everyhere we can.
    for (unsigned int i=0; i<units.size(); i++) {
        PortCoreUnit *unit = units[i];
        if (unit==YARP_NULLPTR) continue;
        if (unit->isOutput() && !unit->isFinished()) {
            bool log = (unit->getMode()!="");
            if (log) {
                // Some connections are for logging only.
                logCount++;
            }
            bool ok = (mode==PORTCORE_SEND_NORMAL)?(!log):(log);
            if (!ok) continue;
            bool waiter = waitAfterSend||(mode==PORTCORE_SEND_LOG);
            YMSG(("------- -- inc\n"));
            packetMutex.wait();
            packet->inc();  // One more connection carrying message.
            packetMutex.post();
            YMSG(("------- -- presend\n"));
            bool gotReplyOne = false;
            // Send the message off on this connection.
            void *out = unit->send(writer,
                                   reader,
                                   (callback!=YARP_NULLPTR)?callback:(&writer),
                                   (void *)packet,
                                   envelopeString,
                                   waiter, waitBeforeSend,
                                   &gotReplyOne);
            gotReply = gotReply||gotReplyOne;
            YMSG(("------- -- send\n"));
            if (out != YARP_NULLPTR) {
                // We got back a report of a message already sent.
                packetMutex.wait();
                ((PortCorePacket *)out)->dec();  // Message on one
                                                 // fewer connections.
                packets.checkPacket((PortCorePacket *)out);
                packetMutex.post();
            }
            if (waiter) {
                if (unit->isFinished()) {
                    all_ok = false;
                }
            }
            YMSG(("------- -- dec\n"));
        }
    }
    YMSG(("------- pack check\n"));
    packetMutex.wait();
    packet->dec();  // We no longer concern ourselves with the message.
                    // It may or may not be traveling on some connections.
                    // But that is not our problem anymore.
    packets.checkPacket(packet);
    packetMutex.post();
    YMSG(("------- packed\n"));
    YMSG(("------- send out\n"));
    if (mode==PORTCORE_SEND_LOG) {
        if (logCount==0) {
            logNeeded = false;
        }
    }
    stateMutex.post();
    YMSG(("------- send out real\n"));

    if (waitAfterSend) {
        if (reader) {
            all_ok = all_ok && gotReply;
        }
    }

    return all_ok;
}



bool PortCore::isWriting() {
    bool writing = false;

    stateMutex.wait();

    // Check if any port is currently writing.  TODO optimize
    // this query by counting down with notifyCompletion().
    if (!finished) {
        for (unsigned int i=0; i<units.size(); i++) {
            PortCoreUnit *unit = units[i];
            if (unit!=YARP_NULLPTR) {
                if (!unit->isFinished()) {
                    if (unit->isBusy()) {
                        writing = true;
                    }
                }
            }
        }
    }

    stateMutex.post();

    return writing;
}


int PortCore::getInputCount() {
    cleanUnits(false);
    packetMutex.wait();
    int result = inputCount;
    packetMutex.post();
    return result;
}

int PortCore::getOutputCount() {
    cleanUnits(false);
    packetMutex.wait();
    int result = outputCount;
    packetMutex.post();
    return result;
}



void PortCore::notifyCompletion(void *tracker) {
    YMSG(("starting notifyCompletion\n"));
    packetMutex.wait();
    if (tracker!=YARP_NULLPTR) {
        ((PortCorePacket *)tracker)->dec();
        packets.checkPacket((PortCorePacket *)tracker);
    }
    packetMutex.post();
    YMSG(("stopping notifyCompletion\n"));
}


bool PortCore::setEnvelope(PortWriter& envelope) {
    envelopeWriter.restart();
    bool ok = envelope.write(envelopeWriter);
    if (ok) {
        setEnvelope(envelopeWriter.toString());
    }
    return ok;
}


void PortCore::setEnvelope(const ConstString& envelope) {
    this->envelope = envelope;
    for (unsigned int i=0; i<this->envelope.length(); i++) {
        // It looks like envelopes are constrained to be printable ASCII?
        // I'm not sure why this would be.  TODO check.
        if (this->envelope[i]<32) {
            this->envelope = this->envelope.substr(0, i);
            break;
        }
    }
    YARP_DEBUG(log, ConstString("set envelope to ") + this->envelope);
}

ConstString PortCore::getEnvelope() {
    return envelope;
}

bool PortCore::getEnvelope(PortReader& envelope) {
    StringInputStream sis;
    sis.add(this->envelope.c_str());
    sis.add("\r\n");
    StreamConnectionReader sbr;
    Route route;
    sbr.reset(sis, YARP_NULLPTR, route, 0, true);
    return envelope.read(sbr);
}

// Shorthand to create a nested (tag, val) pair to add to a message.
#define STANZA(name, tag, val) Bottle name; name.addString(tag); name.addString(val.c_str());
#define STANZA_INT(name, tag, val) Bottle name; name.addString(tag); name.addInt(val);

// Make an RPC connection to talk to a ROS API, send a message, get reply.
// NOTE: ROS support can now be moved out of here, once all documentation
// of older ways to interoperate with it are purged and people stop
// doing it.
static bool __pc_rpc(const Contact& c,
                     const char *carrier,
                     Bottle& writer,
                     Bottle& reader,
                     bool verbose) {
    ContactStyle style;
    style.quiet = !verbose;
    style.timeout = 4;
    style.carrier = carrier;
    bool ok  = Network::write(c, writer, reader, style);
    return ok;
}

// ACE is sometimes confused by localhost aliases, in a ROS-incompatible
// way.  This method does a quick sanity check if we are using ROS.
static bool __tcp_check(const Contact& c) {
#ifdef YARP_HAS_ACE
    ACE_INET_Addr addr;
    int result = addr.set(c.getPort(), c.getHost().c_str());
    if (result!=0) {
        fprintf(stderr, "ACE choked on %s:%d\n", c.getHost().c_str(),
                c.getPort());
    }
    result = addr.set(c.getPort(), "127.0.0.1");
    if (result!=0) {
        fprintf(stderr, "ACE choked on 127.0.0.1:%d\n",
                c.getPort());
    }
    result = addr.set(c.getPort(), "127.0.1.1");
    if (result!=0) {
        fprintf(stderr, "ACE choked on 127.0.1.1:%d\n",
                c.getPort());
    }
#endif
    return true;
}

bool PortCore::adminBlock(ConnectionReader& reader, void *id,
                          OutputStream *os) {
    Bottle cmd, result;

    // We've received a message to the port that is marked as administrative.
    // That means that instead of passing it along as data to the user of the
    // port, the port itself is responsible for reading and responding to
    // it.  So let's read the message and see what we're supposed to do.
    cmd.read(reader);

    YARP_SPRINTF2(log, debug,
                  "Port %s received command %s", getName().c_str(),
                                                 cmd.toString().c_str());

    StringOutputStream cache;

    int vocab = cmd.get(0).asVocab();

    // We support ROS client API these days.  Here we recode some long ROS
    // command names, just for convenience.
    if (cmd.get(0).asString()=="publisherUpdate") {
        vocab = VOCAB4('r', 'p', 'u', 'p');
    }
    if (cmd.get(0).asString()=="requestTopic") {
        vocab = VOCAB4('r', 't', 'o', 'p');
    }
    if (cmd.get(0).asString()=="getPid") {
        vocab = VOCAB3('p', 'i', 'd');
    }
    if (cmd.get(0).asString()=="getBusInfo") {
        vocab = VOCAB3('b', 'u', 's');
    }

    ConstString infoMsg;
    switch (vocab) {
    case VOCAB4('h', 'e', 'l', 'p'):
        // We give a list of the most useful administrative commands.
        result.addVocab(VOCAB4('m', 'a', 'n', 'y'));
        result.addString("[help]                  # give this help");
        result.addString("[ver]                   # report protocol version information");
        result.addString("[add] $portname         # add an output connection");
        result.addString("[add] $portname $car    # add an output with a given protocol");
        result.addString("[del] $portname         # remove an input or output connection");
        result.addString("[list] [in]             # list input connections");
        result.addString("[list] [out]            # list output connections");
        result.addString("[list] [in]  $portname  # give details for input");
        result.addString("[list] [out] $portname  # give details for output");
        result.addString("[prop] [get]            # get all user-defined port properties");
        result.addString("[prop] [get] $prop      # get a user-defined port property (prop, val)");
        result.addString("[prop] [set] $prop $val # set a user-defined port property (prop, val)");
        result.addString("[prop] [get] $portname  # get Qos properties of a connection to/from a port");
        result.addString("[prop] [set] $portname  # set Qos properties of a connection to/from a port");
        result.addString("[prop] [get] $cur_port  # get information about current process (e.g., scheduling priority, pid)");
        result.addString("[prop] [set] $cur_port  # set properties of the current process (e.g., scheduling priority, pid)");
        result.addString("[atch] [out] $prop      # attach a portmonitor plug-in to the port's output");
        result.addString("[atch] [in]  $prop      # attach a portmonitor plug-in to the port's input");
        result.addString("[dtch] [out]            # detach portmonitor plug-in from the port's output");
        result.addString("[dtch] [in]             # detach portmonitor plug-in from the port's input");
        //result.addString("[atch] $portname $prop  # attach a portmonitor plug-in to the connection to/from $portname");
        //result.addString("[dtch] $portname        # detach any portmonitor plug-in from the connection to/from $portname");
        break;
    case VOCAB3('v', 'e', 'r'):
        // Gives a version number for the administrative commands.
        // It is distinct from YARP library versioning.
        result.addVocab(Vocab::encode("ver"));
        result.addInt(1);
        result.addInt(2);
        result.addInt(3);
        break;
    case VOCAB3('a', 'd', 'd'):
        {
            // Add an output to the port.
            ConstString output = cmd.get(1).asString().c_str();
            ConstString carrier = cmd.get(2).asString().c_str();
            if (carrier!="") {
                output = carrier + ":/" + output;
            }
            addOutput(output, id, &cache, false);
            ConstString r = cache.toString();
            int v = (r[0]=='A')?0:-1;
            result.addInt(v);
            result.addString(r.c_str());
        }
        break;
    case VOCAB4('a', 't', 'c', 'h'):
        {
            switch (cmd.get(1).asVocab()) {
            case VOCAB3('o', 'u', 't'): {
                ConstString propString = cmd.get(2).asString().c_str();
                Property prop(propString.c_str());
                ConstString errMsg;
                if (!attachPortMonitor(prop, true, errMsg)) {
                    result.clear();
                    result.addVocab(Vocab::encode("fail"));
                    result.addString(errMsg.c_str());
                }
                else {
                    result.clear();
                    result.addVocab(Vocab::encode("ok"));
                }
            }
            break;
            case VOCAB2('i', 'n'): {
                ConstString propString = cmd.get(2).asString().c_str();
                Property prop(propString.c_str());
                ConstString errMsg;
                if (!attachPortMonitor(prop, false, errMsg)) {
                    result.clear();
                    result.addVocab(Vocab::encode("fail"));
                    result.addString(errMsg.c_str());
                }
                else {
                    result.clear();
                    result.addVocab(Vocab::encode("ok"));
                }
            }
            break;
            default:
                result.clear();
                result.addVocab(Vocab::encode("fail"));
                result.addString("attach command must be followd by [out] or [in]");
            };
        }
        break;
    case VOCAB4('d', 't', 'c', 'h'):
        {
            switch (cmd.get(1).asVocab()) {
            case VOCAB3('o', 'u', 't'): {
                if (dettachPortMonitor(true))
                    result.addVocab(Vocab::encode("ok"));
                else
                    result.addVocab(Vocab::encode("fail"));
            }
            break;
            case VOCAB2('i', 'n'): {
                if (dettachPortMonitor(false))
                    result.addVocab(Vocab::encode("ok"));
                else
                    result.addVocab(Vocab::encode("fail"));
            }
            break;
            default:
                result.clear();
                result.addVocab(Vocab::encode("fail"));
                result.addString("dettach command must be followd by [out] or [in]");
            };
        }
        break;
    case VOCAB3('d', 'e', 'l'):
        {
            // Delete any inputs or outputs involving the named port.
            removeOutput(ConstString(cmd.get(1).asString().c_str()), id, &cache);
            ConstString r1 = cache.toString();
            cache.reset();
            removeInput(ConstString(cmd.get(1).asString().c_str()), id, &cache);
            ConstString r2 = cache.toString();
            int v = (r1[0]=='R'||r2[0]=='R')?0:-1;
            result.addInt(v);
            if (r1[0]=='R' && r2[0]!='R') {
                result.addString(r1.c_str());
            } else if (r1[0]!='R' && r2[0]=='R') {
                result.addString(r2.c_str());
            } else {
                result.addString((r1 + r2).c_str());
            }
        }
        break;
    case VOCAB4('l', 'i', 's', 't'):
        switch (cmd.get(1).asVocab()) {
        case VOCAB2('i', 'n'):
            {
                // Return a list of all input connections.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                for (unsigned int i2=0; i2<units.size(); i2++) {
                    PortCoreUnit *unit = units[i2];
                    if (unit!=YARP_NULLPTR) {
                        if (unit->isInput()&&!unit->isFinished()) {
                            Route route = unit->getRoute();
                            if (target=="") {
                                const ConstString& name = route.getFromName();
                                if (name!="") {
                                    result.addString(name.c_str());
                                }
                            } else if (route.getFromName()==target.c_str()) {
                                STANZA(bfrom, "from", route.getFromName());
                                STANZA(bto, "to", route.getToName());
                                STANZA(bcarrier, "carrier",
                                       route.getCarrierName());
                                result.addList() = bfrom;
                                result.addList() = bto;
                                result.addList() = bcarrier;
                                Carrier *carrier = Carriers::chooseCarrier(route.getCarrierName());
                                if (carrier->isConnectionless()) {
                                    STANZA_INT(bconnectionless, "connectionless", 1);
                                    result.addList() = bconnectionless;
                                }
                                if (!carrier->isPush()) {
                                    STANZA_INT(breverse, "push", 0);
                                    result.addList() = breverse;
                                }
                                delete carrier;
                            }
                        }
                    }
                }
                stateMutex.post();
            }
            break;
        case VOCAB3('o', 'u', 't'):
        default:
            {
                // Return a list of all output connections.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                for (unsigned int i=0; i<units.size(); i++) {
                    PortCoreUnit *unit = units[i];
                    if (unit!=YARP_NULLPTR) {
                        if (unit->isOutput()&&!unit->isFinished()) {
                            Route route = unit->getRoute();
                            if (target=="") {
                                result.addString(route.getToName().c_str());
                            } else if (route.getToName()==target.c_str()) {
                                STANZA(bfrom, "from", route.getFromName());
                                STANZA(bto, "to", route.getToName());
                                STANZA(bcarrier, "carrier",
                                       route.getCarrierName());
                                result.addList() = bfrom;
                                result.addList() = bto;
                                result.addList() = bcarrier;
                                Carrier *carrier = Carriers::chooseCarrier(route.getCarrierName());
                                if (carrier->isConnectionless()) {
                                    STANZA_INT(bconnectionless, "connectionless", 1);
                                    result.addList() = bconnectionless;
                                }
                                if (!carrier->isPush()) {
                                    STANZA_INT(breverse, "push", 0);
                                    result.addList() = breverse;
                                }
                                delete carrier;
                            }
                        }
                    }
                }
                stateMutex.post();
            }
        }
        break;

    case VOCAB3('s', 'e', 't'):
        switch (cmd.get(1).asVocab()) {
        case VOCAB2('i', 'n'):
            {
                // Set carrier parameters on a given input connection.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                if (target=="") {
                    result.addInt(-1);
                    result.addString("target port is not specified.\r\n");
                }
                else {
                    if (target == getName()) {
                        yarp::os::Property property;
                        property.fromString(cmd.toString());
                        ConstString errMsg;
                        if (!setParamPortMonitor(property, false, errMsg)) {
                            result.clear();
                            result.addVocab(Vocab::encode("fail"));
                            result.addString(errMsg.c_str());
                        }
                        else {
                            result.clear();
                            result.addVocab(Vocab::encode("ok"));
                        }
                    }
                    else {
                        for (unsigned int i=0; i<units.size(); i++) {
                            PortCoreUnit *unit = units[i];
                            if (unit && unit->isInput() && !unit->isFinished()) {
                                Route route = unit->getRoute();
                                if (route.getFromName() == target.c_str()) {
                                    yarp::os::Property property;
                                    property.fromString(cmd.toString());
                                    unit->setCarrierParams(property);
                                    result.addInt(0);
                                    ConstString msg = "Configured connection from ";
                                    msg += route.getFromName().c_str();
                                    msg += "\r\n";
                                    result.addString(msg.c_str());
                                    break;
                                }
                            }
                        }
                    }
                    if (!result.size())
                    {
                        result.addInt(-1);
                        ConstString msg = "Could not find an incoming connection from ";
                        msg += target.c_str();
                        msg += "\r\n";
                        result.addString(msg.c_str());
                    }
                }
                stateMutex.post();
            }
            break;
        case VOCAB3('o', 'u', 't'):
        default:
            {
                // Set carrier parameters on a given output connection.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                if (target=="") {
                    result.addInt(-1);
                    result.addString("target port is not specified.\r\n");
                }
                else {
                    if (target == getName()) {
                        yarp::os::Property property;
                        property.fromString(cmd.toString());
                        ConstString errMsg;
                        if (!setParamPortMonitor(property, true, errMsg)) {
                            result.clear();
                            result.addVocab(Vocab::encode("fail"));
                            result.addString(errMsg.c_str());
                        }
                        else {
                            result.clear();
                            result.addVocab(Vocab::encode("ok"));
                        }
                    }
                    else {
                        for (unsigned int i=0; i<units.size(); i++) {
                            PortCoreUnit *unit = units[i];
                            if (unit && unit->isOutput() && !unit->isFinished()) {
                                Route route = unit->getRoute();
                                if (route.getToName() == target.c_str()) {
                                    yarp::os::Property property;
                                    property.fromString(cmd.toString());
                                    unit->setCarrierParams(property);
                                    result.addInt(0);
                                    ConstString msg = "Configured connection to ";
                                    msg += route.getFromName().c_str();
                                    msg += "\r\n";
                                    result.addString(msg.c_str());
                                    break;
                                }
                            }
                        }
                    }
                    if (!result.size())
                    {
                        result.addInt(-1);
                        ConstString msg = "Could not find an incoming connection to ";
                        msg += target.c_str();
                        msg += "\r\n";
                        result.addString(msg.c_str());
                    }
                }
                stateMutex.post();

            }
        }
        break;

    case VOCAB3('g', 'e', 't'):
        switch (cmd.get(1).asVocab()) {
        case VOCAB2('i', 'n'):
            {
                // Get carrier parameters for a given input connection.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                if (target=="") {
                    result.addInt(-1);
                    result.addString("target port is not specified.\r\n");
                }
                else {
                    if (target == getName()) {
                        yarp::os::Property property;
                        ConstString errMsg;
                        if (!getParamPortMonitor(property, false, errMsg)) {
                            result.clear();
                            result.addVocab(Vocab::encode("fail"));
                            result.addString(errMsg.c_str());
                        }
                        else {
                            result.clear();
                            result.addDict() = property;
                        }
                    }
                    else {
                        for (unsigned int i=0; i<units.size(); i++) {
                            PortCoreUnit *unit = units[i];
                            if (unit && unit->isInput() && !unit->isFinished()) {
                                Route route = unit->getRoute();
                                if (route.getFromName() == target.c_str()) {
                                    yarp::os::Property property;
                                    unit->getCarrierParams(property);
                                    result.addDict() = property;
                                    break;
                                }
                            }
                        }
                        if (!result.size())
                        {
                            result.addInt(-1);
                            ConstString msg = "Could not find an incoming connection from ";
                            msg += target.c_str();
                            msg += "\r\n";
                            result.addString(msg.c_str());
                        }
                    }
                }
                stateMutex.post();
            }
            break;
        case VOCAB3('o', 'u', 't'):
        default:
            {
                // Get carrier parameters for a given output connection.
                ConstString target = cmd.get(2).asString();
                stateMutex.wait();
                if (target=="") {
                    result.addInt(-1);
                    result.addString("target port is not specified.\r\n");
                }
                else {
                    if (target == getName()) {
                        yarp::os::Property property;
                        ConstString errMsg;
                        if (!getParamPortMonitor(property, true, errMsg)) {
                            result.clear();
                            result.addVocab(Vocab::encode("fail"));
                            result.addString(errMsg.c_str());
                        }
                        else {
                            result.clear();
                            result.addDict() = property;
                        }
                    }
                    else {
                        for (unsigned int i=0; i<units.size(); i++) {
                            PortCoreUnit *unit = units[i];
                            if (unit && unit->isOutput() && !unit->isFinished()) {
                                Route route = unit->getRoute();
                                if (route.getToName() == target.c_str()) {
                                    yarp::os::Property property;
                                    property.fromString(cmd.toString());
                                    unit->getCarrierParams(property);
                                    result.addDict() = property;
                                    break;
                                }
                            }
                        }
                        if (!result.size())
                        {
                            result.addInt(-1);
                            ConstString msg = "Could not find an incoming connection to ";
                            msg += target.c_str();
                            msg += "\r\n";
                            result.addString(msg.c_str());
                        }
                    }
                }
                stateMutex.post();

            }
        }
        break;

    case VOCAB4('r', 'p', 'u', 'p'):
        {
            // When running against a ROS name server, we need to
            // support ROS-style callbacks for connecting publishers
            // with subscribers.  Note: this should not be necessary
            // anymore, now that a dedicated yarp::os::Node class
            // has been implemented, but is still needed for older
            // ways of interfacing with ROS without using dedicated
            // node ports.
            YARP_SPRINTF1(log, debug,
                          "publisherUpdate! --> %s", cmd.toString().c_str());
            ConstString topic =
                RosNameSpace::fromRosName(cmd.get(2).asString());
            Bottle *pubs = cmd.get(3).asList();
            if (pubs!=YARP_NULLPTR) {
                Property listed;
                for (int i=0; i<pubs->size(); i++) {
                    ConstString pub = pubs->get(i).asString();
                    listed.put(pub, 1);
                }
                Property present;
                stateMutex.wait();
                for (unsigned int i=0; i<units.size(); i++) {
                    PortCoreUnit *unit = units[i];
                    if (unit!=YARP_NULLPTR) {
                        if (unit->isPupped()) {
                            ConstString me = unit->getPupString();
                            present.put(me, 1);
                            if (!listed.check(me)) {
                                unit->setDoomed();
                            }
                        }
                    }
                }
                stateMutex.post();
                for (int i=0; i<pubs->size(); i++) {
                    ConstString pub = pubs->get(i).asString();
                    if (!present.check(pub)) {
                        YARP_SPRINTF1(log, debug, "ROS ADD %s", pub.c_str());
                        Bottle req, reply;
                        req.addString("requestTopic");
                        NestedContact nc(getName());
                        req.addString(nc.getNodeName().c_str());
                        req.addString(topic);
                        Bottle& lst = req.addList();
                        Bottle& sublst = lst.addList();
                        sublst.addString("TCPROS");
                        YARP_SPRINTF2(log, debug,
                                      "Sending [%s] to %s", req.toString().c_str(),
                                      pub.c_str());
                        Contact c = Contact::fromString(pub.c_str());
                        if (!__pc_rpc(c, "xmlrpc", req, reply, false)) {
                            fprintf(stderr, "Cannot connect to ROS subscriber %s\n",
                                    pub.c_str());
                            // show diagnosics
                            __pc_rpc(c, "xmlrpc", req, reply, true);
                            __tcp_check(c);
                        } else {
                            Bottle *pref = reply.get(2).asList();
                            ConstString hostname = "";
                            ConstString carrier = "";
                            int portnum = 0;
                            if (reply.get(0).asInt()!=1) {
                                fprintf(stderr, "Failure looking up topic %s: %s\n", topic.c_str(), reply.toString().c_str());
                            } else if (pref==YARP_NULLPTR) {
                                fprintf(stderr, "Failure looking up topic %s: expected list of protocols\n", topic.c_str());
                            } else if (pref->get(0).asString()!="TCPROS") {
                                fprintf(stderr, "Failure looking up topic %s: unsupported protocol %s\n", topic.c_str(),
                                        pref->get(0).asString().c_str());
                            } else {
                                Value hostname2 = pref->get(1);
                                Value portnum2 = pref->get(2);
                                hostname = hostname2.asString().c_str();
                                portnum = portnum2.asInt();
                                carrier = "tcpros+role.pub+topic.";
                                carrier += topic;
                                YARP_SPRINTF3(log, debug,
                                              "topic %s available at %s:%d",
                                              topic.c_str(), hostname.c_str(),
                                              portnum);
                            }
                            if (portnum!=0) {
                                Contact addr(hostname.c_str(), portnum);
                                OutputProtocol *op = YARP_NULLPTR;
                                Route r = Route(getName(),
                                                pub.c_str(),
                                                carrier.c_str());
                                op = Carriers::connect(addr);
                                if (op==YARP_NULLPTR) {
                                    fprintf(stderr, "NO CONNECTION\n");
                                    std::exit(1);
                                } else {
                                    op->attachPort(contactable);
                                    op->open(r);
                                }
                                Route route = op->getRoute();
                                route.swapNames();
                                op->rename(route);
                                InputProtocol *ip =  &(op->getInput());
                                stateMutex.wait();
                                PortCoreUnit *unit = new PortCoreInputUnit(*this,
                                                                           getNextIndex(),
                                                                           ip,
                                                                           true);
                                yAssert(unit!=YARP_NULLPTR);
                                unit->setPupped(pub);
                                unit->start();
                                units.push_back(unit);
                                stateMutex.post();
                            }
                        }
                    }
                }
            }
            result.addInt(1);
            result.addString("ok");
            reader.requestDrop(); // ROS needs us to close down.
        }
        break;
    case VOCAB4('r', 't', 'o', 'p'):
        {
            // ROS-style query for topics.
            YARP_SPRINTF1(log, debug, "requestTopic! --> %s",
                          cmd.toString().c_str());
            result.addInt(1);
            NestedContact nc(getName());
            result.addString(nc.getNodeName().c_str());
            Bottle& lst = result.addList();
            Contact addr = getAddress();
            lst.addString("TCPROS");
            lst.addString(addr.getHost().c_str());
            lst.addInt(addr.getPort());
            reader.requestDrop(); // ROS likes to close down.
        }
        break;
    case VOCAB3('p', 'i', 'd'):
        {
            // ROS-style query for PID.
            result.addInt(1);
            result.addString("");
            result.addInt(getPid());
            reader.requestDrop(); // ROS likes to close down.
        }
        break;
    case VOCAB3('b', 'u', 's'):
        {
            // ROS-style query for bus information - we support this
            // in yarp::os::Node but not otherwise.
            result.addInt(1);
            result.addString("");
            result.addList().addList();
            reader.requestDrop(); // ROS likes to close down.
        }
        break;
    case VOCAB4('p', 'r', 'o', 'p'):
        {
            // Set/get arbitrary properties on a port.
            switch (cmd.get(1).asVocab()) {
            case VOCAB3('g', 'e', 't'):
                {
                    Property *p = acquireProperties(false);
                    if (p) {
                        if (!cmd.get(2).isNull()) {
                            // request: "prop get /portname"
                            ConstString portName = cmd.get(2).asString();
                            bool bFound = false;
                            if ((portName.size() > 0) && (portName[0] == '/')) {
                                // check for their own name
                                if (portName == getName()) {
                                    bFound = true;
                                    result.clear();
                                    Bottle& sched = result.addList();
                                    sched.addString("sched");
                                    Property& sched_prop = sched.addDict();
                                    sched_prop.put("tid", (int)this->getTid());
                                    sched_prop.put("priority", this->getPriority());
                                    sched_prop.put("policy", this->getPolicy());

                                    int pid =  getPid();
                                    SystemInfo::ProcessInfo info = SystemInfo::getProcessInfo(pid);
                                    Bottle& proc = result.addList();
                                    proc.addString("process");
                                    Property& proc_prop = proc.addDict();
                                    proc_prop.put("pid", pid);
                                    proc_prop.put("name", (info.pid !=-1) ? info.name : "unknown");
                                    proc_prop.put("arguments", (info.pid !=-1) ? info.arguments : "unknown");
                                    proc_prop.put("priority", info.schedPriority);
                                    proc_prop.put("policy", info.schedPolicy);

                                    SystemInfo::PlatformInfo pinfo = SystemInfo::getPlatformInfo();
                                    Bottle& platform = result.addList();
                                    platform.addString("platform");
                                    Property& platform_prop = platform.addDict();
                                    platform_prop.put("os", pinfo.name);
                                    platform_prop.put("hostname", address.getHost());

                                    int f = getFlags();
                                    bool is_input = (f & PORTCORE_IS_INPUT);
                                    bool is_output = (f & PORTCORE_IS_OUTPUT);
                                    bool is_rpc = (f & PORTCORE_IS_RPC);
                                    Bottle& port = result.addList();
                                    port.addString("port");
                                    Property& port_prop = port.addDict();
                                    port_prop.put("is_input", is_input);
                                    port_prop.put("is_output", is_output);
                                    port_prop.put("is_rpc", is_rpc);
                                    port_prop.put("type", getType().getName());
                                }
                                else {
                                    for (unsigned int i=0; i<units.size(); i++) {
                                        PortCoreUnit *unit = units[i];
                                        if (unit && !unit->isFinished()) {
                                            Route route = unit->getRoute();
                                            ConstString coreName = (unit->isOutput()) ? route.getToName() : route.getFromName();
                                            if (portName == coreName) {
                                                bFound = true;
                                                int priority = unit->getPriority();
                                                int policy = unit->getPolicy();
                                                int tos = getTypeOfService(unit);
                                                int tid = (int) unit->getTid();
                                                result.clear();
                                                Bottle& sched = result.addList();
                                                sched.addString("sched");
                                                Property& sched_prop = sched.addDict();
                                                sched_prop.put("tid", tid);
                                                sched_prop.put("priority", priority);
                                                sched_prop.put("policy", policy);
                                                Bottle& qos = result.addList();
                                                qos.addString("qos");
                                                Property& qos_prop = qos.addDict();
                                                qos_prop.put("tos", tos);
                                            }
                                        } // end isFinished()
                                    } // end for loop
                                } // end portName == getname()

                                if (!bFound) {  // cannot find any port matchs the requested one
                                    result.clear();
                                    result.addVocab(Vocab::encode("fail"));
                                    ConstString msg = "cannot find any connection to/from ";
                                    msg = msg + portName;
                                    result.addString(msg.c_str());
                                }
                            } // end of (portName[0] == '/')
                            else
                                result.add(p->find(cmd.get(2).asString()));
                        } else {
                            result.fromString(p->toString());
                        }
                    }
                    releaseProperties(p);
                }
                break;
            case VOCAB3('s', 'e', 't'):
                {
                    Property *p = acquireProperties(false);
                    bool bOk = true;
                    if (p) {
                        p->put(cmd.get(2).asString(), cmd.get(3));
                        // setting scheduling properties of all threads within the process
                        // scope through the admin port
                        // e.g. prop set /current_port (process ((priority 30) (policy 1)))
                        Bottle& process = cmd.findGroup("process");
                        if (!process.isNull()) {
                            ConstString portName = cmd.get(2).asString();
                            if ((portName.size() > 0) && (portName[0] == '/')) {
                                // check for their own name
                                if (portName == getName()) {
                                    bOk = false;
                                    Bottle* process_prop = process.find("process").asList();
                                    if (process_prop != YARP_NULLPTR) {
                                        int prio = -1;
                                        int policy = -1;
                                        if (process_prop->check("priority")) {
                                            prio = process_prop->find("priority").asInt();
                                        }
                                        if (process_prop->check("policy")) {
                                            policy = process_prop->find("policy").asInt();
                                        }
                                        bOk = setProcessSchedulingParam(prio, policy);
                                    }
                                }
                            }
                        }
                        // check if we need to set the PortCoreUnit scheduling policy
                        // e.g., "prop set /portname (sched ((priority 30) (policy 1)))"
                        // The priority and policy values on Linux are:
                        // SCHED_OTHER : policy=0, priority=[0 ..  0]
                        // SCHED_FIFO  : policy=1, priority=[1 .. 99]
                        // SCHED_RR    : policy=2, priority=[1 .. 99]
                        Bottle& sched = cmd.findGroup("sched");
                        if (!sched.isNull())
                        {
                            if ((cmd.get(2).asString().size() > 0) && (cmd.get(2).asString()[0] == '/')) {
                                bOk = false;
                                for (unsigned int i=0; i<units.size(); i++) {
                                    PortCoreUnit *unit = units[i];
                                    if (unit && !unit->isFinished()) {
                                        Route route = unit->getRoute();
                                        ConstString portName = (unit->isOutput()) ? route.getToName() : route.getFromName();

                                        if (portName == cmd.get(2).asString()) {
                                            Bottle* sched_prop = sched.find("sched").asList();
                                            if (sched_prop != YARP_NULLPTR) {
                                                int prio = -1;
                                                int policy = -1;
                                                if (sched_prop->check("priority")) {
                                                    prio = sched_prop->find("priority").asInt();
                                                }
                                                if (sched_prop->check("policy")) {
                                                    policy = sched_prop->find("policy").asInt();
                                                }
                                                bOk = (unit->setPriority(prio, policy) != -1);
                                            } else {
                                                bOk = false;
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        // check if we need to set the packet QOS policy
                        // e.g., "prop set /portname (qos ((priority HIGH)))"
                        // e.g., "prop set /portname (qos ((dscp AF12)))"
                        // e.g., "prop set /portname (qos ((tos 12)))"
                        Bottle& qos = cmd.findGroup("qos");
                        if (!qos.isNull())
                        {
                            if ((cmd.get(2).asString().size() > 0) && (cmd.get(2).asString()[0] == '/')) {
                                bOk = false;
                                for (unsigned int i=0; i<units.size(); i++) {
                                    PortCoreUnit *unit = units[i];
                                    if (unit && !unit->isFinished()) {
                                        Route route = unit->getRoute();
                                        ConstString portName = (unit->isOutput()) ? route.getToName() : route.getFromName();
                                        if (portName == cmd.get(2).asString()) {
                                            Bottle* qos_prop = qos.find("qos").asList();
                                            if (qos_prop != YARP_NULLPTR) {
                                                if (qos_prop->check("priority")) {
                                                    NetInt32 priority = qos_prop->find("priority").asVocab();
                                                    // set the packet DSCP value based on some predefined priority levels
                                                    // the expected levels are: LOW, NORM, HIGH, CRIT
                                                    int dscp;
                                                    switch(priority) {
                                                        case VOCAB3('L', 'O', 'W')    : dscp = 10; break;
                                                        case VOCAB4('N', 'O', 'R', 'M'): dscp = 0;  break;
                                                        case VOCAB4('H', 'I', 'G', 'H'): dscp = 36; break;
                                                        case VOCAB4('C', 'R', 'I', 'T'): dscp = 44; break;
                                                        default: dscp = -1;
                                                    };
                                                    if (dscp >= 0) {
                                                        bOk = setTypeOfService(unit, dscp<<2);
                                                    }
                                                }
                                                else if (qos_prop->check("dscp")) {
                                                    QosStyle::PacketPriorityDSCP dscp_class = QosStyle::getDSCPByVocab(qos_prop->find("dscp").asVocab());
                                                    int dscp = -1;
                                                    if (dscp_class == QosStyle::DSCP_Invalid) {
                                                        dscp = qos_prop->find("dscp").asInt();
                                                    } else {
                                                        dscp = (int)dscp_class;
                                                    }
                                                    if ((dscp>=0) && (dscp<64)) {
                                                        bOk = setTypeOfService(unit, dscp<<2);
                                                    }
                                                }
                                                else if (qos_prop->check("tos")) {
                                                    int tos = qos_prop->find("tos").asInt();
                                                    // set the TOS value (backward compatibility)
                                                    bOk = setTypeOfService(unit, tos);
                                                }
                                            }
                                            else {
                                                bOk = false;
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    releaseProperties(p);
                    result.addVocab((bOk) ? Vocab::encode("ok") : Vocab::encode("fail"));
                }
                break;
            default:
                result.addVocab(Vocab::encode("fail"));
                result.addString("property action not known");
                break;
            }
        }
        break;
    default:
        {
            bool ok = false;
            if (adminReader) {
                DummyConnector con;
                cmd.write(con.getWriter());
                lockCallback();
                ok = adminReader->read(con.getReader());
                unlockCallback();
                if (ok) {
                    result.read(con.getReader());
                }
            }
            if (!ok) {
                result.addVocab(Vocab::encode("fail"));
                result.addString("send [help] for list of valid commands");
            }
        }
        break;
    }

    ConnectionWriter *writer = reader.getWriter();
    if (writer!=YARP_NULLPTR) {
        result.write(*writer);
    }

    /**
     * @brief We introduce a nonsense arbitrary delay in the calls to the port administrator
     * for debugging purpose. This is indeed a temporary feature and will be removed soon.
     * The delay is applied if the "NONSENSE_ADMIN_DELAY" environment variable is set.
     */
    ConstString nonsense_delay = NetworkBase::getEnvironment("NONSENSE_ADMIN_DELAY");
    if (nonsense_delay.size())
        yarp::os::SystemClock::delaySystem(atof(nonsense_delay.c_str()));

    return true;
}


bool PortCore::setTypeOfService(PortCoreUnit *unit, int tos) {
    if (unit->isOutput()) {
        OutputProtocol* op = dynamic_cast<PortCoreOutputUnit*>(unit)->getOutPutProtocol();
        if (op)
            return op->getOutputStream().setTypeOfService(tos);
    }

    // Some of the input units may have output stream object to write back to
    // the connection (e.g., tcp ack and reply). Thus the QoS preferences should be
    // also configured for them.
    if (unit->isInput()) {
        InputProtocol* ip = dynamic_cast<PortCoreInputUnit*>(unit)->getInPutProtocol();
        if (ip && ip->getOutput().isOk())
            return ip->getOutput().getOutputStream().setTypeOfService(tos);
    }
    // if there is nothing to be set, returns true
    return true;
}

int PortCore::getTypeOfService(PortCoreUnit *unit) {
    if (unit->isOutput()) {
        OutputProtocol* op = dynamic_cast<PortCoreOutputUnit*>(unit)->getOutPutProtocol();
        if (op)
            return op->getOutputStream().getTypeOfService();
    }

    // Some of the input units may have output stream object to write back to
    // the connection (e.g., tcp ack and reply). Thus the QoS preferences should be
    // also configured for them.
    if (unit->isInput()) {
        InputProtocol* ip = dynamic_cast<PortCoreInputUnit*>(unit)->getInPutProtocol();
        if (ip && ip->getOutput().isOk())
            return ip->getOutput().getOutputStream().getTypeOfService();
    }
    return -1;
}

// attach a portmonitor plugin to the port or to a specific connection
bool PortCore::attachPortMonitor(yarp::os::Property& prop, bool isOutput, ConstString &errMsg) {
    // attach to the current port
    Carrier *portmonitor = Carriers::chooseCarrier("portmonitor");
    if (!portmonitor) {
        errMsg = "Portmonitor carrier modifier cannot be find or it is not enabled in Yarp!";
        return false;
    }

    if (isOutput) {
        dettachPortMonitor(true);
        prop.put("source", getName());
        prop.put("destination", "");
        prop.put("sender_side", 1);
        prop.put("receiver_side", 0);
        prop.put("carrier", "");
        modifier.outputMutex.lock();
        modifier.outputModifier = portmonitor;
        if (!modifier.outputModifier->configureFromProperty(prop)) {
            modifier.releaseOutModifier();
            errMsg = "Failed to configure the portmonitor plug-in";
            modifier.outputMutex.unlock();
            return false;
        }
        modifier.outputMutex.unlock();
    }
    else {
        dettachPortMonitor(false);
        prop.put("source", "");
        prop.put("destination", getName());
        prop.put("sender_side", 0);
        prop.put("receiver_side", 1);
        prop.put("carrier", "");
        modifier.inputMutex.lock();
        modifier.inputModifier = portmonitor;
        if (!modifier.inputModifier->configureFromProperty(prop)) {
            modifier.releaseInModifier();
            errMsg = "Failed to configure the portmonitor plug-in";
            modifier.inputMutex.unlock();
            return false;
        }
        modifier.inputMutex.unlock();
    }
    return true;
}

// detach the portmonitor from the port or specific connection
bool PortCore::dettachPortMonitor(bool isOutput) {
    if (isOutput) {
        modifier.outputMutex.lock();
        modifier.releaseOutModifier();
        modifier.outputMutex.unlock();
    }
    else {
        modifier.inputMutex.lock();
        modifier.releaseInModifier();
        modifier.inputMutex.unlock();
    }
    return true;
}

bool PortCore::setParamPortMonitor(yarp::os::Property& param,
                                   bool isOutput, yarp::os::ConstString &errMsg) {
    if (isOutput) {
        modifier.outputMutex.lock();
        if (modifier.outputModifier == YARP_NULLPTR) {
            errMsg = "No port modifer is attached to the output";
            modifier.outputMutex.unlock();
            return false;
        }
        modifier.outputModifier->setCarrierParams(param);
        modifier.outputMutex.unlock();
    }
    else {
        modifier.inputMutex.lock();
        if (modifier.inputModifier == YARP_NULLPTR) {
            errMsg = "No port modifer is attached to the input";
            modifier.inputMutex.unlock();
            return false;
        }
        modifier.inputModifier->setCarrierParams(param);
        modifier.inputMutex.unlock();
    }
    return true;
}

bool PortCore::getParamPortMonitor(yarp::os::Property& param,
                                   bool isOutput, yarp::os::ConstString &errMsg) {
    if (isOutput) {
        modifier.outputMutex.lock();
        if (modifier.outputModifier == YARP_NULLPTR) {
            errMsg = "No port modifer is attached to the output";
            modifier.outputMutex.unlock();
            return false;
        }
        modifier.outputModifier->getCarrierParams(param);
        modifier.outputMutex.unlock();
    }
    else {
        modifier.inputMutex.lock();
        if (modifier.inputModifier == YARP_NULLPTR) {
            errMsg = "No port modifer is attached to the input";
            modifier.inputMutex.unlock();
            return false;
        }
        modifier.inputModifier->getCarrierParams(param);
        modifier.inputMutex.unlock();
    }
    return true;
}

void PortCore::reportUnit(PortCoreUnit *unit, bool active) {
    if (unit!=YARP_NULLPTR) {
        bool isLog = (unit->getMode()!="");
        if (isLog) {
            logNeeded = true;
        }
    }
}

bool PortCore::setProcessSchedulingParam(int priority, int policy) {
#if defined(__linux__)
    // set the sched properties of all threads within the process
    struct sched_param sch_param;
    sch_param.__sched_priority = priority;

    DIR *dir;
    char path[PATH_MAX];
    sprintf(path, "/proc/%d/task/", getPid());

    dir = opendir(path);
    if (dir==YARP_NULLPTR) {
        return false;
    }

    struct dirent *d;
    char *end;
    int tid = 0;
    bool ret = true;
    while((d = readdir(dir)) != YARP_NULLPTR) {
        if (!isdigit((unsigned char) *d->d_name)) {
            continue;
        }

        tid = (pid_t) strtol(d->d_name, &end, 10);
        if (d->d_name == end || (end && *end)) {
            closedir(dir);
            return false;
        }
        ret &= (sched_setscheduler(tid, policy, &sch_param) == 0);
    }
    closedir(dir);
    return ret;
#elif defined(YARP_HAS_ACE) // for other platforms
    // TODO: Check how to set the scheduling properties of all process's threads in Windows
    ACE_Sched_Params param(policy, (ACE_Sched_Priority) priority, ACE_SCOPE_PROCESS);
    int ret = ACE_OS::set_scheduling_params(param, ACE_OS::getpid());
    return (ret != -1);
#else
    return false;
#endif
}

int PortCore::getPid() {
#if defined(YARP_HAS_ACE)
    return ACE_OS::getpid();
#elif defined(__linux__)
    return getpid();
#elif defined(_WIN32)
    return (int) GetCurrentProcessId();
#endif
    return -1;
}

Property *PortCore::acquireProperties(bool readOnly) {
    stateMutex.wait();
    if (!readOnly) {
        if (!prop) {
            prop = new Property();
        }
    }
    return prop;
}

void PortCore::releaseProperties(Property *prop) {
    stateMutex.post();
}
