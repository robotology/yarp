/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCORE_H
#define YARP_OS_IMPL_PORTCORE_H

#include <yarp/os/Carriers.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/ModifyingCarrier.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortReaderCreator.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/PortWriter.h>
#include <yarp/os/Property.h>
#include <yarp/os/Type.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/PortCorePackets.h>
#include <yarp/os/impl/ThreadImpl.h>

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Mutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace yarp {
namespace os {
namespace impl {

class PortCoreUnit;

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
class YARP_os_impl_API PortDataModifier
{
public:
    PortDataModifier() :
            outputModifier(nullptr),
            inputModifier(nullptr)
    {
    }

    virtual ~PortDataModifier()
    {
        releaseOutModifier();
        releaseInModifier();
    }

    void releaseOutModifier()
    {
        if (outputModifier != nullptr) {
            outputModifier->close();
            delete outputModifier;
            outputModifier = nullptr;
        }
    }

    void releaseInModifier()
    {
        if (inputModifier != nullptr) {
            inputModifier->close();
            delete inputModifier;
            inputModifier = nullptr;
        }
    }

public:
    yarp::os::Carrier* outputModifier;
    yarp::os::Carrier* inputModifier;
    std::mutex outputMutex;
    std::mutex inputMutex;
};

class YARP_os_impl_API PortCore :
        public ThreadImpl,
        public yarp::os::PortReader
{
public:
    /**
     * Constructor.
     */
    PortCore();

    /**
     * Destructor.
     */
    ~PortCore();

    /**
     * Add an output connection to this port.
     * @param dest the name of the target
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     * @param onlyIfNeeded if true, don't add the connection if there
     * is already a connection to the named target
     * @return true on success
     */
    bool addOutput(const std::string& dest,
                   void* id,
                   yarp::os::OutputStream* os,
                   bool onlyIfNeeded = false);

    /**
     * Add another output to the port.
     */
    void addOutput(OutputProtocol* op);

    /**
     * Remove an input connection.
     * @param src the name of the source port
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     */
    void removeInput(const std::string& src,
                     void* id,
                     yarp::os::OutputStream* os);

    /**
     * Remove an output connection.
     * @param dest the name of the target port
     * @param id an opaque tracker for the connection
     * @param os the output stream for messages about this operation
     */
    void removeOutput(const std::string& dest,
                      void* id,
                      yarp::os::OutputStream* os);

    /**
     * Remove any connection matching the supplied route.
     * @param route the source/target/carrier associated with the connection
     * @param synch true if we should wait for removal to complete
     * @return true if a connection was found that needed removal
     */
    bool removeIO(const Route& route, bool synch = false);

    /**
     * Produce a text description of the port and its connections.
     * @param id opaque identifier of connection that needs the description
     * @param os stream to write on
     */
    void describe(void* id, yarp::os::OutputStream* os);

    /**
     * Generate a description of the connections associated with the
     * port.
     */
    void describe(yarp::os::PortReport& reporter);

    /**
     * Read a block of regular payload data.
     * @param reader source of data
     * @param id opaque identifier of connection providing data
     * @param os stream to write error messages on
     */
    bool readBlock(ConnectionReader& reader,
                   void* id,
                   yarp::os::OutputStream* os);

    /**
     * Read a block of administrative data.
     * Process an administrative message.
     * @param reader source of data
     * @param id opaque identifier of connection providing data
     */
    bool adminBlock(ConnectionReader& reader,
                    void* id);

    /**
     * Set the name of this port.
     * @param name the name of this port
     */
    void setName(const std::string& name);

    /**
     * @return the name of this port
     */
    std::string getName();

    /**
     * Set some envelope information to pass along with a message
     * without actually being part of the message.
     * @param envelope the extra message to send
     */
    void setEnvelope(const std::string& envelope);

    /**
     * Set some extra meta data to pass along with the message
     */
    bool setEnvelope(yarp::os::PortWriter& envelope);

    std::string getEnvelope();

    /**
     * Get any meta data associated with the last message received
     */
    bool getEnvelope(yarp::os::PortReader& envelope);

    /**
     * Handle a port event (connection, disconnection, etc)
     * Generate a description of the connections associated with the
     * port.
     * @param info the event description
     */
    void report(const yarp::os::PortInfo& info);

    /**
     * Called by a connection handler with active=true just after it
     * is fully configured, and with active=false just before it shuts
     * itself down.
     * @param unit the connection handler starting up / shutting down
     * @param active true if the handler is starting up, false if shutting down
     */
    void reportUnit(PortCoreUnit* unit, bool active);

    /**
     * Configure the port to meet certain restrictions in behavior.
     */
    void setFlags(unsigned int flags)
    {
        this->m_flags = flags;
    }

    void setContactable(Contactable* contactable)
    {
        this->m_contactable = contactable;
    }

    /**
     * Check current configuration of port.
     */
    unsigned int getFlags()
    {
        return m_flags;
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
    void setWaitBeforeSend(bool waitBeforeSend)
    {
        this->m_waitBeforeSend = waitBeforeSend;
    }

    /**
     * After sending a message, should we wait for
     * it to be sent to all destinations before returning?
     */
    void setWaitAfterSend(bool waitAfterSend)
    {
        this->m_waitAfterSend = waitAfterSend;
    }

    /**
     * Callback for data.
     */
    bool read(yarp::os::ConnectionReader& reader) override
    {
        // does nothing by default
        YARP_UNUSED(reader);
        return true;
    }

    /**
     * Begin main thread.
     */
    bool start() override;

    /**
     * Start up the port, but without a main thread.
     */
    bool manualStart(const char* sourceName);

    /**
     * Send a normal message.
     * @param writer the message
     * @param reader where to direct replies
     * @param callback who to call onCompletion() on when message sent.
     */
    bool send(const yarp::os::PortWriter& writer,
              yarp::os::PortReader* reader = nullptr,
              const yarp::os::PortWriter* callback = nullptr);

    /**
     * Send a message with a specific mode (normal or log).
     * @param writer the message
     * @param reader where to direct replies
     * @param callback who to call onCompletion() on when message sent.
     */
    bool sendHelper(const yarp::os::PortWriter& writer,
                    int mode,
                    yarp::os::PortReader* reader = nullptr,
                    const yarp::os::PortWriter* callback = nullptr);

    /**
     * Shut down port.
     */
    void close() override;

    /**
     * The body of the main thread.
     */
    void run() override;

    /**
     * A diagnostic for testing purposes.
     */
    int getEventCount();

    /**
     * Get the address associated with the port.
     */
    const Contact& getAddress() const;

    void resetPortName(const std::string& str);

    /**
     * Get the creator of callbacks.
     */
    yarp::os::PortReaderCreator* getReadCreator();

    /**
     * Call the right onCompletion() after sending message
     */
    void notifyCompletion(void* tracker);

    /**
     * Normally the port will unregister its name with the name server
     * when shutting down.  This can be inhibited.
     */
    void setControlRegistration(bool flag);

    /**
     * Prepare the port to be shut down.
     */
    void interrupt();

    /**
     * Undo an interrupt()
     */
    void resume();

    /**
     * Set a callback to be notified of changes in port status.
     */
    void setReportCallback(yarp::os::PortReport* reporter);

    /**
     * Reset the callback to be notified of changes in port status.
     */
    void resetReportCallback();

    /**
     * @return true if there is a server socket/thread currently
     * associated with this port.
     */
    bool isListening() const;

    /**
     * @return true if there the port is configured to operate without
     * a server socket/thread.
     */
    bool isManual() const;

    /**
     * @return true if port operation has been interrupted.
     */
    bool isInterrupted() const;

    void setTimeout(float timeout);

    /**
     * @warning Must be called in the same thread as releaseProperties
     */
    Property* acquireProperties(bool readOnly);

    /**
     * @warning Must be called in the same thread as acquireProperties
     */
    void releaseProperties(Property* prop);

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    YARP_DEPRECATED
    bool setCallbackLock(yarp::os::Mutex* mutex);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED

    bool setCallbackLock(std::mutex* mutex = nullptr);

    bool removeCallbackLock();

    bool lockCallback();

    bool tryLockCallback();

    void unlockCallback();

    yarp::os::impl::PortDataModifier& getPortModifier();

    void checkType(PortReader& reader);

    yarp::os::Type getType();

    void promiseType(const Type& typ);

private:
    // main internal PortCore state and operations
    std::vector<PortCoreUnit *> m_units;  ///< list of connections
    std::mutex m_stateMutex;
    std::condition_variable m_stateCv;
    std::mutex m_packetMutex;      ///< control access to message cache
    std::condition_variable m_connectionChangeCv; ///< signal changes in connections
    Face* m_face {nullptr};  ///< network server
    std::string m_name; ///< name of port
    yarp::os::Contact m_address;    ///< network address of port
    yarp::os::PortReader *m_reader {nullptr}; ///< where to send read events
    yarp::os::PortReader *m_adminReader {nullptr}; ///< where to send admin read events
    yarp::os::PortReaderCreator *m_readableCreator {nullptr}; ///< factory for readers
    yarp::os::PortReport *m_eventReporter {nullptr}; ///< where to send general events
    std::atomic<bool> m_listening {false}; ///< is the port server listening on the network?
    std::atomic<bool> m_running {false};   ///< is the port server thread running?
    std::atomic<bool> m_starting {false};  ///< is the port in its startup phase?
    std::atomic<bool> m_closing {false};   ///< is the port in its closing phase?
    std::atomic<bool> m_finished {false};  ///< is the port server thread finished running?
    bool m_finishing {false}; ///< is the port server thread trying to finish?
    bool m_waitBeforeSend {true}; ///< should we wait for all current writes to complete before writing more?
    bool m_waitAfterSend {true};  ///< should we wait for writes to complete immediately after we start them?
    bool m_controlRegistration {true};  ///< should the port unregister its name when shutting down?
    bool m_interruptable {true};  ///< is the port in an interruptible state?
    bool m_interrupted {false};    ///< is the port interrupted?
    bool m_manual {false};    ///< is the port operating without a server?
    int m_events {0};     ///< count of events that have occurred on the port
    int m_connectionListeners {0};  ///< how many threads need notification of connection changes
    int m_inputCount {0}; ///< how many input connections do we have
    int m_outputCount {0};///< how many output connections do we have
    int m_dataOutputCount {0}; ///< how many regular data output connections do we have
    unsigned int m_flags {PORTCORE_IS_INPUT | PORTCORE_IS_OUTPUT}; ///< binary flags encoding restrictions on port
    bool m_logNeeded {false}; ///< port needs to monitor message content
    PortCorePackets m_packets {}; ///< a pool for tracking messages currently being sent
    std::string m_envelope;///< user-defined wrapping data
    float m_timeout {-1};  ///< a timeout to apply to all network operations
    int m_counter {1};    ///< port-unique ids for connections
    yarp::os::Property *m_prop {nullptr};  ///< optional unstructured properties associated with port
    yarp::os::Contactable *m_contactable {nullptr}; ///< user-facing object that contains this PortCore
    std::mutex* m_mutex {nullptr}; ///< callback optional access control lock
#ifndef YARP_NO_DEPRECATED // since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    yarp::os::Mutex* m_old_mutex {nullptr};
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
    bool m_mutexOwned {false}; ///< do we own the optional callback lock
    BufferedConnectionWriter m_envelopeWriter {true}; ///< storage area for envelope, if present

    std::mutex m_typeMutex; ///< control access to type
    bool m_checkedType {false};
    Type m_type;

    // port data modifier
    yarp::os::impl::PortDataModifier m_modifier;

    // set IP packet TOS
    bool setTypeOfService(PortCoreUnit* unit, int tos);

    // get IP packet TOS
    int getTypeOfService(PortCoreUnit* unit);

    // set the scheduling properties of all threads
    // within the process scope.
    bool setProcessSchedulingParam(int priority = -1, int policy = -1);

    // attach a portmonitor plugin to the port or to a specific connection
    bool attachPortMonitor(yarp::os::Property& prop, bool isOutput, std::string& errMsg);

    // detach the portmonitor from the port or specific connection
    bool detachPortMonitor(bool isOutput);

    // set the parameter for the portmonitor of the port (if any)
    bool setParamPortMonitor(const yarp::os::Property& param, bool isOutput, std::string& errMsg);

    // get the parameters from the portmonitor of the port (if any)
    bool getParamPortMonitor(yarp::os::Property& param, bool isOutput, std::string& errMsg);

    void closeMain();

    bool isUnit(const Route& route, int index);

    // only called in "finished" phase
    void closeUnits();

    // called anytime, garbage collects terminated units
    void cleanUnits(bool blocking = true);

    // only called by the manager
    void reapUnits();

    // only called in "running" phase
    void addInput(InputProtocol* ip);

    bool removeUnit(const Route& route, bool synch = false, bool* except = nullptr);

    int getNextIndex();
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCORE_H
