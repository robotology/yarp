/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PUBLISHER_H
#define YARP_OS_PUBLISHER_H

#include <yarp/os/Log.h>
#include <yarp/os/AbstractContactable.h>
#include <yarp/os/BufferedPort.h>

namespace yarp {
    namespace os {
         template <class T> class Publisher;
    }
}

/**
 *
 * A port specialized for publishing data of a constant type on a topic.
 *
 * \sa yarp::os::Subscriber
 *
 */
template <class T>
class yarp::os::Publisher : public AbstractContactable {
public:
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using Contactable::open;
#endif // YARP_NO_DEPRECATED

    using AbstractContactable::write;

    /**
     *
     * Constructor.
     *
     * @param name optional topic name to publish to.
     *
     */
    Publisher(const ConstString& name = "") {
        buffered_port = nullptr;
        T example;
        port.promiseType(example.getType());
        port.setInputMode(false);
        port.setOutputMode(true);
        port.setRpcMode(false);
        if (name!="") {
            yAssert(topic(name));
        }
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~Publisher() {
        clear();
    }

    /**
     *
     * Set topic to publish to
     *
     * @param name topic name
     *
     * @return true on success
     *
     */
    bool topic(const ConstString& name) {
        port.includeNodeInName(true);
        return open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const ConstString& name) override {
        clear();
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) override {
        clear();
        return port.open(contact, registerName);
    }

    // documentation provided in Contactable
    virtual void close() override {
        active().close();
    }

    // documentation provided in Contactable
    virtual void interrupt() override {
        active().interrupt();
    }

    // documentation provided in Contactable
    virtual void resume() override {
        active().resume();
    }

    // documented in Contactable
    void setReader(PortReader& reader) override {
        active().setReader(reader);
    }

    /**
     * Access the object which will be transmitted by the next call to
     * yarp::os::Publisher::write.
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
        return buffer().prepare();
    }

    /**
     * Give the last prepared object back to YARP without writing it.
     *
     * @return true if there was a prepared object to return.
     */
    bool unprepare() {
        return buffer().unprepare();
    }

    /**
     * Write the current object being returned by Publisher::prepare.
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
        buffer().write(forceStrict);
    }

    /**
     *
     * Wait for any pending writes to complete.
     *
     */
    void waitForWrite() {
        buffer().waitForWrite();
    }

    virtual int getPendingReads() {
        if (buffered_port)
            return buffered_port->getPendingReads();
        return 0;
    }

    virtual Port& asPort() override {
        return port;
    }

    virtual const Port& asPort() const override {
        return port;
    }

private:
    Port port;
    BufferedPort<T> *buffered_port;

    Contactable& active() {
        if (buffered_port) return *buffered_port;
        return port;
    }

    BufferedPort<T>& buffer() {
        if (!buffered_port) {
            buffered_port = new BufferedPort<T>(port);
            yAssert(buffered_port);
        }
        return *buffered_port;
    }

    void clear() {
        if (!buffered_port) return;
        delete buffered_port;
        buffered_port = nullptr;
    }
};

#endif // YARP_OS_PUBLISHER_H
