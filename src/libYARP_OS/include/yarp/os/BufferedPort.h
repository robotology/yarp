// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_BUFFEREDPORT_
#define _YARP2_BUFFEREDPORT_

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
 * A mini-server for performing network communication in the background.
 * If you are a beginner, you might want to use the Port class first,
 * and then come back to BufferedPort when you want to send and receive
 * messages in the background without having to stop your processing.
 * This is convenient, but requires a little care to understand the
 * life-cycle of objects written to and read from the network
 * (see BufferedPort::read and BufferedPort::write).
 */
template <class T>
class yarp::os::BufferedPort : public Contactable,
                        public TypedReader<T>, public TypedReaderCallback<T>
{
public:
    using Contactable::open;

    /**
     * The type of content stored by this BufferedPort.
     */
    typedef T ContentType;

    /**
     * Constructor.
     */
    BufferedPort() {
        port.enableBackgroundWrite(true);
        reader.attach(port);
        writer.attach(port);
        interrupted = false;
    }

    /**
     * Destructor.
     */
    virtual ~BufferedPort() {
        port.close();
    }


    // documentation provided in Contactable
    virtual bool open(const ConstString& name) {
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name) {
        return port.addOutput(name);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name, const ConstString& carrier) {
        return port.addOutput(name,carrier);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact){
        return port.addOutput(contact);
    }

    // documentation provided in Contactable
    virtual void close() {
        port.close();
    }

    // documentation provided in Contactable
    virtual void interrupt() {
        interrupted = true;
        port.interrupt();
    }

    virtual void resume() {
        port.resume();
        interrupted = false;
    }

    /**
     * Get the number of objects ready to be read.
     */
    virtual int getPendingReads() {
        return reader.getPendingReads();
    }

    // documentation provided in Contactable
    virtual Contact where() const {
        return port.where();
    }

    // documentation provided in Contactable
    virtual ConstString getName() const {
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

    void setStrict(bool strict=true) {
        reader.setStrict(strict);
    }

    virtual T *read(bool shouldWait=true) {
        if (interrupted) return 0 /* NULL */;
        T *result = reader.read(shouldWait);
        // in some circs PortReaderBuffer::read(true) may return false
        while (result==0 /*NULL*/ && shouldWait && !reader.isClosed() &&
               !interrupted) {
            result = reader.read(shouldWait);
        }
        return result;
    }

    virtual T *lastRead() {
        return reader.lastRead();
    }

    virtual bool isClosed() {
        return reader.isClosed();
    }

    void setReplier(PortReader& reader) {
        this->reader.setReplier(reader);
    }

    void setReader(PortReader& reader) {
        setReplier(reader);
    }

    /**
     * this method will be called with new data, as long as you've
     * requested this be done by calling useCallback()
     */
    virtual void onRead(T& datum) {
        // override this to do something
    }

    /**
     * Set an object whose onRead method will be called when data is
     * available.
     */
    void useCallback(TypedReaderCallback<T>& callback) {
        reader.useCallback(callback);
    }

    /**
     * Call own onRead() method -- user can override that method
     * to be informed about data as it arrives
     */
    void useCallback() {
        reader.useCallback(*this);
    }

    void disableCallback() {
        reader.disableCallback();
    }

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) {
        return port.setEnvelope(envelope);
    }


    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) {
        return reader.getEnvelope(envelope);
    }

    // documented in Contactable
    virtual int getInputCount() {
        return port.getInputCount();
    }

    // documented in Contactable
    virtual int getOutputCount() {
        return port.getOutputCount();
    }

    // documented in Contactable
    bool isWriting() {
        return port.isWriting();
    }

    // documented in Contactable
    virtual void getReport(PortReport& reporter) {
        port.getReport(reporter);
    }

    // documented in Contactable
    virtual void setReporter(PortReport& reporter) {
        port.setReporter(reporter);
    }

    // documented in TypedReader
    virtual void *acquire() {
        return reader.acquire();
    }

    // documented in TypedReader
    virtual void release(void *handle) {
        reader.release(handle);
    }


    // documented in TypedReader
    virtual void setTargetPeriod(double period) {
        reader.setTargetPeriod(period);
    }

    virtual Type getType() {
        return port.getType();
    }

    virtual void promiseType(const Type& typ) {
        port.promiseType(typ);
    }

    virtual void setReadOnly() {
        port.setReadOnly();
    }

    virtual void setWriteOnly() {
        port.setWriteOnly();
    }


    virtual Property *acquireProperties(bool readOnly) {
        return port.acquireProperties(readOnly);
    }

    virtual void releaseProperties(Property *prop) {
        port.releaseProperties(prop);
    }

private:
    PortWriterBuffer<T> writer;
    Port port;
    PortReaderBuffer<T> reader;
    bool interrupted;

    // forbid this
    BufferedPort(const BufferedPort& alt) {
    }

    // forbid this
    const BufferedPort& operator = (const BufferedPort& alt) {
        return *this;
    }
};

#endif
