// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007, 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTCORE_
#define _YARP2_PORTCORE_

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/PortManager.h>
#include <yarp/Readable.h>
#include <yarp/Writable.h>
#include <yarp/os/impl/PortCorePacket.h>

#include <yarp/os/PortReport.h>

#include <ace/Vector_T.h>

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

#define PORTCORE_SEND_NORMAL 1
#define PORTCORE_SEND_LOG 2

/**
 * This is the heart of a yarp port.  It is the thread manager.
 * All other port components are insulated from threading.
 * It maintains a collection of incoming and
 * outgoing connections.  Data coming from incoming connections is
 * directed to the handler set with setReadHandler().  Calls to send()
 * result in data being sent to all the outgoing connections.
 * This class is used to construct yarp::os::Port and yarp::os::BufferedPort.
 */
class yarp::os::impl::PortCore : public ThreadImpl, public PortManager, public yarp::os::PortReader {
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
        autoHandshake = true;
        waitBeforeSend = waitAfterSend = true;
        connectionListeners = 0;
        events = 0;
        face = NULL;
        reader = NULL;
        readableCreator = NULL;
        outputCount = inputCount = 0;
        controlRegistration = true;
        interruptible = true;
        eventReporter = NULL;
        logNeeded = false;
    }

    /**
     * Destructor.
     */
    virtual ~PortCore();

    /**
     * Begin service at a given address.
     */
    bool listen(const Address& address);

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
    const Address& getAddress() const {
        return address;
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


public:

    // PortManager interface, exposed to inputs

    virtual void addOutput(const String& dest, void *id, OutputStream *os);
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

private:

    // internal maintenance of sub units

    ACE_Vector<PortCoreUnit *> units;

    // only called in "finished" phase
    void closeUnits();

    // called anytime, garbage collects terminated units
    void cleanUnits();

    // only called by the manager
    void reapUnits();

    // only called in "running" phase
    void addInput(InputProtocol *ip);

    bool removeUnit(const Route& route, bool synch = false);

private:

    // main internal PortCore state and operations
    SemaphoreImpl stateMutex, packetMutex, connectionChange;
    Logger log;
    Face *face;
    String name;
    Address address;
    yarp::os::PortReader *reader;
    yarp::os::PortReaderCreator *readableCreator;
    yarp::os::PortReport *eventReporter;
    bool listening, running, starting, closing, finished, autoHandshake;
    bool finishing;
    bool waitBeforeSend, waitAfterSend;
    bool controlRegistration;
    bool interruptible;
    int events;
    int connectionListeners;
    int inputCount, outputCount;
    bool logNeeded;
    PortCorePackets packets;
    String envelope;

    void closeMain();

    bool isUnit(const Route& route);
};

#endif
