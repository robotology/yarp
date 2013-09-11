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
#include <yarp/os/Contact.h>
#include <yarp/os/impl/PortManager.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/impl/PortCorePacket.h>

#include <yarp/os/PortReport.h>

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
 * This class is used to construct yarp::os::Port and yarp::os::BufferedPort.
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
    SemaphoreImpl stateMutex;
    SemaphoreImpl packetMutex;
    SemaphoreImpl connectionChange;
    Logger log;
    Face *face;
    String name;
    yarp::os::Contact address;
    yarp::os::PortReader *reader;
    yarp::os::PortReaderCreator *readableCreator;
    yarp::os::PortReport *eventReporter;
    bool listening, running, starting, closing, finished, autoHandshake;
    bool finishing;
    bool waitBeforeSend, waitAfterSend;
    bool controlRegistration;
    bool interruptible;
    bool interrupted;
    bool manual;
    int events;
    int connectionListeners;
    int inputCount, outputCount, dataOutputCount;
    int flags;
    int verbosity;
    bool logNeeded;
    PortCorePackets packets;
    String envelope;
    float timeout;
    int counter;

    void closeMain();

    bool isUnit(const Route& route, int index);
};

#endif
