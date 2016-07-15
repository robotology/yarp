/*
 * Copyright (C) 2006, 2007, 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTCORE
#define YARP2_PORTCORE

#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/SemaphoreImpl.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/PortManager.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/impl/PortCorePackets.h>

#include <yarp/os/PortReport.h>
#include <yarp/os/Property.h>
#include <yarp/os/Mutex.h>

#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

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
            class PortDataModifier;
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
 * Phases
 * ------
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
 * Connections
 * -----------
 *
 * The port's connections are stored in the PortCore#units list.  Input
 * and output connections are stored in the same list, and a lot
 * of the code does not distinguish them.  Outgoing messages on the 
 * connections are tracked using the PortCore#packets list.  A single
 * message may be associated with many connections.
 *
 */

/**
 * @brief The yarp::os::impl::PortDataModifier class is a helper
 *  class to manage the port data modifiers
 */
class YARP_OS_impl_API yarp::os::impl::PortDataModifier {
public:
    PortDataModifier() {
        outputModifier = NULL;
        inputModifier = NULL;
    }
    virtual ~PortDataModifier() {
        releaseOutModifier();
        releaseInModifier();
    }

    void releaseOutModifier() {
        if(outputModifier != NULL) {
            outputModifier->close();
            delete outputModifier;
            outputModifier = NULL;
        }
    }

    void releaseInModifier() {
        if(inputModifier != NULL) {
            inputModifier->close();
            delete inputModifier;
            inputModifier = NULL;
        }
    }

public:
    yarp::os::Carrier* outputModifier;
    yarp::os::Carrier* inputModifier;
    yarp::os::Mutex    outputMutex;
    yarp::os::Mutex    inputMutex;
};

class YARP_OS_impl_API yarp::os::impl::PortCore : public ThreadImpl, public PortManager, public yarp::os::PortReader {
public:

    /**
     * Constructor.
     */
    PortCore() :
            stateMutex(1),
            packetMutex(1),
            connectionChange(1),
            log("port",Logger::get()),
            face(NULL),
            reader(NULL),
            adminReader(NULL),
            readableCreator(NULL),
            eventReporter(NULL),
            listening(false),
            running(false),
            starting(false),
            closing(false),
            finished(false),
            finishing(false),
            waitBeforeSend(true),
            waitAfterSend(true),
            controlRegistration(true),
            interruptible(true),
            interrupted(false),
            manual(false),
            events(0),
            connectionListeners(0),
            inputCount(0),
            outputCount(0),
            dataOutputCount(0),
            flags(PORTCORE_IS_INPUT|PORTCORE_IS_OUTPUT),
            verbosity(1),
            logNeeded(false),
            timeout(-1),
            counter(1),
            prop(NULL),
            contactable(NULL),
            mutex(NULL),
            mutexOwned(false),
            envelopeWriter(true)
    {
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
     * Set a callback for incoming administrative messages.
     */
    void setAdminReadHandler(yarp::os::PortReader& reader);

    /**
     * Set a callback for creating callbacks for incoming data.
     */
    void setReadCreator(yarp::os::PortReaderCreator& creator);

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

    void resetPortName(const ConstString& str) {
        address.setName(str);
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
    void setEnvelope(const ConstString& envelope);

    ConstString getEnvelope();

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


    /**
     *
     * @return true if there is a server socket/thread currently
     * associated with this port.
     *
     */
    bool isListening() const {
        return listening;
    }

    /**
     *
     * @return true if there the port is configured to operate without
     * a server socket/thread.
     *
     */
    bool isManual() const {
        return manual;
    }

    /**
     *
     * @return true if port operation has been interrupted.
     *
     */
    bool isInterrupted() const {
        return interrupted;
    }

public:

    // documented in PortManager
    virtual bool addOutput(const ConstString& dest, void *id, OutputStream *os,
                           bool onlyIfNeeded);

    // documented in PortManager
    virtual void removeOutput(const ConstString& dest, void *id, OutputStream *os);

    // documented in PortManager
    virtual void removeInput(const ConstString& dest, void *id, OutputStream *os);

    // documented in PortManager
    virtual void describe(void *id, OutputStream *os);

    // documented in PortManager
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

    bool setCallbackLock(yarp::os::Mutex *mutex = NULL) {
        removeCallbackLock();
        if (mutex) {
            this->mutex = mutex;
            mutexOwned = false;
        } else {
            this->mutex = new yarp::os::Mutex();
            mutexOwned = true;
        }
        return true;
    }

    bool removeCallbackLock() {
        if (mutexOwned&&mutex) {
            delete mutex;
        }
        mutex = NULL;
        mutexOwned = false;
        return true;
    }

    bool lockCallback() {
        if (!mutex) return false;
        mutex->lock();
        return true;
    }

    bool tryLockCallback() {
        if (!mutex) return true;
        return mutex->tryLock();
    }

    void unlockCallback() {
        if (!mutex) return;
        mutex->unlock();
    }

    void take(PortCore *alt) {
    }

    yarp::os::impl::PortDataModifier& getPortModifier() {
        return modifier;
    }

private:

    // main internal PortCore state and operations
    PlatformVector<PortCoreUnit *> units;  ///< list of connections
    SemaphoreImpl stateMutex;       ///< control access to essential port state
    SemaphoreImpl packetMutex;      ///< control access to message cache
    SemaphoreImpl connectionChange; ///< signal changes in connections
    Logger log;  ///< message logger
    Face *face;  ///< network server
    ConstString name; ///< name of port
    yarp::os::Contact address;    ///< network address of port
    yarp::os::PortReader *reader; ///< where to send read events
    yarp::os::PortReader *adminReader; ///< where to send admin read events
    yarp::os::PortReaderCreator *readableCreator; ///< factory for readers
    yarp::os::PortReport *eventReporter; ///< where to send general events
    bool listening; ///< is the port server listening on the network?
    bool running;   ///< is the port server thread running?
    bool starting;  ///< is the port in its startup phase?
    bool closing;   ///< is the port in its closing phase?
    bool finished;  ///< is the port server thread finished running?
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
    ConstString envelope;///< user-defined wrapping data
    float timeout;  ///< a timeout to apply to all network operations
    int counter;    ///< port-unique ids for connections
    yarp::os::Property *prop;  ///< optional unstructured properties associated with port
    yarp::os::Contactable *contactable;  ///< user-facing object that contains this PortCore
    yarp::os::Mutex *mutex; ///< callback optional access control lock
    bool mutexOwned;        ///< do we own the optional callback lock
    BufferedConnectionWriter envelopeWriter; ///< storage area for envelope, if present

    // port data modifier
    yarp::os::impl::PortDataModifier modifier;

    // set IP packet TOS
    bool setTypeOfService(PortCoreUnit *unit, int tos);

    // get IP packet TOS
    int  getTypeOfService(PortCoreUnit *unit);

    // set the scheduling properties of all threads
    // whithin the process scope.
    bool setProcessSchedulingParam(int priority=-1, int policy=-1);

    // cross-platform way of getting process ID (with or whitout ACE)
    int  getPid();

    // attach a portmonitor plugin to the port or to a specific connection
    bool attachPortMonitor(yarp::os::Property& prop, bool isOutput, ConstString &errMsg);

    // detach the portmonitor from the port or specific connection
    bool dettachPortMonitor(bool isOutput);

    void closeMain();

    bool isUnit(const Route& route, int index);

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
};

#endif
