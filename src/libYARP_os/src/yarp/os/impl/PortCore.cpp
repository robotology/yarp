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
#include <yarp/os/PortInfoData.h>
#include <yarp/os/ThreadInfoData.h>

#include <yarp/os/PortCoreMsgs.h>

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
    yCIDebug(PORTCORE, getName(), "Starting listening on %s", address.toURI().c_str());
    // If we're using ACE, we really need to have it initialized before
    // this point.
    if (!NetworkBase::initialized()) {
        yCIError(PORTCORE, getName(), "YARP not initialized; create a yarp::os::Network object before using ports");
        return false;
    }

    yCITrace(PORTCORE, getName(), "listen");

    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);

        // This method assumes we are not already on the network.
        // We can assume this because it is not a user-facing class,
        // and we carefully never call this method again without
        // calling close().
        yCIAssert(PORTCORE, getName(), !m_listening);
        yCIAssert(PORTCORE, getName(), !m_running);
        yCIAssert(PORTCORE, getName(), !m_closing.load());
        yCIAssert(PORTCORE, getName(), !m_finished.load());
        yCIAssert(PORTCORE, getName(), m_face == nullptr);

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
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), m_reader == nullptr);
    m_reader = &reader;
}

void PortCore::setAdminReadHandler(PortReader& reader)
{
    // Don't even try to do this when the port is hot, it'll burn you
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), m_adminReader == nullptr);
    m_adminReader = &reader;
}

void PortCore::setReadCreator(PortReaderCreator& creator)
{
    // Don't even try to do this when the port is hot, it'll burn you
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), m_readableCreator == nullptr);
    m_readableCreator = &creator;
}


void PortCore::run()
{
    yCITrace(PORTCORE, getName(), "run");

    // This is the server thread for the port.  We listen on
    // the network and handle any incoming connections.
    // We don't touch those connections, just shove them
    // in a list and move on.  It is important that this
    // thread doesn't make a connecting client wait just
    // because some other client is slow.

    // We assume that listen() has succeeded and that
    // start() has been called.
    yCIAssert(PORTCORE, getName(), m_listening.load());
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), !m_closing.load());
    yCIAssert(PORTCORE, getName(), !m_finished.load());
    yCIAssert(PORTCORE, getName(), m_starting.load());

    // Enter running phase
    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_running.store(true);
        m_starting.store(false);
    }

    // Notify the start() thread that the run() thread is running
    m_stateCv.notify_one();

    yCITrace(PORTCORE, getName(), "run running");

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
                yCIDebug(PORTCORE, getName(), "received something");
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
            yCIDebug(PORTCORE, getName(), "spun off a connection");
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

    yCITrace(PORTCORE, getName(), "run closing");

    // The server thread is shutting down.
    std::lock_guard<std::mutex> lock(m_stateMutex);
    m_connectionListeners = 0;
    m_connectionChangeCv.notify_all();
    m_finished.store(true);
}


void PortCore::close()
{
    closeMain();

    m_modifier.releaseOutModifier();
    m_modifier.releaseInModifier();
}


bool PortCore::start()
{
    yCITrace(PORTCORE, getName(), "start");

    // This wait will, on success, be matched by a post in run()
    std::unique_lock<std::mutex> lock(m_stateMutex);

    // We assume that listen() has been called.
    yCIAssert(PORTCORE, getName(), m_listening.load());
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), !m_starting.load());
    yCIAssert(PORTCORE, getName(), !m_finished.load());
    yCIAssert(PORTCORE, getName(), !m_closing.load());

    m_starting.store(true);

    // Start the server thread.
    bool started = ThreadImpl::start();
    if (!started) {
        // run() won't be happening
        yAssert(false);

    } else {
        // Wait for the signal from the run thread before returning.
        m_stateCv.wait(lock, [&]{ return m_running.load(); });
        yCIAssert(PORTCORE, getName(), m_running.load());
    }
    return started;
}


bool PortCore::manualStart(const char* sourceName)
{
    yCITrace(PORTCORE, getName(), "manualStart");

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
    yCITrace(PORTCORE, getName(), "interrupt");

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
            yCIDebug(PORTCORE, getName(), "sending update-state message to listener");
            StreamConnectionReader sbr;
            lockCallback();
            m_reader->read(sbr);
            unlockCallback();
        }
    }
}


void PortCore::closeMain()
{
    yCITrace(PORTCORE, getName(), "closeMain");

    {
        // Critical section
        std::lock_guard<std::mutex> lock(m_stateMutex);

        // We may not have anything to do.
        if (m_finishing || !(m_running.load() || m_manual)) {
            yCITrace(PORTCORE, getName(), "closeMain - nothing to do");
            return;
        }

        yCITrace(PORTCORE, getName(), "closeMain - Central");

        // Move into official "finishing" phase.
        m_finishing = true;
        yCIDebug(PORTCORE, getName(), "now preparing to shut down port");
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
            yCIDebug(PORTCORE, getName(), "requesting removal of connection from %s", removeName.c_str());
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
        yCIAssert(PORTCORE, getName(), m_finished.load());

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
        yCIAssert(PORTCORE, getName(), m_face != nullptr);
        m_face->close();
        delete m_face;
        m_face = nullptr;
        m_listening.store(false);
    }

    // Check if the client is waiting for input.  If so, wake them up
    // with the bad news.  An empty message signifies a request to
    // check the port state.
    if (m_reader != nullptr) {
        yCIDebug(PORTCORE, getName(), "sending end-of-port message to listener");
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
    yCIAssert(PORTCORE, getName(), !m_listening.load());
    yCIAssert(PORTCORE, getName(), !m_running.load());
    yCIAssert(PORTCORE, getName(), !m_starting.load());
    yCIAssert(PORTCORE, getName(), !m_closing.load());
    yCIAssert(PORTCORE, getName(), !m_finished.load());
    yCIAssert(PORTCORE, getName(), !m_finishing);
    yCIAssert(PORTCORE, getName(), m_face == nullptr);
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
    yCIAssert(PORTCORE, getName(), m_finished.load());

    // In the "finished" phase, nobody else touches the units,
    // so we can go ahead and shut them down and delete them.
    for (auto& i : m_units) {
        PortCoreUnit* unit = i;
        if (unit != nullptr) {
            yCIDebug(PORTCORE, getName(), "closing a unit");
            unit->close();
            yCIDebug(PORTCORE, getName(), "joining a unit");
            unit->join();
            delete unit;
            yCIDebug(PORTCORE, getName(), "deleting a unit");
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
                yCIDebug(PORTCORE, getName(), "Informing connection %s that it is doomed", s.c_str());
                unit->close();
                yCIDebug(PORTCORE, getName(), "Closed connection %s", s.c_str());
                unit->join();
                yCIDebug(PORTCORE, getName(), "Joined thread of connection %s", s.c_str());
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
    yCIDebug(PORTCORE, getName(), "/ routine check of connections to this port begins");
    if (!m_finished.load()) {

        // First, we delete and null out any defunct entries in the list.
        for (auto& i : m_units) {
            PortCoreUnit* unit = i;
            if (unit != nullptr) {
                yCIDebug(PORTCORE, getName(), "| checking connection %s %s", unit->getRoute().toString().c_str(), unit->getMode().c_str());
                if (unit->isFinished()) {
                    std::string con = unit->getRoute().toString();
                    yCIDebug(PORTCORE, getName(), "|   removing connection %s", con.c_str());
                    unit->close();
                    unit->join();
                    delete unit;
                    i = nullptr;
                    yCIDebug(PORTCORE, getName(), "|   removed connection %s", con.c_str());
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
    yCIDebug(PORTCORE, getName(), "\\ routine check of connections to this port ends");
}


void PortCore::addInput(InputProtocol* ip)
{
    yCITrace(PORTCORE, getName(), "addInput");

    // This method is only called by the server thread in its running phase.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will have its own thread associated
    // with it.

    yCIAssert(PORTCORE, getName(), ip != nullptr);
    std::lock_guard<std::mutex> lock(m_stateMutex);
    PortCoreUnit* unit = new PortCoreInputUnit(*this,
                                               getNextIndex(),
                                               ip,
                                               false);
    yCIAssert(PORTCORE, getName(), unit != nullptr);
    unit->start();
    m_units.push_back(unit);
    yCITrace(PORTCORE, getName(), "there are now %zu units", m_units.size());
}


void PortCore::addOutput(OutputProtocol* op)
{
    yCITrace(PORTCORE, getName(), "addOutput");

    // This method is called from threads associated with input
    // connections.
    // It wraps up a network connection as a unit and adds it to
    // PortCore#units.  The unit will start with its own thread
    // associated with it, but that thread will be very short-lived
    // if the port is not configured to do background writes.

    yCIAssert(PORTCORE, getName(), op != nullptr);
    if (!m_finished.load()) {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        PortCoreUnit* unit = new PortCoreOutputUnit(*this, getNextIndex(), op);
        yCIAssert(PORTCORE, getName(), unit != nullptr);
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
        yCIDebug(PORTCORE, getName(), "asked to remove connection in the way of %s", route.toString().c_str());
        *except = false;
    } else {
        yCIDebug(PORTCORE, getName(), "asked to remove connection %s", route.toString().c_str());
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
                    yCIDebug(PORTCORE, getName(), "removing connection %s", alt.toString().c_str());
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
        yCIDebug(PORTCORE, getName(), "one or more connections need prodding to die");

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
            yCIDebug(PORTCORE, getName(), "sent message to prod connection death");

            if (synch) {
                // Wait for connections to be cleaned up.
                yCIDebug(PORTCORE, getName(), "synchronizing with connection death");
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
    yCIDebug(PORTCORE, getName(), "asked to add output to %s", dest.c_str());

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
            yCIDebug(PORTCORE, getName(), "output already present to %s", dest.c_str());
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
            yCIDebug(PORTCORE, getName(), "open route error");
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
            yCIAssert(PORTCORE, getName(), unit != nullptr);
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


bool PortCore::removeOutput(const std::string& dest, void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    yCIDebug(PORTCORE, getName(), "asked to remove output to %s", dest.c_str());

    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    const bool removed = removeUnit(Route("*", dest, "*"), true);
    if (removed) {
        bw.appendLine(std::string("Removed connection from ") + getName() + " to " + dest);
    } else {
        bw.appendLine(std::string("Could not find an outgoing connection to ") + dest);
    }
    if (os != nullptr) {
        bw.write(*os);
    }
    cleanUnits();
    return removed;
}

bool PortCore::removeInput(const std::string& src, void* id, OutputStream* os)
{
    YARP_UNUSED(id);
    yCIDebug(PORTCORE, getName(), "asked to remove input to %s", src.c_str());

    // All the real work done by removeUnit().
    BufferedConnectionWriter bw(true);
    const bool removed = removeUnit(Route(src, "*", "*"), true);
    if (removed) {
        bw.appendLine(std::string("Removing connection from ") + src + " to " + getName());
    } else {
        bw.appendLine(std::string("Could not find an incoming connection from ") + src);
    }
    if (os != nullptr) {
        bw.write(*os);
    }
    cleanUnits();
    return removed;
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
        m_interruptable = false; // No mutex; user of interrupt() has to be careful.

        bool haveOutputs = (m_outputCount != 0); // No mutex, but failure modes are benign.

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
        yCIDebug(PORTCORE, getName(), "data received, no reader for it");
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

    yCITrace(PORTCORE, getName(), "------- send in real");

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

    yCITrace(PORTCORE, getName(), "------- send in");
    // Prepare a "packet" for tracking a single message which
    // may travel by multiple outputs.
    m_packetMutex.lock();
    PortCorePacket* packet = m_packets.getFreePacket();
    yCIAssert(PORTCORE, getName(), packet != nullptr);
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
            yCITrace(PORTCORE, getName(), "------- -- inc");
            m_packetMutex.lock();
            packet->inc(); // One more connection carrying message.
            m_packetMutex.unlock();
            yCITrace(PORTCORE, getName(), "------- -- pre-send");
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
            yCITrace(PORTCORE, getName(), "------- -- send");
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
            yCITrace(PORTCORE, getName(), "------- -- dec");
        }
    }
    yCITrace(PORTCORE, getName(), "------- pack check");
    m_packetMutex.lock();

    // We no longer concern ourselves with the message.
    // It may or may not be traveling on some connections.
    // But that is not our problem anymore.
    packet->dec();

    m_packets.checkPacket(packet);
    m_packetMutex.unlock();
    yCITrace(PORTCORE, getName(), "------- packed");
    yCITrace(PORTCORE, getName(), "------- send out");
    if (mode == PORTCORE_SEND_LOG) {
        if (logCount == 0) {
            m_logNeeded = false;
        }
    }

    yCITrace(PORTCORE, getName(), "------- send out real");

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
    yCITrace(PORTCORE, getName(), "starting notifyCompletion");
    m_packetMutex.lock();
    if (tracker != nullptr) {
        (static_cast<PortCorePacket*>(tracker))->dec();
        m_packets.checkPacket(static_cast<PortCorePacket*>(tracker));
    }
    m_packetMutex.unlock();
    yCITrace(PORTCORE, getName(), "stopping notifyCompletion");
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
    yCIDebug(PORTCORE, getName(), "set envelope to %s", m_envelope.c_str());
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

    yCIDebug(PORTCORE, getName(), "Port %s received command %s", getName().c_str(), cmd.toString().c_str());

    // Server side of the commands defined in idl/PortCoreMsgs.thrift.
    // NOTE: being a local class, it has the same access rights of adminBlock().
    class PortCoreMsgsServer : public yarp::os::PortCoreMsgs
    {
        PortCore& m_owner;
        void* m_id; // the id of the unit which sent the command

        bool attach(const std::string& portmonitor_options, bool isOutput)
        {
            std::string errMsg;
            if (!m_owner.attachPortMonitor(portmonitor_options, isOutput, errMsg)) {
                yCIError(PORTCORE, m_owner.getName(), "%s", errMsg.c_str());
                return false;
            }
            return true;
        }

        return_listConnections listConnections(bool isInput)
        {
            return_listConnections result;
            result.ret = true;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            for (auto* unit : m_owner.m_units) {
                if ((unit != nullptr) && (isInput ? unit->isInput() : unit->isOutput()) && !unit->isFinished()) {
                    const Route route = unit->getRoute();
                    const std::string& name = isInput ? route.getFromName() : route.getToName();
                    if (!name.empty()) {
                        result.connections.push_back(name);
                    }
                }
            }
            return result;
        }

        return_getConnectionInfo getConnectionInfo(bool isInput, const std::string& portname)
        {
            return_getConnectionInfo result;
            result.ret = true;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            for (auto* unit : m_owner.m_units) {
                if ((unit != nullptr) && (isInput ? unit->isInput() : unit->isOutput()) && !unit->isFinished()) {
                    const Route route = unit->getRoute();
                    if ((isInput ? route.getFromName() : route.getToName()) == portname) {
                        result.found = true;
                        result.from_port = route.getFromName();
                        result.to_port = route.getToName();
                        result.carrier = route.getCarrierName();
                        Carrier* carrier = Carriers::chooseCarrier(route.getCarrierName());
                        if (carrier != nullptr) {
                            result.connectionless = carrier->isConnectionless();
                            result.push = carrier->isPush();
                            delete carrier;
                        }
                        break;
                    }
                }
            }
            return result;
        }

        return_setPortmonitorParams setPortmonitorParams(const std::string& params, bool isOutput)
        {
            return_setPortmonitorParams result;
            result.ret = true;
            Property property;
            property.fromString(params);
            result.success = m_owner.setParamPortMonitor(property, isOutput, result.message);
            return result;
        }

        return_getPortmonitorParams getPortmonitorParams(bool isOutput)
        {
            return_getPortmonitorParams result;
            result.ret = true;
            Property property;
            result.success = m_owner.getParamPortMonitor(property, isOutput, result.message);
            if (result.success) {
                result.params = property.toString();
            }
            return result;
        }

        // Returns the unit of the (active) connection coming from (isInput) or
        // going to (!isInput) the given port, or nullptr if there is none.
        // NOTE: m_owner.m_stateMutex must be locked by the caller.
        PortCoreUnit* findConnectionUnit(bool isInput, const std::string& portname)
        {
            for (auto* unit : m_owner.m_units) {
                if ((unit != nullptr) && (isInput ? unit->isInput() : unit->isOutput()) && !unit->isFinished()) {
                    const Route route = unit->getRoute();
                    if ((isInput ? route.getFromName() : route.getToName()) == portname) {
                        return unit;
                    }
                }
            }
            return nullptr;
        }

        std::string connectionNotFoundMessage(bool isInput, const std::string& portname)
        {
            return isInput ? "Could not find an incoming connection from " + portname
                           : "Could not find an outgoing connection to " + portname;
        }

        return_setConnectionParams setConnectionParams(bool isInput, const std::string& portname, const std::string& params)
        {
            return_setConnectionParams result;
            result.ret = true;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            PortCoreUnit* unit = findConnectionUnit(isInput, portname);
            if (unit == nullptr) {
                result.success = false;
                result.message = connectionNotFoundMessage(isInput, portname);
                return result;
            }
            Property property;
            property.fromString(params);
            unit->setCarrierParams(property);
            result.success = true;
            result.message = (isInput ? "Configured connection from " : "Configured connection to ") + portname;
            return result;
        }

        return_getConnectionParams getConnectionParams(bool isInput, const std::string& portname)
        {
            return_getConnectionParams result;
            result.ret = true;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            PortCoreUnit* unit = findConnectionUnit(isInput, portname);
            if (unit == nullptr) {
                result.success = false;
                result.message = connectionNotFoundMessage(isInput, portname);
                return result;
            }
            Property property;
            unit->getCarrierParams(property);
            result.success = true;
            result.params = property.toString();
            return result;
        }

        return_getConnectionQos getConnectionQos(bool isInput, const std::string& portname)
        {
            return_getConnectionQos result;
            result.ret = true;
            result.qos_tos = -1;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            PortCoreUnit* unit = findConnectionUnit(isInput, portname);
            if (unit != nullptr) {
                result.found = true;
                result.thread.tid = static_cast<int>(unit->getTid());
                result.thread.priority = unit->getPriority();
                result.thread.policy = unit->getPolicy();
                result.qos_tos = m_owner.getTypeOfService(unit);
            }
            return result;
        }

        return_setConnectionQos setConnectionQos(bool isInput, const std::string& portname, int priority, int policy, int tos)
        {
            // The priority and policy values on Linux are:
            // SCHED_OTHER : policy=0, priority=[0 ..  0]
            // SCHED_FIFO  : policy=1, priority=[1 .. 99]
            // SCHED_RR    : policy=2, priority=[1 .. 99]
            return_setConnectionQos result;
            result.ret = true;
            std::lock_guard<std::mutex> lock(m_owner.m_stateMutex);
            PortCoreUnit* unit = findConnectionUnit(isInput, portname);
            if (unit == nullptr) {
                result.success = false;
                result.message = connectionNotFoundMessage(isInput, portname);
                return result;
            }
            if (unit->setPriority(priority, policy) == -1) {
                result.success = false;
                result.message = "Cannot set the scheduling parameters of the thread of the connection";
                return result;
            }
            // a negative tos means "leave unchanged"
            if (tos >= 0 && !m_owner.setTypeOfService(unit, tos)) {
                result.success = false;
                result.message = "Cannot set the type of service of the connection";
                return result;
            }
            result.success = true;
            return result;
        }

    public:
        PortCoreMsgsServer(PortCore& owner, void* id) : m_owner(owner), m_id(id) {}

        return_addConnection addConnection(const std::string& portname, const std::string& carrier) override
        {
            // Add an output to the port.
            return_addConnection result;
            result.ret = true;
            const std::string output = carrier.empty() ? portname : carrier + ":/" + portname;
            StringOutputStream cache;
            result.success = m_owner.addOutput(output, m_id, &cache, false);
            result.message = cache.toString();
            return result;
        }

        return_delConnection delConnection(const std::string& portname) override
        {
            // Delete any inputs or outputs involving the named port.
            return_delConnection result;
            result.ret = true;
            StringOutputStream cache;
            const bool removedOutput = m_owner.removeOutput(portname, m_id, &cache);
            const std::string outputMessage = cache.toString();
            cache.reset();
            const bool removedInput = m_owner.removeInput(portname, m_id, &cache);
            const std::string inputMessage = cache.toString();
            // The operation succeeds if at least one connection was removed.
            // The reply contains only the message(s) of the removal(s) which
            // actually happened (or both failure messages, if none).
            result.success = removedOutput || removedInput;
            if (removedOutput && !removedInput) {
                result.message = outputMessage;
            } else if (!removedOutput && removedInput) {
                result.message = inputMessage;
            } else {
                result.message = outputMessage + inputMessage;
            }
            return result;
        }

        bool attachPortmonitorOut(const std::string& prop) override { return attach(prop, true); }
        bool attachPortmonitorIn(const std::string& prop) override { return attach(prop, false); }
        bool detachPortmonitorOut() override { return m_owner.detachPortMonitor(true); }
        bool detachPortmonitorIn() override { return m_owner.detachPortMonitor(false); }
        return_setPortmonitorParams setPortmonitorParamsOut(const std::string& params) override { return setPortmonitorParams(params, true); }
        return_setPortmonitorParams setPortmonitorParamsIn(const std::string& params) override { return setPortmonitorParams(params, false); }
        return_getPortmonitorParams getPortmonitorParamsOut() override { return getPortmonitorParams(true); }
        return_getPortmonitorParams getPortmonitorParamsIn() override { return getPortmonitorParams(false); }
        return_setConnectionParams setConnectionParamsOut(const std::string& portname, const std::string& params) override { return setConnectionParams(false, portname, params); }
        return_setConnectionParams setConnectionParamsIn(const std::string& portname, const std::string& params) override { return setConnectionParams(true, portname, params); }
        return_getConnectionParams getConnectionParamsOut(const std::string& portname) override { return getConnectionParams(false, portname); }
        return_getConnectionParams getConnectionParamsIn(const std::string& portname) override { return getConnectionParams(true, portname); }
        return_listConnections listConnectionsIn() override { return listConnections(true); }
        return_listConnections listConnectionsOut() override { return listConnections(false); }
        return_getConnectionInfo getConnectionInfoIn(const std::string& portname) override { return getConnectionInfo(true, portname); }
        return_getConnectionInfo getConnectionInfoOut(const std::string& portname) override { return getConnectionInfo(false, portname); }

        return_getAdminProtocolVersion getAdminProtocolVersion() override
        {
            // Gives a version number for the administrative commands.
            // It is distinct from YARP library versioning.
            return_getAdminProtocolVersion result;
            result.ret = true;
            result.version_major = 1;
            result.version_minor = 2;
            result.version_patch = 4;
            return result;
        }

        return_getPortInfo getPortInfo() override
        {
            return_getPortInfo result;
            result.ret = true;

            const SystemInfo::ProcessInfo processInfo = SystemInfo::getProcessInfo();
            result.process = processInfo;
            if (processInfo.pid == -1) {
                result.process.name = "unknown";
                result.process.arguments = "unknown";
            }
            result.platform = SystemInfo::getPlatformInfo();

            result.thread.tid = static_cast<int>(m_owner.getTid());
            result.thread.priority = m_owner.getPriority();
            result.thread.policy = m_owner.getPolicy();

            const unsigned int flags = m_owner.getFlags();
            result.port.is_input = (flags & PORTCORE_IS_INPUT) != 0;
            result.port.is_output = (flags & PORTCORE_IS_OUTPUT) != 0;
            result.port.is_rpc = (flags & PORTCORE_IS_RPC) != 0;
            result.port.type = m_owner.getType().getName();
            result.port.hostname = m_owner.m_address.getHost();
            return result;
        }

        return_getConnectionQos getConnectionQosIn(const std::string& portname) override { return getConnectionQos(true, portname); }
        return_getConnectionQos getConnectionQosOut(const std::string& portname) override { return getConnectionQos(false, portname); }
        return_setConnectionQos setConnectionQosIn(const std::string& portname, const std::int32_t scheduler_priority, const std::int32_t scheduler_policy, const std::int32_t qos_tos) override { return setConnectionQos(true, portname, scheduler_priority, scheduler_policy, qos_tos); }
        return_setConnectionQos setConnectionQosOut(const std::string& portname, const std::int32_t scheduler_priority, const std::int32_t scheduler_policy, const std::int32_t qos_tos) override { return setConnectionQos(false, portname, scheduler_priority, scheduler_policy, qos_tos); }

        bool setProcessScheduling(const std::int32_t priority, const std::int32_t policy) override
        {
            // set the scheduling parameters of all the threads of the process
            return m_owner.setProcessSchedulingParam(priority, policy);
        }
    };

    auto handleAdminThriftCmd = [this, id](const Bottle& cmd, Bottle& result) {
        // Forward the (already read) command to the thrift service and
        // collect its reply. Returns false if the command is not one of
        // the commands of the service.
        PortCoreMsgsServer server(*this, id);
        DummyConnector con;
        cmd.write(con.getWriter());
        if (!server.read(con.getReader())) {
            return false;
        }
        result.read(con.getReader());
        return true;
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
            result.addString("send help for the list of valid commands");
        }
        return result;
    };

    // All the administrative commands (including "help") are defined in the
    // thrift service PortCoreMsgs. Other commands are forwarded to the admin
    // reader (if any).
    if (!handleAdminThriftCmd(cmd, result)) {
        result = handleAdminUnknownCmd(cmd);
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

    yCIDebug(PORTCORE, getName(), "Trying to set TOS = %d", tos);

    if (unit->isOutput()) {
        auto* outUnit = dynamic_cast<PortCoreOutputUnit*>(unit);
        if (outUnit != nullptr) {
            OutputProtocol* op = outUnit->getOutPutProtocol();
            if (op != nullptr) {
                yCIDebug(PORTCORE, getName(), "Trying to set TOS = %d on output unit", tos);
                bool ok = op->getOutputStream().setTypeOfService(tos);
                if (!ok) {
                    yCIWarning(PORTCORE, getName(), "Setting TOS on output unit failed");
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
                yCIDebug(PORTCORE, getName(), "Trying to set TOS = %d on input unit", tos);
                bool ok = ip->getOutput().getOutputStream().setTypeOfService(tos);
                if (!ok) {
                    yCIWarning(PORTCORE, getName(), "Setting TOS on input unit failed");
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
bool PortCore::attachPortMonitor(const std::string& portmonitor_options, bool isOutput, std::string& errMsg)
{
    // attach to the current port
    Carrier* portmonitor = Carriers::chooseCarrier("portmonitor");
    if (portmonitor == nullptr) {
        errMsg = "Portmonitor carrier modifier cannot be find or it is not enabled in YARP!";
        return false;
    }

    yarp::os::Property prop;
    prop.fromString(portmonitor_options);

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
    m_mutexOwned = false;
    return true;
}

bool PortCore::lockCallback()
{
    if (m_mutex == nullptr) {
        return false;
    }
    m_mutex->lock();
    return true;
}

bool PortCore::tryLockCallback()
{
    if (m_mutex == nullptr) {
        return true;
    }
    return m_mutex->try_lock();
}

void PortCore::unlockCallback()
{
    if (m_mutex == nullptr) {
        return;
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
