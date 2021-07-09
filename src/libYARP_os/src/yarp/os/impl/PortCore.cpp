/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCore.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/InputProtocol.h>
#include <yarp/os/Name.h>
#include <yarp/os/Network.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/RosNameSpace.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/ConnectionRecorder.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PortCoreInputUnit.h>
#include <yarp/os/impl/PortCoreOutputUnit.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <cstdio>
#include <functional>
#include <random>
#include <regex>
#include <vector>

#ifdef YARP_HAS_ACE
#    include <ace/INET_Addr.h>
#    include <ace/Sched_Params.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#endif

#if defined(__linux__) // used for configuring scheduling properties
#    include <dirent.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif


using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp;

namespace {
YARP_OS_LOG_COMPONENT(PORTCORE, "yarp.os.impl.PortCore")
} // namespace

PortCore::PortCore() = default;

PortCore::~PortCore()
{
    close();
    removeCallbackLock();
}


bool PortCore::listen(const Contact& address, bool shouldAnnounce)
{
    yCDebug(PORTCORE, "Starting listening on %s", address.toURI().c_str());
    // If we're using ACE, we really need to have it initialized before
    // this point.
    if (!NetworkBase::initialized()) {
        yCError(PORTCORE, "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    yCTrace(PORTCORE, "listen");

    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);

        // This method assumes we are not already on the network.
        // We can assume this because it is not a user-facing class,
        // and we carefully never call this method again without
        // calling close().
        yCAssert(PORTCORE, !m_listening);
        yCAssert(PORTCORE, !m_running);
        yCAssert(PORTCORE, !m_closing.load());
        yCAssert(PORTCORE, !m_finished.load());
        yCAssert(PORTCORE, m_face == nullptr);

        // Try to put the port on the network, using the user-supplied
        // address (which may be incomplete).  You can think of
        // this as getting a server socket.
        m_address = address;
        setName(address.getRegName());
        if (m_timeout > 0) {
            m_address.setTimeout(m_timeout);
        }
        m_face = Carriers::listen(m_address);

        // We failed, abort.
        if (m_face == nullptr) {
            return false;
        }

        // Update our address if it was incomplete.
        if (m_address.getPort() <= 0) {
            m_address = m_face->getLocalAddress();
            if (m_address.getRegName() == "...") {
                m_address.setName(std::string("/") + m_address.getHost() + "_" + yarp::conf::numeric::to_string(m_address.getPort()));
                setName(m_address.getRegName());
            }
        }

        // Move into listening phase
        m_listening.store(true);
    }

    // Now that we are on the network, we can let the name server know this.
    if (shouldAnnounce) {
        if (!(NetworkBase::getLocalMode() && NetworkBase::getQueryBypass() == nullptr)) {
            std::string portName = address.getRegName();
            Bottle cmd;
            Bottle reply;
            cmd.addString("announce");
            cmd.addString(portName);
            ContactStyle style;
            NetworkBase::writeToNameServer(cmd, reply, style);
        }
    }

    // Success!
    return true;
}


void PortCore::setReadHandler(PortReader& reader)
{
    // Don't even try to do this when the port is hot, it'll burn you
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, m_reader == nullptr);
    m_reader = &reader;
}

void PortCore::setAdminReadHandler(PortReader& reader)
{
    // Don't even try to do this when the port is hot, it'll burn you
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, m_adminReader == nullptr);
    m_adminReader = &reader;
}

void PortCore::setReadCreator(PortReaderCreator& creator)
{
    // Don't even try to do this when the port is hot, it'll burn you
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, m_readableCreator == nullptr);
    m_readableCreator = &creator;
}


void PortCore::run()
{
    yCTrace(PORTCORE, "run");

    // This is the server thread for the port.  We listen on
    // the network and handle any incoming connections.
    // We don't touch those connections, just shove them
    // in a list and move on.  It is important that this
    // thread doesn't make a connecting client wait just
    // because some other client is slow.

    // We assume that listen() has succeeded and that
    // start() has been called.
    yCAssert(PORTCORE, m_listening.load());
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, !m_closing.load());
    yCAssert(PORTCORE, !m_finished.load());
    yCAssert(PORTCORE, m_starting.load());

    // Enter running phase
    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_running.store(true);
        m_starting.store(false);
    }

    // Notify the start() thread that the run() thread is running
    m_stateCv.notify_one();

    yCTrace(PORTCORE, "run running");

    // Enter main loop, where we block on incoming connections.
    // The loop is exited when PortCore#closing is set.  One last
    // connection will be needed to de-block this thread and ensure
    // that it checks PortCore#closing.
    bool shouldStop = false;
    while (!shouldStop) {

        // Block and wait for a connection
        InputProtocol* ip = m_face->read();

        {
            // Critical section
            std::lock_guard<std::mutex> lock(m_stateMutex);

            // Attach the connection to this port and update its timeout setting
            if (ip != nullptr) {
                ip->attachPort(m_contactable);
                yCDebug(PORTCORE, "received something");
                if (m_timeout > 0) {
                    ip->setTimeout(m_timeout);
                }
            }

            // Check whether we should shut down
            shouldStop |= m_closing.load();

            // Increment a global count of connection events
            m_events++;
        }

        // It we are not shutting down, spin off the connection.
        // It starts life as an input connection (although it
        // may later morph into an output).
        if (!shouldStop) {
            if (ip != nullptr) {
                addInput(ip);
            }
            yCDebug(PORTCORE, "spun off a connection");
            ip = nullptr;
        }

        // If the connection wasn't spun off, just shut it down.
        if (ip != nullptr) {
            ip->close();
            delete ip;
            ip = nullptr;
        }

        // Remove any defunct connections.
        reapUnits();

        // Notify anyone listening for connection changes.
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_connectionListeners = 0;
        m_connectionChangeCv.notify_all();
    }

    yCTrace(PORTCORE, "run closing");

    // The server thread is shutting down.
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_connectionListeners = 0;
    m_connectionChangeCv.notify_all();
    m_finished.store(true);
}


void PortCore::close()
{
    closeMain();

    if (m_prop != nullptr) {
        delete m_prop;
        m_prop = nullptr;
    }
    m_modifier.releaseOutModifier();
    m_modifier.releaseInModifier();
}


bool PortCore::start()
{
    yCTrace(PORTCORE, "start");

    // This wait will, on success, be matched by a post in run()
    std::unique_lock<std::mutex> lock(m_stateMutex);

    // We assume that listen() has been called.
    yCAssert(PORTCORE, m_listening.load());
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, !m_starting.load());
    yCAssert(PORTCORE, !m_finished.load());
    yCAssert(PORTCORE, !m_closing.load());

    m_starting.store(true);

    // Start the server thread.
    bool started = ThreadImpl::start();
    if (!started) {
        // run() won't be happening
        yAssert(false);

    } else {
        // Wait for the signal from the run thread before returning.
        m_stateCv.wait(lock, [&]{ return m_running.load(); });
        yCAssert(PORTCORE, m_running.load());
    }
    return started;
}


bool PortCore::manualStart(const char* sourceName)
{
    yCTrace(PORTCORE, "manualStart");

    // This variant of start() does not create a server thread.
    // That is appropriate if we never expect to receive incoming
    // connections for any reason.  No incoming data, no requests
    // for state information, no requests to change connections,
    // nothing.  We set the port's name to something fake, and
    // act like nothing is wrong.
    m_interruptable = false;
    m_manual = true;
    setName(sourceName);
    return true;
}


void PortCore::resume()
{
    // We are no longer interrupted.
    m_interrupted = false;
}

void PortCore::interrupt()
{
    yCTrace(PORTCORE, "interrupt");

    // This is a no-op if there is no server thread.
    if (!m_listening.load()) {
        return;
    }

    // Ignore any future incoming data
    m_interrupted = true;

    // What about data that is already coming in?
    // If interruptable is not currently set, no worries, the user
    // did not or will not end up blocked on a read.
    if (!m_interruptable) {
        return;
    }

    // Since interruptable is set, it is possible that the user
    // may be blocked on a read.  We send an empty message,
    // which is reserved for giving blocking readers a chance to
    // update their state.
    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (m_reader != nullptr) {
            yCDebug(PORTCORE, "sending update-state message to listener");
            StreamConnectionReader sbr;
            lockCallback();
            m_reader->read(sbr);
            unlockCallback();
        }
    }
}


void PortCore::closeMain()
{
    yCTrace(PORTCORE, "closeMain");

    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);

        // We may not have anything to do.
        if (m_finishing || !(m_running.load() || m_manual)) {
            yCTrace(PORTCORE, "closeMain - nothing to do");
            return;
        }

        yCTrace(PORTCORE, "closeMain - Central");

        // Move into official "finishing" phase.
        m_finishing = true;
        yCDebug(PORTCORE, "now preparing to shut down port");
    }

    // Start disconnecting inputs.  We ask the other side of the
    // connection to do this, so it won't come as a surprise.
    // The details of how disconnection works vary by carrier.
    // While we are doing this, the server thread may be still running.
    // This is because other ports may need to talk to the server
    // to organize details of how a connection should be broken down.
    bool done = false;
    std::string prevName;
    while (!done) {
        done = true;
        std::string removeName;
        {
            // Critical section
            std::lock_guard<std::mutex> lock(m_stateMutex);
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isInput() && !unit->isDoomed()) {
                    Route r = unit->getRoute();
                    std::string s = r.getFromName();
                    if (s.length() >= 1 && s[0] == '/' && s != getName() && s != prevName) {
                        removeName = s;
                        done = false;
                        break;
                    }
                }
            }
        }
        if (!done) {
            yCDebug(PORTCORE, "requesting removal of connection from %s", removeName.c_str());
            bool result = NetworkBase::disconnect(removeName,
                                                  getName(),
                                                  true);
            if (!result) {
                NetworkBase::disconnectInput(getName(),
                                             removeName,
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
        {
            // Critical section
            std::lock_guard<std::mutex> lock(m_stateMutex);
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
                    removeRoute = unit->getRoute();
                    if (removeRoute.getFromName() == getName()) {
                        done = false;
                        break;
                    }
                }
            }
        }
        if (!done) {
            removeUnit(removeRoute, true);
        }
    }

    bool stopRunning = m_running.load();

    // If the server thread is still running, we need to bring it down.
    if (stopRunning) {
        // Let the server thread know we no longer need its services.
        m_closing.store(true);

        // Wake up the server thread the only way we can, by sending
        // a message to it.  Then join it, it is done.
        if (!m_manual) {
            OutputProtocol* op = m_face->write(m_address);
            if (op != nullptr) {
                op->close();
                delete op;
            }
            join();
        }

        // We should be finished now.
        yCAssert(PORTCORE, m_finished.load());

        // Clean up our connection list. We couldn't do this earlier,
        // because the server thread was running.
        closeUnits();

        // Reset some state flags.
        {
            // Critical section
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_finished.store(false);
            m_closing.store(false);
            m_running.store(false);
        }
    }

    // There should be no other threads at this point and we
    // can stop listening on the network.
    if (m_listening.load()) {
        yCAssert(PORTCORE, m_face != nullptr);
        m_face->close();
        delete m_face;
        m_face = nullptr;
        m_listening.store(false);
    }

    // Check if the client is waiting for input.  If so, wake them up
    // with the bad news.  An empty message signifies a request to
    // check the port state.
    if (m_reader != nullptr) {
        yCDebug(PORTCORE, "sending end-of-port message to listener");
        StreamConnectionReader sbr;
        m_reader->read(sbr);
        m_reader = nullptr;
    }

    // We may need to unregister the port with the name server.
    if (stopRunning) {
        std::string name = getName();
        if (name != std::string("")) {
            if (m_controlRegistration) {
                NetworkBase::unregisterName(name);
            }
        }
    }

    // We are done with the finishing process.
    m_finishing = false;

    // We are fresh as a daisy.
    yCAssert(PORTCORE, !m_listening.load());
    yCAssert(PORTCORE, !m_running.load());
    yCAssert(PORTCORE, !m_starting.load());
    yCAssert(PORTCORE, !m_closing.load());
    yCAssert(PORTCORE, !m_finished.load());
    yCAssert(PORTCORE, !m_finishing);
    yCAssert(PORTCORE, m_face == nullptr);
}


int PortCore::getEventCount()
{
    // How many times has the server thread spun off a connection.
    std::lock_guard<std::mutex> lock(m_stateMutex);
    int ct = m_events;
    return ct;
}


void PortCore::closeUnits()
{
    // Empty the PortCore#units list. This is only possible when
    // the server thread is finished.
    yCAssert(PORTCORE, m_finished.load());

    // In the "finished" phase, nobody else touches the units,
    // so we can go ahead and shut them down and delete them.
    for (auto& i : m_units) {
        PortCoreUnit* unit = i;
        if (unit != nullptr) {
            yCDebug(PORTCORE, "closing a unit");
            unit->close();
            yCDebug(PORTCORE, "joining a unit");
            unit->join();
            delete unit;
            yCDebug(PORTCORE, "deleting a unit");
            i = nullptr;
        }
    }

    // Get rid of all our nulls.  Done!
    m_units.clear();
}

void PortCore::reapUnits()
{
    // Connections that should be shut down get tagged as "doomed"
    // but aren't otherwise touched until it is safe to do so.
    if (!m_finished.load()) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        for (auto* unit : m_units) {
            if ((unit != nullptr) && unit->isDoomed() && !unit->isFinished()) {
                std::string s = unit->getRoute().toString();
                yCDebug(PORTCORE, "Informing connection %s that it is doomed", s.c_str());
                unit->close();
                yCDebug(PORTCORE, "Closed connection %s", s.c_str());
                unit->join();
                yCDebug(PORTCORE, "Joined thread of connection %s", s.c_str());
            }
        }
    }
    cleanUnits();
}

void PortCore::cleanUnits(bool blocking)
{
    // We will remove any connections that have finished operating from
    // the PortCore#units list.

    // Depending on urgency, either wait for a safe time to do this
    // or skip if unsafe.
    std::unique_lock<std::mutex> lock(m_stateMutex, std::defer_lock);
    if (blocking) {
        lock.lock();
    } else {
        bool have_lock = lock.try_lock();
        if (!have_lock) {
            return;
        }
    }
    // here we have the lock

    // We will update our connection counts as a by-product.
    int updatedInputCount = 0;
    int updatedOutputCount = 0;
    int updatedDataOutputCount = 0;
    yCDebug(PORTCORE, "/ routine check of connections to this port begins");
    if (!m_finished.load()) {

        // First, we delete and null out any defunct entries in the list.
        for (auto& i : m_units) {
            PortCoreUnit* unit = i;
            if (unit != nullptr) {
                yCDebug(PORTCORE, "| checking connection %s %s", unit->getRoute().toString().c_str(), unit->getMode().c_str());
                if (unit->isFinished()) {
                    std::string con = unit->getRoute().toString();
                    yCDebug(PORTCORE, "|   removing connection %s", con.c_str());
                    unit->close();
                    unit->join();
                    delete unit;
                    i = nullptr;
                    yCDebug(PORTCORE, "|   removed connection %s", con.c_str());
                } else {
                    // No work to do except updating connection counts.
                    if (!unit->isDoomed()) {
                        if (unit->isOutput()) {
                            updatedOutputCount++;
                            if (unit->getMode().empty()) {
                                updatedDataOutputCount++;
                            }
                        }
                        if (unit->isInput()) {
                            if (unit->getRoute().getFromName() != "admin") {
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
        size_t rem = 0;
        for (size_t i2 = 0; i2 < m_units.size(); i2++) {
            if (m_units[i2] != nullptr) {
                if (rem < i2) {
                    m_units[rem] = m_units[i2];
                    m_units[i2] = nullptr;
                }
                rem++;
            }
        }

        // ... Now we get rid of the null entries
        for (size_t i3 = 0; i3 < m_units.size() - rem; i3++) {
            m_units.pop_back();
        }
    }

    // Finalize the connection counts.
    m_dataOutputCount = updatedDataOutputCount;
    lock.unlock();

    m_packetMutex.lock();
    m_inputCount = updatedInputCount;
    m_outputCount = updatedOutputCount;
    m_packetMutex.unlock();
    yCDebug(PORTCORE, "\\ routine check of connections to this port ends");
}


void PortCore::addInput(InputProtocol* ip)
{
    yCTrace(PORTCORE, "addInput");

    // This method is only called by the server thread in its running phase.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will have its own thread associated
    // with it.

    yCAssert(PORTCORE, ip != nullptr);
    std::lock_guard<std::mutex> lock(m_stateMutex);
    PortCoreUnit* unit = new PortCoreInputUnit(*this,
                                               getNextIndex(),
                                               ip,
                                               false);
    yCAssert(PORTCORE, unit != nullptr);
    unit->start();
    m_units.push_back(unit);
    yCTrace(PORTCORE, "there are now %zu units", m_units.size());
}


void PortCore::addOutput(OutputProtocol* op)
{
    yCTrace(PORTCORE, "addOutput");

    // This method is called from threads associated with input
    // connections.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will start with its own thread
    // associated with it, but that thread will be very short-lived
    // if the port is not configured to do background writes.

    yCAssert(PORTCORE, op != nullptr);
    if (!m_finished.load()) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        PortCoreUnit* unit = new PortCoreOutputUnit(*this, getNextIndex(), op);
        yCAssert(PORTCORE, unit != nullptr);
        unit->start();
        m_units.push_back(unit);
    }
}


bool PortCore::isUnit(const Route& route, int index)
{
    // Check if a connection with a specified route (and optional ID) is present
    bool needReap = false;
    if (!m_finished.load()) {
        for (auto* unit : m_units) {
            if (unit != nullptr) {
                Route alt = unit->getRoute();
                std::string wild = "*";
                bool ok = true;
                if (index >= 0) {
                    ok = ok && (unit->getIndex() == index);
                }
                if (ok) {
                    if (route.getFromName() != wild) {
                        ok = ok && (route.getFromName() == alt.getFromName());
                    }
                    if (route.getToName() != wild) {
                        ok = ok && (route.getToName() == alt.getToName());
                    }
                    if (route.getCarrierName() != wild) {
                        ok = ok && (route.getCarrierName() == alt.getCarrierName());
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


bool PortCore::removeUnit(const Route& route, bool synch, bool* except)
{
    // This is a request to remove a connection.  It could arise from any
    // input thread.

    if (except != nullptr) {
        yCDebug(PORTCORE, "asked to remove connection in the way of %s", route.toString().c_str());
        *except = false;
    } else {
        yCDebug(PORTCORE, "asked to remove connection %s", route.toString().c_str());
    }

    // Scan for units that match the given route, and collect their IDs.
    // Mark them as "doomed".
    std::vector<int> removals;

    bool needReap = false;
    if (!m_finished.load()) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        for (auto* unit : m_units) {
            if (unit != nullptr) {
                Route alt = unit->getRoute();
                std::string wild = "*";
                bool ok = true;
                if (route.getFromName() != wild) {
                    ok = ok && (route.getFromName() == alt.getFromName());
                }
                if (route.getToName() != wild) {
                    ok = ok && (route.getToName() == alt.getToName());
                }
                if (route.getCarrierName() != wild) {
                    if (except == nullptr) {
                        ok = ok && (route.getCarrierName() == alt.getCarrierName());
                    } else {
                        if (route.getCarrierName() == alt.getCarrierName()) {
                            *except = true;
                            ok = false;
                        }
                    }
                }

                if (ok) {
                    yCDebug(PORTCORE, "removing connection %s", alt.toString().c_str());
                    removals.push_back(unit->getIndex());
                    unit->setDoomed();
                    needReap = true;
                }
            }
        }
    }

    // If we find one or more matches, we need to do some work.
    // We've marked those matches as "doomed" so they'll get cleared
    // up eventually, but we can speed this up by waking up the
    // server thread.
    if (needReap) {
        yCDebug(PORTCORE, "one or more connections need prodding to die");

        if (m_manual) {
            // No server thread, no problems.
            reapUnits();
        } else {
            // Send a blank message to make up server thread.
            OutputProtocol* op = m_face->write(m_address);
            if (op != nullptr) {
                op->close();
                delete op;
            }
            yCDebug(PORTCORE, "sent message to prod connection death");

            if (synch) {
                // Wait for connections to be cleaned up.
                yCDebug(PORTCORE, "synchronizing with connection death");
                {
                    // Critical section
                    std::unique_lock<std::mutex> lock(m_stateMutex);
                    while (std::any_of(removals.begin(), removals.end(), [&](int removal){ return isUnit(route, removal); })) {
                        m_connectionListeners++;
                        m_connectionChangeCv.wait(lock, [&]{ return m_connectionListeners == 0; });
                    }
                }
            }
        }
    }
    return needReap;
}


bool PortCore::addOutput(const std::string& dest,
                         void* id,
                         OutputStream* os,
                         bool onlyIfNeeded)
{
    YARP_UNUSED(id);
    yCDebug(PORTCORE, "asked to add output to %s", dest.c_str());

    // Buffer to store text describing outcome (successful connection,
    // or a failure).
    BufferedConnectionWriter bw(true);

    // Look up the address we'll be connecting to.
    Contact parts = Name(dest).toAddress();
    Contact contact = NetworkBase::queryName(parts.getRegName());
    Contact address = contact;

    // If we can't find it, say so and abort.
    if (!address.isValid()) {
        bw.appendLine(std::string("Do not know how to connect to ") + dest);
        if (os != nullptr) {
            bw.write(*os);
        }
        return false;
    }

    // We clean all existing connections to the desired destination,
    // optionally stopping if we find one with the right carrier.
    if (onlyIfNeeded) {
        // Remove any existing connections between source and destination
        // with a different carrier.  If we find a connection already
        // present with the correct carrier, then we are done.
        bool except = false;
        removeUnit(Route(getName(), address.getRegName(), address.getCarrier()), true, &except);
        if (except) {
            // Connection already present.
            yCDebug(PORTCORE, "output already present to %s", dest.c_str());
            bw.appendLine(std::string("Desired connection already present from ") + getName() + " to " + dest);
            if (os != nullptr) {
                bw.write(*os);
            }
            return true;
        }
    } else {
        // Remove any existing connections between source and destination.
        removeUnit(Route(getName(), address.getRegName(), "*"), true);
    }

    // Set up a named route for this connection.
    std::string aname = address.getRegName();
    if (aname.empty()) {
        aname = address.toURI(false);
    }
    Route r(getName(),
            aname,
            ((!parts.getCarrier().empty()) ? parts.getCarrier() : address.getCarrier()));
    r.setToContact(contact);

    // Check for any restrictions on the port.  Perhaps it can only
    // read, or write.
    bool allowed = true;
    std::string err;
    std::string append;
    unsigned int f = getFlags();
    bool allow_output = (f & PORTCORE_IS_OUTPUT) != 0;
    bool rpc = (f & PORTCORE_IS_RPC) != 0;
    Name name(r.getCarrierName() + std::string("://test"));
    std::string mode = name.getCarrierModifier("log");
    bool is_log = (!mode.empty());
    if (is_log) {
        if (mode != "in") {
            err = "Logger configured as log." + mode + ", but only log.in is supported";
            allowed = false;
        } else {
            append = "; " + r.getFromName() + " will forward messages and replies (if any) to " + r.getToName();
        }
    }
    if (!allow_output) {
        if (!is_log) {
            bool push = false;
            Carrier* c = Carriers::getCarrierTemplate(r.getCarrierName());
            if (c != nullptr) {
                push = c->isPush();
            }
            if (push) {
                err = "Outputs not allowed";
                allowed = false;
            }
        }
    } else if (rpc) {
        if (m_dataOutputCount >= 1 && !is_log) {
            err = "RPC output already connected";
            allowed = false;
        }
    }

    // If we found a relevant restriction, abort.
    if (!allowed) {
        bw.appendLine(err);
        if (os != nullptr) {
            bw.write(*os);
        }
        return false;
    }

    // Ok! We can go ahead and make a connection.
    OutputProtocol* op = nullptr;
    if (m_timeout > 0) {
        address.setTimeout(m_timeout);
    }
    op = Carriers::connect(address);
    if (op != nullptr) {
        op->attachPort(m_contactable);
        if (m_timeout > 0) {
            op->setTimeout(m_timeout);
        }

        bool ok = op->open(r);
        if (!ok) {
            yCDebug(PORTCORE, "open route error");
            delete op;
            op = nullptr;
        }
    }

    // No connection, abort.
    if (op == nullptr) {
        bw.appendLine(std::string("Cannot connect to ") + dest);
        if (os != nullptr) {
            bw.write(*os);
        }
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
        InputProtocol* ip = &(op->getInput());
        if (!m_finished.load()) {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            PortCoreUnit* unit = new PortCoreInputUnit(*this,
                                                       getNextIndex(),
                                                       ip,
                                                       true);
            yCAssert(PORTCORE, unit != nullptr);
            unit->start();
            m_units.push_back(unit);
        }
    }

    // Communicated the good news.
    bw.appendLine(std::string("Added connection from ") + getName() + " to " + dest + append);
    if (os != nullptr) {
        bw.write(*os);
    }
    cleanUnits();
    return true;
}


void PortCore::removeOutput(const std::string& dest, void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    yCDebug(PORTCORE, "asked to remove output to %s", dest.c_str());

    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route("*", dest, "*"), true)) {
        bw.appendLine(std::string("Removed connection from ") + getName() + " to " + dest);
    } else {
        bw.appendLine(std::string("Could not find an outgoing connection to ") + dest);
    }
    if (os != nullptr) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::removeInput(const std::string& src, void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    yCDebug(PORTCORE, "asked to remove input to %s", src.c_str());

    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    if (removeUnit(Route(src, "*", "*"), true)) {
        bw.appendLine(std::string("Removing connection from ") + src + " to " + getName());
    } else {
        bw.appendLine(std::string("Could not find an incoming connection from ") + src);
    }
    if (os != nullptr) {
        bw.write(*os);
    }
    cleanUnits();
}

void PortCore::describe(void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    cleanUnits();

    // Buffer to store a human-readable description of the port's
    // state.
    BufferedConnectionWriter bw(true);

    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);

        // Report name and address.
        bw.appendLine(std::string("This is ") + m_address.getRegName() + " at " + m_address.toURI());

        // Report outgoing connections.
        int oct = 0;
        for (auto* unit : m_units) {
            if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
                Route route = unit->getRoute();
                std::string msg = "There is an output connection from " + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
                bw.appendLine(msg);
                oct++;
            }
        }
        if (oct < 1) {
            bw.appendLine("There are no outgoing connections");
        }

        // Report incoming connections.
        int ict = 0;
        for (auto* unit : m_units) {
            if ((unit != nullptr) && unit->isInput() && !unit->isFinished()) {
                Route route = unit->getRoute();
                if (!route.getCarrierName().empty()) {
                    std::string msg = "There is an input connection from " + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
                    bw.appendLine(msg);
                    ict++;
                }
            }
        }
        if (ict < 1) {
            bw.appendLine("There are no incoming connections");
        }
    }

    // Send description across network, or print it.
    if (os != nullptr) {
        bw.write(*os);
    } else {
        StringOutputStream sos;
        bw.write(sos);
        printf("%s\n", sos.toString().c_str());
    }
}


void PortCore::describe(PortReport& reporter)
{
    cleanUnits();

    std::lock_guard<std::mutex> lock(m_stateMutex);

    // Report name and address of port.
    PortInfo baseInfo;
    baseInfo.tag = yarp::os::PortInfo::PORTINFO_MISC;
    std::string portName = m_address.getRegName();
    baseInfo.message = std::string("This is ") + portName + " at " + m_address.toURI();
    reporter.report(baseInfo);

    // Report outgoing connections.
    int oct = 0;
    for (auto* unit : m_units) {
        if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
            Route route = unit->getRoute();
            std::string msg = "There is an output connection from " + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
            PortInfo info;
            info.message = msg;
            info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
            info.incoming = false;
            info.portName = portName;
            info.sourceName = route.getFromName();
            info.targetName = route.getToName();
            info.carrierName = route.getCarrierName();
            reporter.report(info);
            oct++;
        }
    }
    if (oct < 1) {
        PortInfo info;
        info.tag = yarp::os::PortInfo::PORTINFO_MISC;
        info.message = "There are no outgoing connections";
        reporter.report(info);
    }

    // Report incoming connections.
    int ict = 0;
    for (auto* unit : m_units) {
        if ((unit != nullptr) && unit->isInput() && !unit->isFinished()) {
            Route route = unit->getRoute();
            std::string msg = "There is an input connection from " + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
            PortInfo info;
            info.message = msg;
            info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
            info.incoming = true;
            info.portName = portName;
            info.sourceName = route.getFromName();
            info.targetName = route.getToName();
            info.carrierName = route.getCarrierName();
            reporter.report(info);
            ict++;
        }
    }
    if (ict < 1) {
        PortInfo info;
        info.tag = yarp::os::PortInfo::PORTINFO_MISC;
        info.message = "There are no incoming connections";
        reporter.report(info);
    }
}


void PortCore::setReportCallback(yarp::os::PortReport* reporter)
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    if (reporter != nullptr) {
        m_eventReporter = reporter;
    }
}

void PortCore::resetReportCallback()
{
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_eventReporter = nullptr;
}

void PortCore::report(const PortInfo& info)
{
    // We are in the context of one of the input or output threads,
    // so our contact with the PortCore must be absolutely minimal.
    //
    // It is safe to pick up the address of the reporter if this is
    // kept constant over the lifetime of the input/output threads.

    if (m_eventReporter != nullptr) {
        m_eventReporter->report(info);
    }
}


bool PortCore::readBlock(ConnectionReader& reader, void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    YARP_UNUSED(os);
    bool result = true;

    // We are in the context of one of the input threads,
    // so our contact with the PortCore must be absolutely minimal.
    //
    // It is safe to pick up the address of the reader since this is
    // constant over the lifetime of the input threads.

    if (m_reader != nullptr && !m_interrupted) {
        m_interruptable = false; // No mutexing; user of interrupt() has to be careful.

        bool haveOutputs = (m_outputCount != 0); // No mutexing, but failure modes are benign.

        if (m_logNeeded && haveOutputs) {
            // Normally, yarp doesn't pay attention to the content of
            // messages received by the client.  Likewise, the content
            // of replies are not monitored.  However it may sometimes
            // be useful to log this traffic.

            ConnectionRecorder recorder;
            recorder.init(&reader);
            lockCallback();
            result = m_reader->read(recorder);
            unlockCallback();
            recorder.fini();
            // send off a log of this transaction to whoever wants it
            sendHelper(recorder, PORTCORE_SEND_LOG);
        } else {
            // YARP is not needed as a middleman
            lockCallback();
            result = m_reader->read(reader);
            unlockCallback();
        }

        m_interruptable = true;
    } else {
        // Read and ignore message, there is no where to send it.
        yCDebug(PORTCORE, "data received, no reader for it");
        Bottle b;
        result = b.read(reader);
    }
    return result;
}


bool PortCore::send(const PortWriter& writer,
                    PortReader* reader,
                    const PortWriter* callback)
{
    // check if there is any modifier
    // we need to protect this part while the modifier
    // plugin is loading or unloading!
    m_modifier.outputMutex.lock();
    if (m_modifier.outputModifier != nullptr) {
        if (!m_modifier.outputModifier->acceptOutgoingData(writer)) {
            m_modifier.outputMutex.unlock();
            return false;
        }
        m_modifier.outputModifier->modifyOutgoingData(writer);
    }
    m_modifier.outputMutex.unlock();
    if (!m_logNeeded) {
        return sendHelper(writer, PORTCORE_SEND_NORMAL, reader, callback);
    }
    // logging is desired, so we need to wrap up and log this send
    // (and any reply it gets)  -- TODO not yet supported
    return sendHelper(writer, PORTCORE_SEND_NORMAL, reader, callback);
}

bool PortCore::sendHelper(const PortWriter& writer,
                          int mode,
                          PortReader* reader,
                          const PortWriter* callback)
{
    if (m_interrupted || m_finishing) {
        return false;
    }

    bool all_ok = true;
    bool gotReply = false;
    int logCount = 0;
    std::string envelopeString = m_envelope;

    // Pass a message to all output units for sending on.  We could
    // be doing more here to cache the serialization of the message
    // and reuse it across output connections.  However, one key
    // optimization is present: external blocks written by
    // yarp::os::ConnectionWriter::appendExternalBlock are never
    // copied.  So for example the core image array in a yarp::sig::Image
    // is untouched by the port communications code.

    yCTrace(PORTCORE, "------- send in real");

    // Give user the chance to know that this object is about to be
    // written.
    writer.onCommencement();

    // All user-facing parts of this port will be blocked on this
    // operation, so we'll want to be snappy. How long the
    // operation lasts will depend on these flags:
    //   * waitAfterSend
    //   * waitBeforeSend
    // set by setWaitAfterSend() and setWaitBeforeSend().
    std::lock_guard<std::mutex> lock(m_stateMutex);

    // If the port is shutting down, abort.
    if (m_finished.load()) {
        return false;
    }

    yCTrace(PORTCORE, "------- send in");
    // Prepare a "packet" for tracking a single message which
    // may travel by multiple outputs.
    m_packetMutex.lock();
    PortCorePacket* packet = m_packets.getFreePacket();
    yCAssert(PORTCORE, packet != nullptr);
    packet->setContent(&writer, false, callback);
    m_packetMutex.unlock();

    // Scan connections, placing message everywhere we can.
    for (auto* unit : m_units) {
        if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
            bool log = (!unit->getMode().empty());
            if (log) {
                // Some connections are for logging only.
                logCount++;
            }
            bool ok = (mode == PORTCORE_SEND_NORMAL) ? (!log) : (log);
            if (!ok) {
                continue;
            }
            bool waiter = m_waitAfterSend || (mode == PORTCORE_SEND_LOG);
            yCTrace(PORTCORE, "------- -- inc");
            m_packetMutex.lock();
            packet->inc(); // One more connection carrying message.
            m_packetMutex.unlock();
            yCTrace(PORTCORE, "------- -- pre-send");
            bool gotReplyOne = false;
            // Send the message off on this connection.
            void* out = unit->send(writer,
                                   reader,
                                   (callback != nullptr) ? callback : (&writer),
                                   reinterpret_cast<void*>(packet),
                                   envelopeString,
                                   waiter,
                                   m_waitBeforeSend,
                                   &gotReplyOne);
            gotReply = gotReply || gotReplyOne;
            yCTrace(PORTCORE, "------- -- send");
            if (out != nullptr) {
                // We got back a report of a message already sent.
                m_packetMutex.lock();
                (static_cast<PortCorePacket*>(out))->dec(); // Message on one fewer connections.
                m_packets.checkPacket(static_cast<PortCorePacket*>(out));
                m_packetMutex.unlock();
            }
            if (waiter) {
                if (unit->isFinished()) {
                    all_ok = false;
                }
            }
            yCTrace(PORTCORE, "------- -- dec");
        }
    }
    yCTrace(PORTCORE, "------- pack check");
    m_packetMutex.lock();

    // We no longer concern ourselves with the message.
    // It may or may not be traveling on some connections.
    // But that is not our problem anymore.
    packet->dec();

    m_packets.checkPacket(packet);
    m_packetMutex.unlock();
    yCTrace(PORTCORE, "------- packed");
    yCTrace(PORTCORE, "------- send out");
    if (mode == PORTCORE_SEND_LOG) {
        if (logCount == 0) {
            m_logNeeded = false;
        }
    }

    yCTrace(PORTCORE, "------- send out real");

    if (m_waitAfterSend && reader != nullptr) {
        all_ok = all_ok && gotReply;
    }

    return all_ok;
}


bool PortCore::isWriting()
{
    bool writing = false;

    // Check if any port is currently writing.  TODO optimize
    // this query by counting down with notifyCompletion().
    if (!m_finished.load()) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        for (auto* unit : m_units) {
            if ((unit != nullptr) && !unit->isFinished() && unit->isBusy()) {
                writing = true;
            }
        }
    }

    return writing;
}


int PortCore::getInputCount()
{
    cleanUnits(false);
    m_packetMutex.lock();
    int result = m_inputCount;
    m_packetMutex.unlock();
    return result;
}

int PortCore::getOutputCount()
{
    cleanUnits(false);
    m_packetMutex.lock();
    int result = m_outputCount;
    m_packetMutex.unlock();
    return result;
}


void PortCore::notifyCompletion(void* tracker)
{
    yCTrace(PORTCORE, "starting notifyCompletion");
    m_packetMutex.lock();
    if (tracker != nullptr) {
        (static_cast<PortCorePacket*>(tracker))->dec();
        m_packets.checkPacket(static_cast<PortCorePacket*>(tracker));
    }
    m_packetMutex.unlock();
    yCTrace(PORTCORE, "stopping notifyCompletion");
}


bool PortCore::setEnvelope(PortWriter& envelope)
{
    m_envelopeWriter.restart();
    bool ok = envelope.write(m_envelopeWriter);
    if (ok) {
        setEnvelope(m_envelopeWriter.toString());
    }
    return ok;
}


void PortCore::setEnvelope(const std::string& envelope)
{
    m_envelope = envelope;
    for (size_t i = 0; i < m_envelope.length(); i++) {
        // It looks like envelopes are constrained to be printable ASCII?
        // I'm not sure why this would be.  TODO check.
        if (m_envelope[i] < 32) {
            m_envelope = m_envelope.substr(0, i);
            break;
        }
    }
    yCDebug(PORTCORE, "set envelope to %s", m_envelope.c_str());
}

std::string PortCore::getEnvelope()
{
    return m_envelope;
}

bool PortCore::getEnvelope(PortReader& envelope)
{
    StringInputStream sis;
    sis.add(m_envelope);
    sis.add("\r\n");
    StreamConnectionReader sbr;
    Route route;
    sbr.reset(sis, nullptr, route, 0, true);
    return envelope.read(sbr);
}

// Make an RPC connection to talk to a ROS API, send a message, get reply.
// NOTE: ROS support can now be moved out of here, once all documentation
// of older ways to interoperate with it are purged and people stop
// doing it.
static bool __pc_rpc(const Contact& c,
                     const char* carrier,
                     Bottle& writer,
                     Bottle& reader,
                     bool verbose)
{
    ContactStyle style;
    style.quiet = !verbose;
    style.timeout = 4;
    style.carrier = carrier;
    bool ok = Network::write(c, writer, reader, style);
    return ok;
}

// ACE is sometimes confused by localhost aliases, in a ROS-incompatible
// way.  This method does a quick sanity check if we are using ROS.
static bool __tcp_check(const Contact& c)
{
#ifdef YARP_HAS_ACE
    ACE_INET_Addr addr;
    int result = addr.set(c.getPort(), c.getHost().c_str());
    if (result != 0) {
        yCWarning(PORTCORE, "ACE choked on %s:%d\n", c.getHost().c_str(), c.getPort());
    }
    result = addr.set(c.getPort(), "127.0.0.1");
    if (result != 0) {
        yCWarning(PORTCORE, "ACE choked on 127.0.0.1:%d\n", c.getPort());
    }
    result = addr.set(c.getPort(), "127.0.1.1");
    if (result != 0) {
        yCWarning(PORTCORE, "ACE choked on 127.0.1.1:%d\n", c.getPort());
    }
#endif
    return true;
}

namespace {
enum class PortCoreCommand : yarp::conf::vocab32_t
{
    Unknown = 0,
    Help = yarp::os::createVocab32('h', 'e', 'l', 'p'),
    Ver = yarp::os::createVocab32('v', 'e', 'r'),
    Pray = yarp::os::createVocab32('p', 'r', 'a', 'y'),
    Add = yarp::os::createVocab32('a', 'd', 'd'),
    Del = yarp::os::createVocab32('d', 'e', 'l'),
    Atch = yarp::os::createVocab32('a', 't', 'c', 'h'),
    Dtch = yarp::os::createVocab32('d', 't', 'c', 'h'),
    List = yarp::os::createVocab32('l', 'i', 's', 't'),
    Set = yarp::os::createVocab32('s', 'e', 't'),
    Get = yarp::os::createVocab32('g', 'e', 't'),
    Prop = yarp::os::createVocab32('p', 'r', 'o', 'p'),
    RosPublisherUpdate = yarp::os::createVocab32('r', 'p', 'u', 'p'),
    RosRequestTopic = yarp::os::createVocab32('r', 't', 'o', 'p'),
    RosGetPid = yarp::os::createVocab32('p', 'i', 'd'),
    RosGetBusInfo = yarp::os::createVocab32('b', 'u', 's'),
};

enum class PortCoreConnectionDirection : yarp::conf::vocab32_t
{
    Error = 0,
    Out = yarp::os::createVocab32('o', 'u', 't'),
    In = yarp::os::createVocab32('i', 'n'),
};

enum class PortCorePropertyAction : yarp::conf::vocab32_t
{
    Error = 0,
    Get = yarp::os::createVocab32('g', 'e', 't'),
    Set = yarp::os::createVocab32('s', 'e', 't')
};

PortCoreCommand parseCommand(const yarp::os::Value& v)
{
    if (v.isString()) {
        // We support ROS client API these days.  Here we recode some long ROS
        // command names, just for convenience.
        std::string cmd = v.asString();
        if (cmd == "publisherUpdate") {
            return PortCoreCommand::RosPublisherUpdate;
        }
        if (cmd == "requestTopic") {
            return PortCoreCommand::RosRequestTopic;
        }
        if (cmd == "getPid") {
            return PortCoreCommand::RosGetPid;
        }
        if (cmd == "getBusInfo") {
            return PortCoreCommand::RosGetBusInfo;
        }
    }

    auto cmd = static_cast<PortCoreCommand>(v.asVocab32());
    switch (cmd) {
    case PortCoreCommand::Help:
    case PortCoreCommand::Ver:
    case PortCoreCommand::Pray:
    case PortCoreCommand::Add:
    case PortCoreCommand::Del:
    case PortCoreCommand::Atch:
    case PortCoreCommand::Dtch:
    case PortCoreCommand::List:
    case PortCoreCommand::Set:
    case PortCoreCommand::Get:
    case PortCoreCommand::Prop:
    case PortCoreCommand::RosPublisherUpdate:
    case PortCoreCommand::RosRequestTopic:
    case PortCoreCommand::RosGetPid:
    case PortCoreCommand::RosGetBusInfo:
        return cmd;
    default:
        return PortCoreCommand::Unknown;
    }
}

PortCoreConnectionDirection parseConnectionDirection(yarp::conf::vocab32_t v, bool errorIsOut = false)
{
    auto dir = static_cast<PortCoreConnectionDirection>(v);
    switch (dir) {
    case PortCoreConnectionDirection::In:
    case PortCoreConnectionDirection::Out:
        return dir;
    default:
        return errorIsOut ? PortCoreConnectionDirection::Out : PortCoreConnectionDirection::Error;
    }
}

PortCorePropertyAction parsePropertyAction(yarp::conf::vocab32_t v)
{
    auto action = static_cast<PortCorePropertyAction>(v);
    switch (action) {
    case PortCorePropertyAction::Get:
    case PortCorePropertyAction::Set:
        return action;
    default:
        return PortCorePropertyAction::Error;
    }
}

void describeRoute(const Route& route, Bottle& result)
{
    Bottle& bfrom = result.addList();
    bfrom.addString("from");
    bfrom.addString(route.getFromName());

    Bottle& bto = result.addList();
    bto.addString("to");
    bto.addString(route.getToName());

    Bottle& bcarrier = result.addList();
    bcarrier.addString("carrier");
    bcarrier.addString(route.getCarrierName());

    Carrier* carrier = Carriers::chooseCarrier(route.getCarrierName());
    if (carrier->isConnectionless()) {
        Bottle& bconnectionless = result.addList();
        bconnectionless.addString("connectionless");
        bconnectionless.addInt32(1);
    }
    if (!carrier->isPush()) {
        Bottle& breverse = result.addList();
        breverse.addString("push");
        breverse.addInt32(0);
    }
    delete carrier;
}

} // namespace

bool PortCore::adminBlock(ConnectionReader& reader,
                          void* id)
{
    Bottle cmd;
    Bottle result;

    // We've received a message to the port that is marked as administrative.
    // That means that instead of passing it along as data to the user of the
    // port, the port itself is responsible for reading and responding to
    // it.  So let's read the message and see what we're supposed to do.
    cmd.read(reader);

    yCDebug(PORTCORE, "Port %s received command %s", getName().c_str(), cmd.toString().c_str());

    auto handleAdminHelpCmd = []() {
        Bottle result;
        // We give a list of the most useful administrative commands.
        result.addVocab32('m', 'a', 'n', 'y');
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
        return result;
    };

    auto handleAdminVerCmd = []() {
        // Gives a version number for the administrative commands.
        // It is distinct from YARP library versioning.
        Bottle result;
        result.addVocab32("ver");
        result.addInt32(1);
        result.addInt32(2);
        result.addInt32(3);
        return result;
    };

    auto handleAdminPrayCmd = [this]() {
        // Strongly inspired by nethack #pray command:
        // https://nethackwiki.com/wiki/Prayer
        // http://www.steelypips.org/nethack/pray.html

        Bottle result;

        bool found = false;
        std::string name = yarp::conf::environment::get_string("YARP_ROBOT_NAME", &found);
        if (!found) {
            name = getName();
            // Remove initial "/"
            while (name[0] == '/') {
                name = name.substr(1);
            }
            // Keep only the first part of the port name
            auto i = name.find('/');
            if (i != std::string::npos) {
                name = name.substr(0, i);
            }
        }

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist2(0,1);
        auto d2 = std::bind(dist2, mt);

        result.addString("You begin praying to " + name + ".");
        result.addString("You finish your prayer.");

        static const char* godvoices[] = {
            "booms out",
            "thunders",
            "rings out",
            "booms",
        };
        std::uniform_int_distribution<int> godvoices_dist(0, (sizeof(godvoices) / sizeof(godvoices[0])) - 1);
        auto godvoice = [&]() {
            return std::string(godvoices[godvoices_dist(mt)]);
        };

        static const char* creatures[] = {
            "mortal",
            "creature",
            "robot",
        };
        std::uniform_int_distribution<int> creatures_dist(0, (sizeof(creatures) / sizeof(creatures[0])) - 1);
        auto creature = [&]() {
            return std::string(creatures[creatures_dist(mt)]);
        };

        static const char* auras[] = {
            "amber",
            "light blue",
            "golden",
            "white",
            "orange",
            "black",
        };
        std::uniform_int_distribution<int> auras_dist(0, (sizeof(auras) / sizeof(auras[0])) - 1);
        auto aura = [&]() {
            return std::string(auras[auras_dist(mt)]);
        };

        static const char* items[] = {
            "keyboard",
            "mouse",
            "monitor",
            "headphones",
            "smartphone",
            "wallet",
            "eyeglasses",
            "shirt",
        };
        std::uniform_int_distribution<int> items_dist(0, (sizeof(items) / sizeof(items[0])) - 1);
        auto item = [&]() {
            return std::string(items[items_dist(mt)]);
        };

        static const char* blessings[] = {
            "You feel more limber.",
            "The slime disappears.",
            "Your amulet vanishes! You can breathe again.",
            "You can breathe again.",
            "You are back on solid ground.",
            "Your stomach feels content.",
            "You feel better.",
            "You feel much better.",
            "Your surroundings change.",
            "Your shape becomes uncertain.",
            "Your chain disappears.",
            "There's a tiger in your tank.",
            "You feel in good health again.",
            "Your eye feels better.",
            "Your eyes feel better.",
            "Looks like you are back in Kansas.",
            "Your <ITEM> softly glows <AURA>.",
        };
        std::uniform_int_distribution<int> blessings_dist(0, (sizeof(blessings) / sizeof(blessings[0])) - 1);
        auto blessing = [&](){
            auto blessing = std::string(blessings[blessings_dist(mt)]);
            blessing = std::regex_replace(blessing, std::regex("<ITEM>"), item());
            blessing = std::regex_replace(blessing, std::regex("<AURA>"), aura());
            return blessing;
        };

        std::uniform_int_distribution<int> dist13(0,12);
        switch(dist13(mt)) {
        case 0:
        case 1:
            result.addString("You feel that " + name + " is " + (d2() ? "bummed" : "displeased") + ".");
            break;
        case 2:
        case 3:
            result.addString("The voice of " + name + " " + godvoice() +
                             ": \"Thou " + (d2() ? "hast strayed from the path" : "art arrogant") +
                             ", " + creature() + ". Thou must relearn thy lessons!\"");
            break;
        case 4:
        case 5:
            result.addString("The voice of " + name + " " + godvoice() +
                             ": \"Thou hast angered me.\"");
            result.addString("A black glow surrounds you.");
            break;
        case 6:
            result.addString("The voice of " + name + " " + godvoice() +
                             ": \"Thou hast angered me.\"");
            break;
        case 7:
        case 8:
            result.addString("The voice of " + name + " " + godvoice() +
                             ": \"Thou durst " + (d2() ? "scorn" : "call upon") +
                             " me? Then die, " + creature() + "!\"");
            break;
        case 9:
            result.addString("You feel that " + name + " is " + (d2() ? "pleased as punch" : "well-pleased") + ".");
            result.addString(blessing());
            break;
        case 10:
            result.addString("You feel that " + name + " is " + (d2() ? "ticklish" : "pleased") + ".");
            result.addString(blessing());
            break;
        case 11:
            result.addString("You feel that " + name + " is " + (d2() ? "full" : "satisfied") + ".");
            result.addString(blessing());
            break;
        default:
            result.addString("The voice of " + name + " " + godvoice() +
                             ": \"Thou hast angered me.\"");
            result.addString("Suddenly, a bolt of lightning strikes you!");
            result.addString("You fry to a crisp!");
            break;
        }

        return result;
    };

    auto handleAdminAddCmd = [this, id](std::string output,
                                        const std::string& carrier) {
        // Add an output to the port.
        Bottle result;
        StringOutputStream cache;
        if (!carrier.empty()) {
            output = carrier + ":/" + output;
        }
        addOutput(output, id, &cache, false);
        std::string r = cache.toString();
        int v = (r[0] == 'A') ? 0 : -1;
        result.addInt32(v);
        result.addString(r);
        return result;
    };

    auto handleAdminDelCmd = [this, id](const std::string& dest) {
        // Delete any inputs or outputs involving the named port.
        Bottle result;
        StringOutputStream cache;
        removeOutput(dest, id, &cache);
        std::string r1 = cache.toString();
        cache.reset();
        removeInput(dest, id, &cache);
        std::string r2 = cache.toString();
        int v = (r1[0] == 'R' || r2[0] == 'R') ? 0 : -1;
        result.addInt32(v);
        if (r1[0] == 'R' && r2[0] != 'R') {
            result.addString(r1);
        } else if (r1[0] != 'R' && r2[0] == 'R') {
            result.addString(r2);
        } else {
            result.addString(r1 + r2);
        }
        return result;
    };

    auto handleAdminAtchCmd = [this](PortCoreConnectionDirection direction,
                                     Property prop) {
        Bottle result;
        switch (direction) {
        case PortCoreConnectionDirection::Out: {
            std::string errMsg;
            if (!attachPortMonitor(prop, true, errMsg)) {
                result.addVocab32("fail");
                result.addString(errMsg);
            } else {
                result.addVocab32("ok");
            }
        } break;
        case PortCoreConnectionDirection::In: {
            std::string errMsg;
            if (!attachPortMonitor(prop, false, errMsg)) {
                result.addVocab32("fail");
                result.addString(errMsg);
            } else {
                result.addVocab32("ok");
            }
        } break;
        case PortCoreConnectionDirection::Error:
            result.addVocab32("fail");
            result.addString("attach command must be followed by [out] or [in]");
        }
        return result;
    };

    auto handleAdminDtchCmd = [this](PortCoreConnectionDirection direction) {
        Bottle result;
        switch (direction) {
        case PortCoreConnectionDirection::Out: {
            if (detachPortMonitor(true)) {
                result.addVocab32("ok");
            } else {
                result.addVocab32("fail");
            }
        } break;
        case PortCoreConnectionDirection::In: {
            if (detachPortMonitor(false)) {
                result.addVocab32("ok");
            } else {
                result.addVocab32("fail");
            }
        } break;
        case PortCoreConnectionDirection::Error:
            result.addVocab32("fail");
            result.addString("detach command must be followed by [out] or [in]");
        };
        return result;
    };

    auto handleAdminListCmd = [this](const PortCoreConnectionDirection direction,
                                     const std::string& target) {
        Bottle result;
        switch (direction) {
        case PortCoreConnectionDirection::In: {
            // Return a list of all input connections.
            std::lock_guard<std::mutex> lock(m_stateMutex);
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isInput() && !unit->isFinished()) {
                    Route route = unit->getRoute();
                    if (target.empty()) {
                        const std::string& name = route.getFromName();
                        if (!name.empty()) {
                            result.addString(name);
                        }
                    } else if (route.getFromName() == target) {
                        describeRoute(route, result);
                    }
                }
            }
        } break;
        case PortCoreConnectionDirection::Out: {
            // Return a list of all output connections.
            std::lock_guard<std::mutex> lock(m_stateMutex);
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
                    Route route = unit->getRoute();
                    if (target.empty()) {
                        result.addString(route.getToName());
                    } else if (route.getToName() == target) {
                        describeRoute(route, result);
                    }
                }
            }
        } break;
        case PortCoreConnectionDirection::Error:
            // Should never happen
            yCAssert(PORTCORE, false);
            break;
        }
        return result;
    };

    auto handleAdminSetInCmd = [this](const std::string& target,
                                      const Property& property) {
        Bottle result;
        // Set carrier parameters on a given input connection.
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (target.empty()) {
            result.addInt32(-1);
            result.addString("target port is not specified.\r\n");
        } else {
            if (target == getName()) {
                std::string errMsg;
                if (!setParamPortMonitor(property, false, errMsg)) {
                    result.addVocab32("fail");
                    result.addString(errMsg);
                } else {
                    result.addVocab32("ok");
                }
            } else {
                for (auto* unit : m_units) {
                    if ((unit != nullptr) && unit->isInput() && !unit->isFinished()) {
                        Route route = unit->getRoute();
                        if (route.getFromName() == target) {
                            unit->setCarrierParams(property);
                            result.addInt32(0);
                            std::string msg = "Configured connection from ";
                            msg += route.getFromName();
                            msg += "\r\n";
                            result.addString(msg);
                            break;
                        }
                    }
                }
            }
            if (result.size() == 0) {
                result.addInt32(-1);
                std::string msg = "Could not find an incoming connection from ";
                msg += target;
                msg += "\r\n";
                result.addString(msg);
            }
        }
        return result;
    };

    auto handleAdminSetOutCmd = [this](const std::string& target,
                                       const Property& property) {
        Bottle result;
        // Set carrier parameters on a given output connection.
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (target.empty()) {
            result.addInt32(-1);
            result.addString("target port is not specified.\r\n");
        } else {
            if (target == getName()) {
                std::string errMsg;
                if (!setParamPortMonitor(property, true, errMsg)) {
                    result.addVocab32("fail");
                    result.addString(errMsg);
                } else {
                    result.addVocab32("ok");
                }
            } else {
                for (auto* unit : m_units) {
                    if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
                        Route route = unit->getRoute();
                        if (route.getToName() == target) {
                            unit->setCarrierParams(property);
                            result.addInt32(0);
                            std::string msg = "Configured connection to ";
                            msg += route.getToName();
                            msg += "\r\n";
                            result.addString(msg);
                            break;
                        }
                    }
                }
            }
            if (result.size() == 0) {
                result.addInt32(-1);
                std::string msg = "Could not find an incoming connection to ";
                msg += target;
                msg += "\r\n";
                result.addString(msg);
            }
        }
        return result;
    };

    auto handleAdminGetInCmd = [this](const std::string& target) {
        Bottle result;
        // Get carrier parameters for a given input connection.
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (target.empty()) {
            result.addInt32(-1);
            result.addString("target port is not specified.\r\n");
        } else if (target == getName()) {
            yarp::os::Property property;
            std::string errMsg;
            if (!getParamPortMonitor(property, false, errMsg)) {
                result.addVocab32("fail");
                result.addString(errMsg);
            } else {
                result.addDict() = property;
            }
        } else {
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isInput() && !unit->isFinished()) {
                    Route route = unit->getRoute();
                    if (route.getFromName() == target) {
                        yarp::os::Property property;
                        unit->getCarrierParams(property);
                        result.addDict() = property;
                        break;
                    }
                }
            }
            if (result.size() == 0) {
                result.addInt32(-1);
                std::string msg = "Could not find an incoming connection from ";
                msg += target;
                msg += "\r\n";
                result.addString(msg);
            }
        }
        return result;
    };

    auto handleAdminGetOutCmd = [this](const std::string& target) {
        Bottle result;
        // Get carrier parameters for a given output connection.
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if (target.empty()) {
            result.addInt32(-1);
            result.addString("target port is not specified.\r\n");
        } else if (target == getName()) {
            yarp::os::Property property;
            std::string errMsg;
            if (!getParamPortMonitor(property, true, errMsg)) {
                result.addVocab32("fail");
                result.addString(errMsg);
            } else {
                result.addDict() = property;
            }
        } else {
            for (auto* unit : m_units) {
                if ((unit != nullptr) && unit->isOutput() && !unit->isFinished()) {
                    Route route = unit->getRoute();
                    if (route.getToName() == target) {
                        yarp::os::Property property;
                        unit->getCarrierParams(property);
                        result.addDict() = property;
                        break;
                    }
                }
            }
            if (result.size() == 0) {
                result.addInt32(-1);
                std::string msg = "Could not find an incoming connection to ";
                msg += target;
                msg += "\r\n";
                result.addString(msg);
            }
        }
        return result;
    };

    auto handleAdminPropGetCmd = [this](const std::string& key) {
        Bottle result;
        Property* p = acquireProperties(false);
        if (p != nullptr) {
            if (key.empty()) {
                result.fromString(p->toString());
            } else {
                // request: "prop get /portname"
                if (key[0] == '/') {
                    bool bFound = false;
                    // check for their own name
                    if (key == getName()) {
                        bFound = true;
                        Bottle& sched = result.addList();
                        sched.addString("sched");
                        Property& sched_prop = sched.addDict();
                        sched_prop.put("tid", static_cast<int>(this->getTid()));
                        sched_prop.put("priority", this->getPriority());
                        sched_prop.put("policy", this->getPolicy());

                        SystemInfo::ProcessInfo info = SystemInfo::getProcessInfo();
                        Bottle& proc = result.addList();
                        proc.addString("process");
                        Property& proc_prop = proc.addDict();
                        proc_prop.put("pid", info.pid);
                        proc_prop.put("name", (info.pid != -1) ? info.name : "unknown");
                        proc_prop.put("arguments", (info.pid != -1) ? info.arguments : "unknown");
                        proc_prop.put("priority", info.schedPriority);
                        proc_prop.put("policy", info.schedPolicy);

                        SystemInfo::PlatformInfo pinfo = SystemInfo::getPlatformInfo();
                        Bottle& platform = result.addList();
                        platform.addString("platform");
                        Property& platform_prop = platform.addDict();
                        platform_prop.put("os", pinfo.name);
                        platform_prop.put("hostname", m_address.getHost());

                        unsigned int f = getFlags();
                        bool is_input = (f & PORTCORE_IS_INPUT) != 0;
                        bool is_output = (f & PORTCORE_IS_OUTPUT) != 0;
                        bool is_rpc = (f & PORTCORE_IS_RPC) != 0;
                        Bottle& port = result.addList();
                        port.addString("port");
                        Property& port_prop = port.addDict();
                        port_prop.put("is_input", is_input);
                        port_prop.put("is_output", is_output);
                        port_prop.put("is_rpc", is_rpc);
                        port_prop.put("type", getType().getName());
                    } else {
                        for (auto* unit : m_units) {
                            if ((unit != nullptr) && !unit->isFinished()) {
                                Route route = unit->getRoute();
                                std::string coreName = (unit->isOutput()) ? route.getToName() : route.getFromName();
                                if (key == coreName) {
                                    bFound = true;
                                    int priority = unit->getPriority();
                                    int policy = unit->getPolicy();
                                    int tos = getTypeOfService(unit);
                                    int tid = static_cast<int>(unit->getTid());
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
                        }     // end for loop
                    }         // end portName == getname()

                    if (!bFound) { // cannot find any port matches the requested one
                        result.addVocab32("fail");
                        std::string msg = "cannot find any connection to/from ";
                        msg = msg + key;
                        result.addString(msg);
                    }
                    // end of (portName[0] == '/')
                } else {
                    result.add(p->find(key));
                }
            }
        }
        releaseProperties(p);
        return result;
    };

    auto handleAdminPropSetCmd = [this](const std::string& key,
                                        const Value& value,
                                        const Bottle& process,
                                        const Bottle& sched,
                                        const Bottle& qos) {
        Bottle result;
        Property* p = acquireProperties(false);
        bool bOk = true;
        if (p != nullptr) {
            p->put(key, value);
            // setting scheduling properties of all threads within the process
            // scope through the admin port
            // e.g. prop set /current_port (process ((priority 30) (policy 1)))
            if (!process.isNull()) {
                std::string portName = key;
                if ((!portName.empty()) && (portName[0] == '/')) {
                    // check for their own name
                    if (portName == getName()) {
                        bOk = false;
                        Bottle* process_prop = process.find("process").asList();
                        if (process_prop != nullptr) {
                            int prio = -1;
                            int policy = -1;
                            if (process_prop->check("priority")) {
                                prio = process_prop->find("priority").asInt32();
                            }
                            if (process_prop->check("policy")) {
                                policy = process_prop->find("policy").asInt32();
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
            if (!sched.isNull()) {
                if ((!key.empty()) && (key[0] == '/')) {
                    bOk = false;
                    for (auto* unit : m_units) {
                        if ((unit != nullptr) && !unit->isFinished()) {
                            Route route = unit->getRoute();
                            std::string portName = (unit->isOutput()) ? route.getToName() : route.getFromName();

                            if (portName == key) {
                                Bottle* sched_prop = sched.find("sched").asList();
                                if (sched_prop != nullptr) {
                                    int prio = -1;
                                    int policy = -1;
                                    if (sched_prop->check("priority")) {
                                        prio = sched_prop->find("priority").asInt32();
                                    }
                                    if (sched_prop->check("policy")) {
                                        policy = sched_prop->find("policy").asInt32();
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
            if (!qos.isNull()) {
                if ((!key.empty()) && (key[0] == '/')) {
                    bOk = false;
                    for (auto* unit : m_units) {
                        if ((unit != nullptr) && !unit->isFinished()) {
                            Route route = unit->getRoute();
                            std::string portName = (unit->isOutput()) ? route.getToName() : route.getFromName();
                            if (portName == key) {
                                Bottle* qos_prop = qos.find("qos").asList();
                                if (qos_prop != nullptr) {
                                    int tos = -1;
                                    if (qos_prop->check("priority")) {
                                        // set the packet TOS value on the socket based on some predefined
                                        // priority levels.
                                        // the expected levels are: LOW, NORM, HIGH, CRIT
                                        NetInt32 priority = qos_prop->find("priority").asVocab32();
                                        int dscp;
                                        switch (priority) {
                                        case yarp::os::createVocab32('L', 'O', 'W'):
                                            dscp = 10;
                                            break;
                                        case yarp::os::createVocab32('N', 'O', 'R', 'M'):
                                            dscp = 0;
                                            break;
                                        case yarp::os::createVocab32('H', 'I', 'G', 'H'):
                                            dscp = 36;
                                            break;
                                        case yarp::os::createVocab32('C', 'R', 'I', 'T'):
                                            dscp = 44;
                                            break;
                                        default:
                                            dscp = -1;
                                        }
                                        if (dscp >= 0) {
                                            tos = (dscp << 2);
                                        }
                                    } else if (qos_prop->check("dscp")) {
                                        // Set the packet TOS value on the socket based on the DSCP level
                                        QosStyle::PacketPriorityDSCP dscp_class = QosStyle::getDSCPByVocab(qos_prop->find("dscp").asVocab32());
                                        int dscp = -1;
                                        if (dscp_class == QosStyle::DSCP_Invalid) {
                                            auto dscp_val = qos_prop->find("dscp");
                                            if (dscp_val.isInt32()) {
                                                dscp = dscp_val.asInt32();
                                            }
                                        } else {
                                            dscp = static_cast<int>(dscp_class);
                                        }
                                        if ((dscp >= 0) && (dscp < 64)) {
                                            tos = (dscp << 2);
                                        }
                                    } else if (qos_prop->check("tos")) {
                                        // Set the TOS value directly
                                        auto tos_val = qos_prop->find("tos");
                                        if (tos_val.isInt32()) {
                                            tos = tos_val.asInt32();
                                        }
                                    }
                                    if (tos >= 0) {
                                        bOk = setTypeOfService(unit, tos);
                                    }
                                } else {
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
        result.addVocab32((bOk) ? "ok" : "fail");
        return result;
    };

    // NOTE: YARP partially supports the ROS Slave API https://wiki.ros.org/ROS/Slave_API

    auto handleAdminRosPublisherUpdateCmd = [this](const std::string& topic, Bottle* pubs) {
        // When running against a ROS name server, we need to
        // support ROS-style callbacks for connecting publishers
        // with subscribers.  Note: this should not be necessary
        // anymore, now that a dedicated yarp::os::Node class
        // has been implemented, but is still needed for older
        // ways of interfacing with ROS without using dedicated
        // node ports.
        Bottle result;
        if (pubs != nullptr) {
            Property listed;
            for (size_t i = 0; i < pubs->size(); i++) {
                std::string pub = pubs->get(i).asString();
                listed.put(pub, 1);
            }
            Property present;
            {
                // Critical section
                std::lock_guard<std::mutex> lock(m_stateMutex);
                for (auto* unit : m_units) {
                    if ((unit != nullptr) && unit->isPupped()) {
                        std::string me = unit->getPupString();
                        present.put(me, 1);
                        if (!listed.check(me)) {
                            unit->setDoomed();
                        }
                    }
                }
            }
            for (size_t i = 0; i < pubs->size(); i++) {
                std::string pub = pubs->get(i).asString();
                if (!present.check(pub)) {
                    yCDebug(PORTCORE, "ROS ADD %s", pub.c_str());
                    Bottle req;
                    Bottle reply;
                    req.addString("requestTopic");
                    NestedContact nc(getName());
                    req.addString(nc.getNodeName());
                    req.addString(topic);
                    Bottle& lst = req.addList();
                    Bottle& sublst = lst.addList();
                    sublst.addString("TCPROS");
                    yCDebug(PORTCORE, "Sending [%s] to %s", req.toString().c_str(), pub.c_str());
                    Contact c = Contact::fromString(pub);
                    if (!__pc_rpc(c, "xmlrpc", req, reply, false)) {
                        fprintf(stderr, "Cannot connect to ROS subscriber %s\n", pub.c_str());
                        // show diagnosics
                        __pc_rpc(c, "xmlrpc", req, reply, true);
                        __tcp_check(c);
                    } else {
                        Bottle* pref = reply.get(2).asList();
                        std::string hostname;
                        std::string carrier;
                        int portnum = 0;
                        if (reply.get(0).asInt32() != 1) {
                            fprintf(stderr, "Failure looking up topic %s: %s\n", topic.c_str(), reply.toString().c_str());
                        } else if (pref == nullptr) {
                            fprintf(stderr, "Failure looking up topic %s: expected list of protocols\n", topic.c_str());
                        } else if (pref->get(0).asString() != "TCPROS") {
                            fprintf(stderr, "Failure looking up topic %s: unsupported protocol %s\n", topic.c_str(), pref->get(0).asString().c_str());
                        } else {
                            Value hostname2 = pref->get(1);
                            Value portnum2 = pref->get(2);
                            hostname = hostname2.asString();
                            portnum = portnum2.asInt32();
                            carrier = "tcpros+role.pub+topic.";
                            carrier += topic;
                            yCDebug(PORTCORE, "topic %s available at %s:%d", topic.c_str(), hostname.c_str(), portnum);
                        }
                        if (portnum != 0) {
                            Contact addr(hostname, portnum);
                            OutputProtocol* op = nullptr;
                            Route r = Route(getName(), pub, carrier);
                            op = Carriers::connect(addr);
                            if (op == nullptr) {
                                fprintf(stderr, "NO CONNECTION\n");
                                std::exit(1);
                            } else {
                                op->attachPort(m_contactable);
                                op->open(r);
                            }
                            Route route = op->getRoute();
                            route.swapNames();
                            op->rename(route);
                            InputProtocol* ip = &(op->getInput());
                            {
                                // Critical section
                                std::lock_guard<std::mutex> lock(m_stateMutex);
                                PortCoreUnit* unit = new PortCoreInputUnit(*this,
                                                                           getNextIndex(),
                                                                           ip,
                                                                           true);
                                yCAssert(PORTCORE, unit != nullptr);
                                unit->setPupped(pub);
                                unit->start();
                                m_units.push_back(unit);
                            }
                        }
                    }
                }
            }
        }
        result.addInt32(1);
        result.addString("ok");
        return result;
    };

    auto handleAdminRosRequestTopicCmd = [this]() {
        // ROS-style query for topics.
        Bottle result;
        result.addInt32(1);
        NestedContact nc(getName());
        result.addString(nc.getNodeName());
        Bottle& lst = result.addList();
        Contact addr = getAddress();
        lst.addString("TCPROS");
        lst.addString(addr.getHost());
        lst.addInt32(addr.getPort());
        return result;
    };

    auto handleAdminRosGetPidCmd = []() {
        // ROS-style query for PID.
        Bottle result;
        result.addInt32(1);
        result.addString("");
        result.addInt32(yarp::os::impl::getpid());
        return result;
    };

    auto handleAdminRosGetBusInfoCmd = []() {
        // ROS-style query for bus information - we support this
        // in yarp::os::Node but not otherwise.
        Bottle result;
        result.addInt32(1);
        result.addString("");
        result.addList().addList();
        return result;
    };

    auto handleAdminUnknownCmd = [this](const Bottle& cmd) {
        Bottle result;
        bool ok = false;
        if (m_adminReader != nullptr) {
            DummyConnector con;
            cmd.write(con.getWriter());
            lockCallback();
            ok = m_adminReader->read(con.getReader());
            unlockCallback();
            if (ok) {
                result.read(con.getReader());
            }
        }
        if (!ok) {
            result.addVocab32("fail");
            result.addString("send [help] for list of valid commands");
        }
        return result;
    };

    const PortCoreCommand command = parseCommand(cmd.get(0));
    switch (command) {
    case PortCoreCommand::Help:
        result = handleAdminHelpCmd();
        break;
    case PortCoreCommand::Ver:
        result = handleAdminVerCmd();
        break;
    case PortCoreCommand::Pray:
        result = handleAdminPrayCmd();
        break;
    case PortCoreCommand::Add: {
        std::string output = cmd.get(1).asString();
        std::string carrier = cmd.get(2).asString();
        result = handleAdminAddCmd(std::move(output), carrier);
    } break;
    case PortCoreCommand::Del: {
        const std::string dest = cmd.get(1).asString();
        result = handleAdminDelCmd(dest);
    } break;
    case PortCoreCommand::Atch: {
        const PortCoreConnectionDirection direction = parseConnectionDirection(cmd.get(1).asVocab32());
        Property prop(cmd.get(2).asString().c_str());
        result = handleAdminAtchCmd(direction, std::move(prop));
    } break;
    case PortCoreCommand::Dtch: {
        const PortCoreConnectionDirection direction = parseConnectionDirection(cmd.get(1).asVocab32());
        result = handleAdminDtchCmd(direction);
    } break;
    case PortCoreCommand::List: {
        const PortCoreConnectionDirection direction = parseConnectionDirection(cmd.get(1).asVocab32(), true);
        const std::string target = cmd.get(2).asString();
        result = handleAdminListCmd(direction, target);
    } break;
    case PortCoreCommand::Set: {
        const PortCoreConnectionDirection direction = parseConnectionDirection(cmd.get(1).asVocab32(), true);
        const std::string target = cmd.get(2).asString();
        yarp::os::Property property;
        property.fromString(cmd.toString());
        switch (direction) {
        case PortCoreConnectionDirection::In:
            result = handleAdminSetInCmd(target, property);
            break;
        case PortCoreConnectionDirection::Out:
            result = handleAdminSetOutCmd(target, property);
            break;
        case PortCoreConnectionDirection::Error:
            yCAssert(PORTCORE, false); // Should never happen (error is out)
            break;
        }
    } break;
    case PortCoreCommand::Get: {
        const PortCoreConnectionDirection direction = parseConnectionDirection(cmd.get(1).asVocab32(), true);
        const std::string target = cmd.get(2).asString();
        switch (direction) {
        case PortCoreConnectionDirection::In:
            result = handleAdminGetInCmd(target);
            break;
        case PortCoreConnectionDirection::Out:
            result = handleAdminGetOutCmd(target);
            break;
        case PortCoreConnectionDirection::Error:
            yCAssert(PORTCORE, false); // Should never happen (error is out)
            break;
        }
    } break;
    case PortCoreCommand::Prop: {
        PortCorePropertyAction action = parsePropertyAction(cmd.get(1).asVocab32());
        const std::string key = cmd.get(2).asString();
        // Set/get arbitrary properties on a port.
        switch (action) {
        case PortCorePropertyAction::Get:
            result = handleAdminPropGetCmd(key);
            break;
        case PortCorePropertyAction::Set: {
            const Value& value = cmd.get(3);
            const Bottle& process = cmd.findGroup("process");
            const Bottle& sched = cmd.findGroup("sched");
            const Bottle& qos = cmd.findGroup("qos");
            result = handleAdminPropSetCmd(key, value, process, sched, qos);
        } break;
        case PortCorePropertyAction::Error:
            result.addVocab32("fail");
            result.addString("property action not known");
            break;
        }
    } break;
    case PortCoreCommand::RosPublisherUpdate: {
        yCDebug(PORTCORE, "publisherUpdate! --> %s", cmd.toString().c_str());
        // std::string caller_id = cmd.get(1).asString(); // Currently unused
        std::string topic = RosNameSpace::fromRosName(cmd.get(2).asString());
        Bottle* pubs = cmd.get(3).asList();
        result = handleAdminRosPublisherUpdateCmd(topic, pubs);
        reader.requestDrop(); // ROS needs us to close down.
    } break;
    case PortCoreCommand::RosRequestTopic:
        yCDebug(PORTCORE, "requestTopic! --> %s", cmd.toString().c_str());
        // std::string caller_id = cmd.get(1).asString(); // Currently unused
        // std::string topic = RosNameSpace::fromRosName(cmd.get(2).asString()); // Currently unused
        // Bottle protocols = cmd.get(3).asList(); // Currently unused
        result = handleAdminRosRequestTopicCmd();
        reader.requestDrop(); // ROS likes to close down.
        break;
    case PortCoreCommand::RosGetPid:
        // std::string caller_id = cmd.get(1).asString(); // Currently unused
        result = handleAdminRosGetPidCmd();
        reader.requestDrop(); // ROS likes to close down.
        break;
    case PortCoreCommand::RosGetBusInfo:
        // std::string caller_id = cmd.get(1).asString(); // Currently unused
        result = handleAdminRosGetBusInfoCmd();
        reader.requestDrop(); // ROS likes to close down.
        break;
    case PortCoreCommand::Unknown:
        result = handleAdminUnknownCmd(cmd);
        break;
    }

    ConnectionWriter* writer = reader.getWriter();
    if (writer != nullptr) {
        result.write(*writer);
    }

    return true;
}


bool PortCore::setTypeOfService(PortCoreUnit* unit, int tos)
{
    if (unit == nullptr) {
        return false;
    }

    yCDebug(PORTCORE, "Trying to set TOS = %d", tos);

    if (unit->isOutput()) {
        auto* outUnit = dynamic_cast<PortCoreOutputUnit*>(unit);
        if (outUnit != nullptr) {
            OutputProtocol* op = outUnit->getOutPutProtocol();
            if (op != nullptr) {
                yCDebug(PORTCORE, "Trying to set TOS = %d on output unit", tos);
                bool ok = op->getOutputStream().setTypeOfService(tos);
                if (!ok) {
                    yCWarning(PORTCORE, "Setting TOS on output unit failed");
                }
                return ok;
            }
        }
    }

    // Some of the input units may have output stream object to write back to
    // the connection (e.g., tcp ack and reply). Thus the QoS preferences should be
    // also configured for them.


    if (unit->isInput()) {
        auto* inUnit = dynamic_cast<PortCoreInputUnit*>(unit);
        if (inUnit != nullptr) {
            InputProtocol* ip = inUnit->getInPutProtocol();
            if ((ip != nullptr) && ip->getOutput().isOk()) {
                yCDebug(PORTCORE, "Trying to set TOS = %d on input unit", tos);
                bool ok = ip->getOutput().getOutputStream().setTypeOfService(tos);
                if (!ok) {
                    yCWarning(PORTCORE, "Setting TOS on input unit failed");
                }
                return ok;
            }
        }
    }
    // if there is nothing to be set, returns true
    return true;
}

int PortCore::getTypeOfService(PortCoreUnit* unit)
{
    if (unit == nullptr) {
        return -1;
    }

    if (unit->isOutput()) {
        auto* outUnit = dynamic_cast<PortCoreOutputUnit*>(unit);
        if (outUnit != nullptr) {
            OutputProtocol* op = outUnit->getOutPutProtocol();
            if (op != nullptr) {
                return op->getOutputStream().getTypeOfService();
            }
        }
    }

    // Some of the input units may have output stream object to write back to
    // the connection (e.g., tcp ack and reply). Thus the QoS preferences should be
    // also configured for them.


    if (unit->isInput()) {
        auto* inUnit = dynamic_cast<PortCoreInputUnit*>(unit);
        if (inUnit != nullptr) {
            InputProtocol* ip = inUnit->getInPutProtocol();
            if ((ip != nullptr) && ip->getOutput().isOk()) {
                return ip->getOutput().getOutputStream().getTypeOfService();
            }
        }
    }
    return -1;
}

// attach a portmonitor plugin to the port or to a specific connection
bool PortCore::attachPortMonitor(yarp::os::Property& prop, bool isOutput, std::string& errMsg)
{
    // attach to the current port
    Carrier* portmonitor = Carriers::chooseCarrier("portmonitor");
    if (portmonitor == nullptr) {
        errMsg = "Portmonitor carrier modifier cannot be find or it is not enabled in YARP!";
        return false;
    }

    if (isOutput) {
        detachPortMonitor(true);
        prop.put("source", getName());
        prop.put("destination", "");
        prop.put("sender_side", 1);
        prop.put("receiver_side", 0);
        prop.put("carrier", "");
        m_modifier.outputMutex.lock();
        m_modifier.outputModifier = portmonitor;
        if (!m_modifier.outputModifier->configureFromProperty(prop)) {
            m_modifier.releaseOutModifier();
            errMsg = "Failed to configure the portmonitor plug-in";
            m_modifier.outputMutex.unlock();
            return false;
        }
        m_modifier.outputMutex.unlock();
    } else {
        detachPortMonitor(false);
        prop.put("source", "");
        prop.put("destination", getName());
        prop.put("sender_side", 0);
        prop.put("receiver_side", 1);
        prop.put("carrier", "");
        m_modifier.inputMutex.lock();
        m_modifier.inputModifier = portmonitor;
        if (!m_modifier.inputModifier->configureFromProperty(prop)) {
            m_modifier.releaseInModifier();
            errMsg = "Failed to configure the portmonitor plug-in";
            m_modifier.inputMutex.unlock();
            return false;
        }
        m_modifier.inputMutex.unlock();
    }
    return true;
}

// detach the portmonitor from the port or specific connection
bool PortCore::detachPortMonitor(bool isOutput)
{
    if (isOutput) {
        m_modifier.outputMutex.lock();
        m_modifier.releaseOutModifier();
        m_modifier.outputMutex.unlock();
    } else {
        m_modifier.inputMutex.lock();
        m_modifier.releaseInModifier();
        m_modifier.inputMutex.unlock();
    }
    return true;
}

bool PortCore::setParamPortMonitor(const yarp::os::Property& param,
                                   bool isOutput,
                                   std::string& errMsg)
{
    if (isOutput) {
        m_modifier.outputMutex.lock();
        if (m_modifier.outputModifier == nullptr) {
            errMsg = "No port modifier is attached to the output";
            m_modifier.outputMutex.unlock();
            return false;
        }
        m_modifier.outputModifier->setCarrierParams(param);
        m_modifier.outputMutex.unlock();
    } else {
        m_modifier.inputMutex.lock();
        if (m_modifier.inputModifier == nullptr) {
            errMsg = "No port modifier is attached to the input";
            m_modifier.inputMutex.unlock();
            return false;
        }
        m_modifier.inputModifier->setCarrierParams(param);
        m_modifier.inputMutex.unlock();
    }
    return true;
}

bool PortCore::getParamPortMonitor(yarp::os::Property& param,
                                   bool isOutput,
                                   std::string& errMsg)
{
    if (isOutput) {
        m_modifier.outputMutex.lock();
        if (m_modifier.outputModifier == nullptr) {
            errMsg = "No port modifier is attached to the output";
            m_modifier.outputMutex.unlock();
            return false;
        }
        m_modifier.outputModifier->getCarrierParams(param);
        m_modifier.outputMutex.unlock();
    } else {
        m_modifier.inputMutex.lock();
        if (m_modifier.inputModifier == nullptr) {
            errMsg = "No port modifier is attached to the input";
            m_modifier.inputMutex.unlock();
            return false;
        }
        m_modifier.inputModifier->getCarrierParams(param);
        m_modifier.inputMutex.unlock();
    }
    return true;
}

void PortCore::reportUnit(PortCoreUnit* unit, bool active)
{
    YARP_UNUSED(active);
    if (unit != nullptr) {
        bool isLog = (!unit->getMode().empty());
        if (isLog) {
            m_logNeeded = true;
        }
    }
}

bool PortCore::setProcessSchedulingParam(int priority, int policy)
{
#if defined(__linux__)
    // set the sched properties of all threads within the process
    struct sched_param sch_param;
    sch_param.__sched_priority = priority;

    DIR* dir;
    char path[PATH_MAX];
    sprintf(path, "/proc/%d/task/", yarp::os::impl::getpid());

    dir = opendir(path);
    if (dir == nullptr) {
        return false;
    }

    struct dirent* d;
    char* end;
    long tid = 0;
    bool ret = true;
    while ((d = readdir(dir)) != nullptr) {
        if (isdigit(static_cast<unsigned char>(*d->d_name)) == 0) {
            continue;
        }

        tid = strtol(d->d_name, &end, 10);
        if (d->d_name == end || ((end != nullptr) && (*end != 0))) {
            closedir(dir);
            return false;
        }
        ret &= (sched_setscheduler(static_cast<pid_t>(tid), policy, &sch_param) == 0);
    }
    closedir(dir);
    return ret;
#elif defined(YARP_HAS_ACE) // for other platforms
    // TODO: Check how to set the scheduling properties of all process's threads in Windows
    ACE_Sched_Params param(policy, (ACE_Sched_Priority)priority, ACE_SCOPE_PROCESS);
    int ret = ACE_OS::set_scheduling_params(param, yarp::os::impl::getpid());
    return (ret != -1);
#else
    return false;
#endif
}

Property* PortCore::acquireProperties(bool readOnly)
{
    m_stateMutex.lock();
    if (!readOnly) {
        if (m_prop == nullptr) {
            m_prop = new Property();
        }
    }
    return m_prop;
}

void PortCore::releaseProperties(Property* prop)
{
    YARP_UNUSED(prop);
    m_stateMutex.unlock();
}

bool PortCore::removeIO(const Route& route, bool synch)
{
    return removeUnit(route, synch);
}

void PortCore::setName(const std::string& name)
{
    m_name = name;
}

std::string PortCore::getName()
{
    return m_name;
}

int PortCore::getNextIndex()
{
    int result = m_counter;
    m_counter++;
    if (m_counter < 0) {
        m_counter = 1;
    }
    return result;
}

const Contact& PortCore::getAddress() const
{
    return m_address;
}

void PortCore::resetPortName(const std::string& str)
{
    m_address.setName(str);
}

yarp::os::PortReaderCreator* PortCore::getReadCreator()
{
    return m_readableCreator;
}

void PortCore::setControlRegistration(bool flag)
{
    m_controlRegistration = flag;
}

bool PortCore::isListening() const
{
    return m_listening.load();
}

bool PortCore::isManual() const
{
    return m_manual;
}

bool PortCore::isInterrupted() const
{
    return m_interrupted;
}

void PortCore::setTimeout(float timeout)
{
    m_timeout = timeout;
}

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
bool PortCore::setCallbackLock(yarp::os::Mutex* mutex)
{
    removeCallbackLock();
    if (mutex != nullptr) {
        m_old_mutex = mutex;
        m_mutexOwned = false;
    } else {
        m_old_mutex = new yarp::os::Mutex();
        m_mutexOwned = true;
    }
    return true;
}
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

bool PortCore::setCallbackLock(std::mutex* mutex)
{
    removeCallbackLock();
    if (mutex != nullptr) {
        m_mutex = mutex;
        m_mutexOwned = false;
    } else {
        m_mutex = new std::mutex;
        m_mutexOwned = true;
    }
    return true;
}

bool PortCore::removeCallbackLock()
{
    if (m_mutexOwned && (m_mutex != nullptr)) {
        delete m_mutex;
    }
    m_mutex = nullptr;
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
    m_old_mutex = nullptr;
#endif // YARP_NO_DEPRECATED
    m_mutexOwned = false;
    return true;
}

bool PortCore::lockCallback()
{
    if (m_mutex == nullptr) {
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
        if (m_old_mutex == nullptr) {
            return false;
        }
        m_old_mutex->lock();
        return true;
#else // YARP_NO_DEPRECATED
        return false;
#endif // YARP_NO_DEPRECATED
    }
    m_mutex->lock();
    return true;
}

bool PortCore::tryLockCallback()
{
    if (m_mutex == nullptr) {
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
        if (m_old_mutex == nullptr) {
            return true;
        }
        return m_old_mutex->try_lock();
#else // YARP_NO_DEPRECATED
        return true;
#endif // YARP_NO_DEPRECATED
    }
    return m_mutex->try_lock();
}

void PortCore::unlockCallback()
{
    if (m_mutex == nullptr) {
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
        if (m_old_mutex == nullptr) {
            return;
        }
        return m_old_mutex->unlock();
#else // YARP_NO_DEPRECATED
        return;
#endif // YARP_NO_DEPRECATED
    }
    m_mutex->unlock();
}

yarp::os::impl::PortDataModifier& PortCore::getPortModifier()
{
    return m_modifier;
}

void PortCore::checkType(PortReader& reader)
{
    m_typeMutex.lock();
    if (!m_checkedType) {
        if (!m_type.isValid()) {
            m_type = reader.getReadType();
        }
        m_checkedType = true;
    }
    m_typeMutex.unlock();
}

yarp::os::Type PortCore::getType()
{
    m_typeMutex.lock();
    Type t = m_type;
    m_typeMutex.unlock();
    return t;
}

void PortCore::promiseType(const Type& typ)
{
    m_typeMutex.lock();
    m_type = typ;
    m_typeMutex.unlock();
}
