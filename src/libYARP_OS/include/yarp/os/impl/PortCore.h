// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PORTCORE_
#define _YARP2_PORTCORE_

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/PortManager.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/impl/PortCorePacket.h>

#include <yarp/os/PortReport.h>
#include <yarp/os/Property.h>

#include <yarp/os/impl/PlatformVector.h>

namespace yarp {
    namespace os {
        /**
         *
         * The components from which ports and connections are built.
         * These classes are not intended for regular users, but rather
         * for those extending YARP to new situations.
         * Unlike the classes in yarp::os, yarp::sig, and yarp::dev,
         * there are dependencies on the ACE library here.
         *
         */
        namespace impl {
            class PortCore;
            class PortCoreUnit;
        }
    }
}

#define PORTCORE_SEND_NORMAL (1)
#define PORTCORE_SEND_LOG (2)

// some flags for restricting port behavior
#define PORTCORE_IS_NULL (0)
#define PORTCORE_IS_RPC (1)
#define PORTCORE_IS_INPUT (2)
#define PORTCORE_IS_OUTPUT (4)

/**
 * This is the heart of a yarp port.  It is the thread manager.
 * All other port components are insulated from threading.
 * It maintains a collection of incoming and
 * outgoing connections.  Data coming from incoming connections is
 * directed to the handler set with setReadHandler().  Calls to send()
 * result in data being sent to all the outgoing connections.
 * This class is used to construct yarp::os::Port, which in turn is
 * used to construct yarp::os::BufferedPort and several other port
 * variants.
 *
 * The port's phase in its lifecycle is reflected by flags as follows.
 *
 * ### PortCore()
 * The port is constructed in an inactive, dormant state.
 * All flags off initially.
 *   * -listening -running -starting -closing -finished -finishing
 *
 * ### listen()
 * A call to listen puts the port on the network
 *   * +listening (server now exists)
 *
 * ### start()
 * A call to start creates a thread to serve network requests
 *   * +starting
 *   * +running   (set in server thread)
 *   * -starting  (set in server thread)
 *
 * ### close()
 * A call to close winds everything down
 *   * +finishing
 *   * +closing
 *   * +finished  (set in server thread)
 *   * -listening -running -starting -closing -finished -finishing
 *
 * It is possible to create a port without creating a server for
 * it, by using manualStart() rather than start().  Such ports
 * don't get registered and are not reachable on the network, but
 * can interact with other ports.
 *
 */
class YARP_OS_impl_API yarp::os::impl::PortCore : public ThreadImpl, public PortManager, public yarp::os::PortReader {
public:

    /**
     * Constructor.
     */
    PortCore() : stateMutex(1), packetMutex(1), connectionChange(1),
                 log("port",Logger::get()) {
        // dormant phase
        listening = false;
        running = false;
        starting = false;
        closing = false;
        finished = false;
        finishing = false;
        manual = false;
        autoHandshake = true;
        waitBeforeSend = waitAfterSend = true;
        connectionListeners = 0;
        events = 0;
        face = NULL;
        reader = NULL;
        readableCreator = NULL;
        outputCount = inputCount = 0;
        dataOutputCount = 0;
        controlRegistration = true;
        interruptible = true;
        interrupted = false;
        eventReporter = NULL;
        logNeeded = false;
        flags = PORTCORE_IS_INPUT|PORTCORE_IS_OUTPUT;
        timeout = -1;
        verbosity = 1;
        counter = 1;
        prop = NULL;
        contactable = NULL;
    }

    /**
     * Destructor.
     */
    virtual ~PortCore();

    /**
     * Configure the port to meet certain restrictions in behavior.
     */
    void setFlags(int flags) {
        this->flags = flags;
    }

    void setContactable(Contactable *contactable) {
        this->contactable = contactable;
    }

    /**
     * Check current configuration of port.
     */
    int getFlags() {
        return flags;
    }

    /**
     * Begin service at a given address.
     */
    bool listen(const Contact& address, bool shouldAnnounce = true);

    /**
     * Check if a message is currently being sent.
     */
    bool isWriting();

    /**
     * Check how many input connections there are.
     */
    int getInputCount();

    /**
     * Check how many output connections there are.
     */
    int getOutputCount();

    /**
     * Set a callback for incoming data.
     */
    void setReadHandler(yarp::os::PortReader& reader);

    /**
     * Set a callback for creating callbacks for incoming data.
     */
    void setReadCreator(yarp::os::PortReaderCreator& creator);

    /**
     * Usually this class will handle "handshaking" - establishing
     * what kind of carrier is in use on a given connection.
     * This can optionally be suppressed.
     */
    void setAutoHandshake(bool autoHandshake) {
        this->autoHandshake = autoHandshake;
    }

    /**
     * Upon being asked to send a message, should we wait for
     * any existing message to be sent to all destinations?
     */
    void setWaitBeforeSend(bool waitBeforeSend) {
        this->waitBeforeSend = waitBeforeSend;
    }

    /**
     * After sending a message, should we wait for
     * it to be sent to all destinations before returning?
     */
    void setWaitAfterSend(bool waitAfterSend) {
        this->waitAfterSend = waitAfterSend;
    }

    /**
     * Callback for data.
     */
    virtual bool read(yarp::os::ConnectionReader& reader) {
        // does nothing by default
        return true;
    }

    /**
     * Begin main thread.
     */
    virtual bool start();


    /**
     * Start up the port, but without a main thread.
     */
    bool manualStart(const char *sourceName);

    /**
     * Send a normal message.
     * @param writer the message
     * @param reader where to direct replies
     * @param callback who to call onCompletion() on when message sent.
     */
    bool send(yarp::os::PortWriter& writer,
              yarp::os::PortReader *reader = NULL,
              yarp::os::PortWriter *callback = NULL);

    /**
     * Send a message with a specific mode (normal or log).
     * @param writer the message
     * @param reader where to direct replies
     * @param callback who to call onCompletion() on when message sent.
     */
    bool sendHelper(yarp::os::PortWriter& writer,
                    int mode,
                    yarp::os::PortReader *reader = NULL,
                    yarp::os::PortWriter *callback = NULL);

    /**
     * Shut down port.
     */
    virtual void close();

    /**
     * The body of the main thread.
     */
    virtual void run();

    /**
     * A diagnostic for testing purposes.
     */
    int getEventCount();

    /**
     * Get the address associated with the port.
     */
    const Contact& getAddress() const {
        return address;
    }

    void resetPortName(const String& str) {
        address = address.addName(str);
    }

    /**
     * Get the creator of callbacks.
     */
    yarp::os::PortReaderCreator *getReadCreator() {
        return readableCreator;
    }

    /**
     * Call the right onCompletion() after sending message
     */
    void notifyCompletion(void *tracker);

    /**
     * Set some extra meta data to pass along with the message
     */
    bool setEnvelope(yarp::os::PortWriter& envelope);

    /**
     * Set some extra meta data to pass along with the message
     */
    void setEnvelope(const String& envelope);

    String getEnvelope();

    /**
     * Get any meta data associated with the last message received
     */
    bool getEnvelope(yarp::os::PortReader& envelope);

    /**
     * Normally the port will unregister its name with the name server
     * when shutting down.  This can be inhibited.
     */
    void setControlRegistration(bool flag) {
        controlRegistration = flag;
    }

    /**
     * Prepare the port to be shut down.
     */
    void interrupt();

    /**
     * Undo an interrupt()
     */
    void resume();

    /**
     * Generate a description of the connections associated with the
     * port.
     */
    virtual void describe(yarp::os::PortReport& reporter);

    /**
     * Set a callback to be notified of changes in port status.
     */
    void setReportCallback(yarp::os::PortReport *reporter);

    /**
     * Process an administrative message.
     */
    bool adminBlock(yarp::os::ConnectionReader& reader, void *id,
                    OutputStream *os);


    bool isListening() const {
        return listening;
    }

    bool isManual() const {
        return manual;
    }

    bool isInterrupted() const {
        return interrupted;
    }

public:

    // PortManager interface, exposed to inputs

    virtual bool addOutput(const String& dest, void *id, OutputStream *os,
                           bool onlyIfNeeded);
    virtual void removeOutput(const String& dest, void *id, OutputStream *os);
    virtual void removeInput(const String& dest, void *id, OutputStream *os);
    virtual void describe(void *id, OutputStream *os);
    virtual bool readBlock(yarp::os::ConnectionReader& reader, void *id, OutputStream *os);


    /**
     * Generate a description of the connections associated with the
     * port.
     */
    virtual void report(const yarp::os::PortInfo& info);

    /**
     * Add another output to the port.
     */
    void addOutput(OutputProtocol *op);

    virtual bool removeIO(const Route& route, bool synch) {
        return removeUnit(route,synch);
    }

    virtual void reportUnit(PortCoreUnit *unit, bool active);

    void setTimeout(float timeout) {
        this->timeout = timeout;
    }

    void setVerbosity(int level) {
        verbosity = level;
    }

    int getVerbosity() {
        return verbosity;
    }

    Property *acquireProperties(bool readOnly);
    void releaseProperties(Property *prop);

private:

    // internal maintenance of sub units

    PlatformVector<PortCoreUnit *> units;

    // only called in "finished" phase
    void closeUnits();

    // called anytime, garbage collects terminated units
    void cleanUnits(bool blocking = true);

    // only called by the manager
    void reapUnits();

    // only called in "running" phase
    void addInput(InputProtocol *ip);

    bool removeUnit(const Route& route, bool synch = false,
                    bool *except = NULL);

    int getNextIndex() {
        int result = counter;
        counter++;
        if (counter<0) counter = 1;
        return result;
    }

private:

    // main internal PortCore state and operations
    SemaphoreImpl stateMutex;       ///< control access to essential port state
    SemaphoreImpl packetMutex;      ///< control access to message cache
    SemaphoreImpl connectionChange; ///< signal changes in connections
    Logger log;  ///< message logger
    Face *face;  ///< network server
    String name; ///< name of port
    yarp::os::Contact address;    ///< network address of port
    yarp::os::PortReader *reader; ///< where to send read events
    yarp::os::PortReaderCreator *readableCreator; ///< factory for readers
    yarp::os::PortReport *eventReporter; ///< where to send general events
    bool listening; ///< is the port server listening on the network?
    bool running;   ///< is the port server thread running?
    bool starting;  ///< is the port in its startup phase?
    bool closing;   ///< is the port in its closing phase?
    bool finished;  ///< is the port server thread finished running?
    bool autoHandshake;  ///< should we automatically negotiate carriers for connections
    bool finishing; ///< is the port server thread trying to finish?
    bool waitBeforeSend; ///< should we wait for all current writes to complete before writing more?
    bool waitAfterSend;  ///< should we wait for writes to complete immediately after we start them?
    bool controlRegistration;  ///< should the port unregister its name when shutting down?
    bool interruptible;  ///< is the port in an interruptible state?
    bool interrupted;    ///< is the port interrupted?
    bool manual;    ///< is the port operating without a server?
    int events;     ///< count of events that have occurred on the port
    int connectionListeners;  ///< how many threads need notification of connection changes
    int inputCount; ///< how many input connections do we have
    int outputCount;///< how many output connections do we have
    int dataOutputCount; ///< how many regular data output connections do we have
    int flags;      ///< binary flags encoding restrictions on port
    int verbosity;  ///< threshold on what warnings or debug messages are shown
    bool logNeeded; ///< port needs to monitor message content
    PortCorePackets packets; ///< a pool for tracking messages currently being sent
    String envelope;///< user-defined wrapping data
    float timeout;  ///< a timeout to apply to all network operations
    int counter;    ///< port-unique ids for connections
    yarp::os::Property *prop;  ///< optional unstructured properties associated with port
    yarp::os::Contactable *contactable;  ///< user-facing object that contains this PortCore

    void closeMain();

    bool isUnit(const Route& route, int index);
};

#endif
