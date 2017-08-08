/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_BUFFEREDPORT_H
#define YARP_OS_BUFFEREDPORT_H

#include <yarp/os/Contactable.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortWriterBuffer.h>

namespace yarp {
    namespace os {
        template <class T> class BufferedPort;
    }
}

/**
 * \ingroup comm_class
 *
 * \brief A mini-server for performing network communication in the background.
 *
 * If you are a beginner, you might want to use the yarp::os::Port class first,
 * and then come back to BufferedPort when you want to send and receive
 * messages in the background without having to stop your processing.
 * This is convenient, but requires a little care to understand the
 * life-cycle of objects written to and read from the network
 * (see BufferedPort::read and BufferedPort::write).
 * 
 * By default a BufferedPort attempts to reduce latency between senders 
 * and receivers. To do so messages may be dropped by the writer if BufferedPort::write 
 * is called too quickly. The reader may also drop old messages if BufferedPort::read 
 * is not called fast enough, so that new messages can travel with high priority. This policy 
 * is sometimes called Oldest Packet Drop (ODP).
 *
 * If your application cannot afford dropping messages you can change the buffering policy. 
 * Use BufferedPort::writeStrict() when writing to a port, this waits for pending 
 * transmissions to be finished before writing new data. Call  BufferedPort::setStrict() 
 * to change the buffering policy to FIFO at the receiver side. In this way all messages will be stored
 * inside the BufferedPort and delivered to the reader. Pay attention that in this case a slow reader
 * may cause increasing latency and memory use.
 * 
 * Methods that can be useful to monitor the status of read and write operations are: 
 * yarp::os::BufferedPort::getPendingReads() and yarp::os::BufferedPort::isWriting(). 
 *
 * For examples and help, see:
 * \li \ref what_is_a_port
 * \li \ref note_ports
 * \li \ref port_expert
 * \li \ref yarp_buffering
 */
template <class T>
class yarp::os::BufferedPort : public Contactable,
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
    BufferedPort() {
        T example;
        attached = false;
        port.promiseType(example.getType());
        port.enableBackgroundWrite(true);
        interrupted = false;
    }

    /**
     * Wrap an existing unbuffered port.
     */
    BufferedPort(Port& port) {
        attached = false;
        sharedOpen(port);
        interrupted = false;
    }

    /**
     * Destructor.
     */
    virtual ~BufferedPort() {
        close();
    }


    // documentation provided in Contactable
    virtual bool open(const ConstString& name) YARP_OVERRIDE {
        attachIfNeeded();
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) YARP_OVERRIDE {
        attachIfNeeded();
        return port.open(contact, registerName);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name) YARP_OVERRIDE {
        return port.addOutput(name);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name, const ConstString& carrier) YARP_OVERRIDE {
        return port.addOutput(name, carrier);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact) YARP_OVERRIDE {
        return port.addOutput(contact);
    }

    // documentation provided in Contactable
    virtual void close() YARP_OVERRIDE {
        port.close();
        reader.detach();
        writer.detach();
        attached = false;
    }

    // documentation provided in Contactable
    virtual void interrupt() YARP_OVERRIDE {
        interrupted = true;
        port.interrupt();
    }

    virtual void resume() YARP_OVERRIDE {
        port.resume();
        interrupted = false;
    }

    /**
     * Get the number of objects ready to be read.
     */
    virtual int getPendingReads() YARP_OVERRIDE {
        return reader.getPendingReads();
    }

    // documentation provided in Contactable
    virtual Contact where() const YARP_OVERRIDE {
        return port.where();
    }

    // documentation provided in Contactable
    virtual ConstString getName() const YARP_OVERRIDE {
        return where().getName();
    }


    /**
     * Access the object which will be transmitted by the next call to
     * yarp::os::BufferedPort::write.
     * The object can safely be modified by the user of this class, to
     * prepare it.  Extra objects will be created or reused as
     * necessary depending on the state of communication with the
     * output(s) of the port.  Be careful!  If prepare() gives you
     * a reused object, it is up to the user to clear the object if that is
     * appropriate.
     * If you are sending yarp::os::Bottle objects, you may want to call
     * yarp::os::Bottle::clear(), for example.
     * YARP doesn't clear objects for you, since there are many
     * cases in which overwriting old data is suffient and reallocation
     * of memory would be unnecessary and inefficient.
     * @return the next object that will be written
     */
    T& prepare() {
        return writer.get();
    }


    /**
     *
     * Give the last prepared object back to YARP without writing it.
     *
     * @return true if there was a prepared object to return.
     *
     */
    bool unprepare() {
        return writer.unprepare();
    }

    /**
     * Write the current object being returned by BufferedPort::prepare.
     * That object should no longer be touched by the user of this class --
     * it is now owned by the communications system.  The BufferedPort::prepare
     * method should be called again to get a fresh (or reused) object
     * guaranteed to be not in use by the communications system.
     * @param forceStrict If this is true, wait until any previous sends
     * are complete.  If false, the current object will not be sent on
     * connections that are currently busy.
     *
     */
    void write(bool forceStrict=false) {
        if(isClosed())
        {
            return;
        }
        writer.write(forceStrict);
    }

    /**
     * Write the current object being returned by BufferedPort::prepare,
     * waiting until any previous sends are complete.
     * That object should no longer be touched by the user of this class --
     * it is now owned by the communications system.  The BufferedPort::prepare
     * method should be called again to get a fresh (or reused) object
     * guaranteed to be not in use by the communications system.
     *
     */
    void writeStrict() {
        write(true);
    }

    /**
     *
     * Wait for any pending writes to complete.
     *
     */
    void waitForWrite() {
        writer.waitForWrite();
    }

    /**
     *
     * Never drop any messages read.  If you can't read them as
     * fast as the come in, watch out.
     *
     */
    void setStrict(bool strict=true) YARP_OVERRIDE {
        attachIfNeeded();
        reader.setStrict(strict);
    }

    /**
     *
     * Read a message from the port.  Waits by default.
     * May return YARP_NULLPTR if the port status has changed.
     *
     * @param shouldWait false if the call should return immediately if no message is available
     * @return a message, or YARP_NULLPTR
     *
     */
    virtual T *read(bool shouldWait=true) YARP_OVERRIDE {
        if (!port.isOpen()) return YARP_NULLPTR;
        if (interrupted) return YARP_NULLPTR;
        T *result = reader.read(shouldWait);
        // in some circs PortReaderBuffer::read(true) may return false
        while (result==YARP_NULLPTR && shouldWait && !reader.isClosed() &&
               !interrupted) {
            result = reader.read(shouldWait);
        }
        return result;
    }

    virtual T *lastRead() YARP_OVERRIDE {
        return reader.lastRead();
    }

    virtual bool isClosed() YARP_OVERRIDE {
        return reader.isClosed();
    }

    void setReplier(PortReader& reader) YARP_OVERRIDE {
        attachIfNeeded();
        this->reader.setReplier(reader);
    }

    void setReader(PortReader& reader) YARP_OVERRIDE {
        attachIfNeeded();
        setReplier(reader);
    }

    void setAdminReader(PortReader& reader) YARP_OVERRIDE {
        attachIfNeeded();
        port.setAdminReader(reader);
    }

    /**
     * this method will be called with new data, as long as you've
     * requested this be done by calling useCallback()
     */
    virtual void onRead(T& datum) YARP_OVERRIDE {
        YARP_UNUSED(datum);
        // override this to do something
    }

    /**
     * Set an object whose onRead method will be called when data is
     * available.
     */
    void useCallback(TypedReaderCallback<T>& callback) YARP_OVERRIDE {
        attachIfNeeded();
        reader.useCallback(callback);
    }

    /**
     * Call own onRead() method -- user can override that method
     * to be informed about data as it arrives
     */
    void useCallback() {
        attachIfNeeded();
        reader.useCallback(*this);
    }

    void disableCallback() YARP_OVERRIDE {
        attachIfNeeded();
        reader.disableCallback();
    }

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) YARP_OVERRIDE {
        return port.setEnvelope(envelope);
    }


    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) YARP_OVERRIDE {
        return reader.getEnvelope(envelope);
    }

    // documented in Contactable
    virtual int getInputCount() YARP_OVERRIDE {
        return port.getInputCount();
    }

    // documented in Contactable
    virtual int getOutputCount() YARP_OVERRIDE {
        return port.getOutputCount();
    }

    // documented in Contactable
    bool isWriting() YARP_OVERRIDE {
        return port.isWriting();
    }

    // documented in Contactable
    virtual void getReport(PortReport& reporter) YARP_OVERRIDE {
        port.getReport(reporter);
    }

    // documented in Contactable
    virtual void setReporter(PortReport& reporter) YARP_OVERRIDE {
        port.setReporter(reporter);
    }

    // documented in Contactable
    virtual void resetReporter() YARP_OVERRIDE {
        port.resetReporter();
    }

    // documented in TypedReader
    virtual void *acquire() YARP_OVERRIDE {
        return reader.acquire();
    }

    // documented in TypedReader
    virtual void release(void *handle) YARP_OVERRIDE {
        reader.release(handle);
    }


    // documented in TypedReader
    virtual void setTargetPeriod(double period) YARP_OVERRIDE {
        attachIfNeeded();
        reader.setTargetPeriod(period);
    }

    virtual Type getType() YARP_OVERRIDE {
        return port.getType();
    }

    virtual void promiseType(const Type& typ) YARP_OVERRIDE {
        port.promiseType(typ);
    }

    virtual void setInputMode(bool expectInput) YARP_OVERRIDE {
        port.setInputMode(expectInput);
    }

    virtual void setOutputMode(bool expectOutput) YARP_OVERRIDE {
        port.setOutputMode(expectOutput);
    }

    virtual void setRpcMode(bool expectRpc) YARP_OVERRIDE {
        port.setRpcMode(expectRpc);
    }


    virtual Property *acquireProperties(bool readOnly) YARP_OVERRIDE {
        return port.acquireProperties(readOnly);
    }

    virtual void releaseProperties(Property *prop) YARP_OVERRIDE {
        port.releaseProperties(prop);
    }

    virtual void includeNodeInName(bool flag) YARP_OVERRIDE {
        return port.includeNodeInName(flag);
    }

    virtual bool setCallbackLock(yarp::os::Mutex *mutex) YARP_OVERRIDE {
        return port.setCallbackLock(mutex);
    }

    virtual bool removeCallbackLock() YARP_OVERRIDE {
        return port.removeCallbackLock();
    }

    virtual bool lockCallback() YARP_OVERRIDE {
        return port.lockCallback();
    }

    virtual bool tryLockCallback() YARP_OVERRIDE {
        return port.tryLockCallback();
    }

    virtual void unlockCallback() YARP_OVERRIDE {
        return port.unlockCallback();
    }

private:
    PortWriterBuffer<T> writer;
    Port port;
    PortReaderBuffer<T> reader;
    bool interrupted;
    bool attached;

    // forbid this
    BufferedPort(const BufferedPort& alt) {
        YARP_UNUSED(alt);
    }

    // forbid this
    const BufferedPort& operator = (const BufferedPort& alt) {
        YARP_UNUSED(alt);
        return *this;
    }

    void attachIfNeeded() {
        if (!attached) {
            reader.attach(port);
            writer.attach(port);
            attached = true;
        }
    }

    bool sharedOpen(Port& port) {
        bool ok = this->port.sharedOpen(port);
        if (!ok) return false;
        reader.attach(port);
        writer.attach(port);
        attached = true;
        return true;
    }
};

#endif // YARP_OS_BUFFEREDPORT_H
