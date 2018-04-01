/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_BUFFEREDPORT_H
#define YARP_OS_BUFFEREDPORT_H

#include <yarp/os/Contactable.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortWriterBuffer.h>


// Defined in this file:
#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
namespace yarp { namespace os { template <typename T> class BufferedPort; }}
#endif // YARP_NO_DEPRECATED


namespace yarp {
namespace os {

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0

/**
 * @ingroup comm_class
 *
 * @brief A mini-server for performing network communication in the background.
 *
 * If you are a beginner, you might want to use the yarp::os::Port class first,
 * and then come back to BufferedPort when you want to send and receive
 * messages in the background without having to stop your processing.
 * This is convenient, but requires a little care to understand the
 * life-cycle of objects written to and read from the network
 * (see BufferedPort::read and BufferedPort::write).
 *
 * By default a BufferedPort attempts to reduce latency between senders
 * and receivers. To do so messages may be dropped by the writer if
 * BufferedPort::write is called too quickly
 * The reader may also drop old messages if BufferedPort::read is not called
 * fast enough, so that new messages can travel with high priority.
 * This policy is sometimes called Oldest Packet Drop (ODP).
 *
 * If your application cannot afford dropping messages you can change the
 * buffering policy.
 * Use BufferedPort::writeStrict() when writing to a port, this waits for
 * pending transmissions to be finished before writing new data.
 * Call BufferedPort::setStrict() to change the buffering policy to FIFO at the
 * receiver side.
 * In this way all messages will be stored inside the BufferedPort and delivered
 * to the reader.
 * Pay attention that in this case a slow reader may cause increasing latency
 * and memory use.
 *
 * Methods that can be useful to monitor the status of read and write operations
 * are yarp::os::BufferedPort::getPendingReads() and
 * yarp::os::BufferedPort::isWriting().
 *
 * @sa For examples and help, see:
 * @li @ref what_is_a_port
 * @li @ref note_ports
 * @li @ref port_expert
 * @li @ref yarp_buffering
 *
 * @deprecated since YARP 3.0.0. YARP no longer supports Port and BufferedPort.
 *             Use yarp::os::Publisher or yarp::os::Subscriber instead.
 */
template <typename T>
class YARP_DEPRECATED_MSG("YARP no longer supports Port and BufferedPort. Use yarp::os::Publisher or yarp::os::Subscriber instead.")
BufferedPort : public Contactable,
               public TypedReader<T>,
               public TypedReaderCallback<T>
{
public:
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using Contactable::open;
#endif // YARP_NO_DEPRECATED

    using yarp::os::TypedReaderCallback<T>::onRead;

    /**
     * The type of content stored by this BufferedPort.
     */
    typedef T ContentType;

    /**
     * Constructor.
     */
    BufferedPort();

    /**
     * Wrap an existing unbuffered port.
     */
    BufferedPort(Port& port);

    /**
     * Destructor.
     */
    virtual ~BufferedPort();

    // Documented in Contactable
    virtual bool open(const ConstString& name) override;

    // Documented in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) override;

    // Documented in Contactable
    virtual bool addOutput(const ConstString& name) override;

    // Documented in Contactable
    virtual bool addOutput(const ConstString& name, const ConstString& carrier) override;

    // Documented in Contactable
    virtual bool addOutput(const Contact& contact) override;

    // Documented in Contactable
    virtual void close() override;

    // Documented in Contactable
    virtual void interrupt() override;

    virtual void resume() override;

    /**
     * Get the number of objects ready to be read.
     */
    virtual int getPendingReads() override;

    // Documented in Contactable
    virtual Contact where() const override;

    // Documented in Contactable
    virtual ConstString getName() const override;


    /**
     * Access the object which will be transmitted by the next call to
     * yarp::os::BufferedPort::write.
     *
     * The object can safely be modified by the user of this class, to prepare
     * it.
     * Extra objects will be created or reused as necessary depending on the
     * state of communication with the output(s) of the port.
     *
     * @warning If prepare() gives you a reused object, it is up to the user to
     *          clear the object if that is appropriate.
     *          If you are sending yarp::os::Bottle objects, you may want to
     *          call yarp::os::Bottle::clear(), for example.
     *          YARP doesn't clear objects for you, since there are many
     *          cases in which overwriting old data is suffient and reallocation
     *          of memory would be unnecessary and inefficient.
     *
     * @return the next object that will be written
     */
    T& prepare();

    /**
     * Give the last prepared object back to YARP without writing it.
     *
     * @return true if there was a prepared object to return.
     */
    bool unprepare();

    /**
     * Write the current object being returned by BufferedPort::prepare.
     *
     * @warning That object should no longer be touched by the user of this
     *          class, it is now owned by the communications system.
     *          The BufferedPort::prepare method should be called again to get a
     *          fresh (or reused) object guaranteed to be not in use by the
     *          communications system.
     *
     * @param forceStrict If this is true, wait until any previous sends
     *                    are complete.
     *                    If false, the current object will not be sent on
     *                    connections that are currently busy.
     */
    void write(bool forceStrict = false);

    /**
     * Write the current object being returned by BufferedPort::prepare,
     * waiting until any previous sends are complete.
     *
     * @warning That object should no longer be touched by the user of this
     *          class it is now owned by the communications system.
     *          The BufferedPort::prepare method should be called again to get a
     *          fresh (or reused) object guaranteed to be not in use by the
     *          communications system.
     */
    void writeStrict();

    /**
     * Wait for any pending writes to complete.
     */
    void waitForWrite();

    // Documented in TypedReader
    virtual void setStrict(bool strict = true) override;

    // Documented in TypedReader
    virtual T* read(bool shouldWait = true) override;

    // Documented in TypedReader
    virtual T* lastRead() override;

    // Documented in TypedReader
    virtual bool isClosed() override;

    // Documented in TypedReader
    void setReplier(PortReader& reader) override;

    // Documented in Contactable
    void setReader(PortReader& reader) override;

    // Documented in Contactable
    void setAdminReader(PortReader& reader) override;

    /**
     * Callback method.
     *
     * this method will be called with new data, as long as you've
     * requested this be done by calling useCallback()
     *
     * @param datum data read from a port
     */
    virtual void onRead(T& datum) override;

    // Documented in TypedReader
    virtual void useCallback(TypedReaderCallback<T>& callback) override;

    /**
     * Use own onRead() method as callback.
     *
     * User can override that method to be informed about data as it arrives.
     */
    void useCallback();

    // Documented in TypedReader
    virtual void disableCallback() override;

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) override;

    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) override;

    // documented in Contactable
    virtual int getInputCount() override;

    // documented in Contactable
    virtual int getOutputCount() override;

    // documented in Contactable
    bool isWriting() override;

    // documented in Contactable
    virtual void getReport(PortReport& reporter) override;

    // documented in Contactable
    virtual void setReporter(PortReport& reporter) override;

    // documented in Contactable
    virtual void resetReporter() override ;

    // documented in TypedReader
    virtual void* acquire() override ;

    // documented in TypedReader
    virtual void release(void* handle) override;

    // documented in TypedReader
    virtual void setTargetPeriod(double period) override;

    // documented in Contactable
    virtual Type getType() override;

    // documented in Contactable
    virtual void promiseType(const Type& typ) override;

    // documented in Contactable
    virtual void setInputMode(bool expectInput) override;

    // documented in Contactable
    virtual void setOutputMode(bool expectOutput) override;

    // documented in Contactable
    virtual void setRpcMode(bool expectRpc) override;

    // documented in Contactable
    virtual Property* acquireProperties(bool readOnly) override;

    // documented in Contactable
    virtual void releaseProperties(Property* prop) override;

    // documented in Contactable
    virtual void includeNodeInName(bool flag) override;

    // documented in Contactable
    virtual bool setCallbackLock(yarp::os::Mutex* mutex) override;

    // documented in Contactable
    virtual bool removeCallbackLock() override;

    // documented in Contactable
    virtual bool lockCallback() override;

    // documented in Contactable
    virtual bool tryLockCallback() override;

    // documented in Contactable
    virtual void unlockCallback() override;

private:
    PortWriterBuffer<T> writer;
    Port port;
    PortReaderBuffer<T> reader;
    bool interrupted;
    bool attached;

    // forbid this
    BufferedPort(const BufferedPort& alt) = delete;
    const BufferedPort& operator=(const BufferedPort& alt) = delete;

    void attachIfNeeded();
    bool sharedOpen(Port& port);
};

} // namespace os
} // namespace yarp

#include <yarp/os/BufferedPort-inl.h>

#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_BUFFEREDPORT_H
