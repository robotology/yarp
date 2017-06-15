/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_SUBSCRIBER_H
#define YARP_OS_SUBSCRIBER_H

#include <yarp/os/Log.h>
#include <yarp/os/AbstractContactable.h>
#include <yarp/os/BufferedPort.h>

namespace yarp {
    namespace os {
         template <class T> class Subscriber;
    }
}

/**
 *
 * A port specialized for reading data of a constant type published on a topic.
 *
 * \sa yarp::os::Publisher
 *
 */
template <class T>
class yarp::os::Subscriber : public AbstractContactable,
                             public TypedReaderCallback<T>
{
public:
    using Contactable::open;
    using AbstractContactable::read;

    /**
     *
     * Constructor.
     *
     * @param name optional topic name to publish to.
     *
     */
    Subscriber(const ConstString& name = "") {
        buffered_port = YARP_NULLPTR;
        T example;
        port.promiseType(example.getType());
        port.setInputMode(true);
        port.setOutputMode(false);
        port.setRpcMode(false);
        if (name!="") {
            yAssert(topic(name));
        }
        isStrict = false;
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~Subscriber() {
        clear();
    }

    /**
     *
     * Set topic to subscribe to
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
    virtual bool open(const ConstString& name) YARP_OVERRIDE {
        clear();
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) YARP_OVERRIDE {
        clear();
        return port.open(contact, registerName);
    }

    // documentation provided in Contactable
    virtual void close() YARP_OVERRIDE {
        active().close();
    }

    // documentation provided in Contactable
    virtual void interrupt() YARP_OVERRIDE {
        active().interrupt();
    }

    // documentation provided in Contactable
    virtual void resume() YARP_OVERRIDE {
        active().resume();
    }

    // documented in Contactable
    void setReader(PortReader& reader) YARP_OVERRIDE {
        active().setReader(reader);
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
    T *read(bool shouldWait = true) {
        return buffer().read(shouldWait);
    }

    virtual Port& asPort() YARP_OVERRIDE {
        return port;
    }

    virtual const Port& asPort() const YARP_OVERRIDE {
        return port;
    }

    using TypedReaderCallback<T>::onRead;
    virtual void onRead (T &datum) YARP_OVERRIDE {
         YARP_UNUSED(datum);
         // override this to do something
    }

    void useCallback (TypedReaderCallback< T > &callback) {
        buffer().useCallback(callback);
    }

    void useCallback() {
        buffer().useCallback(*this);
    }

    void disableCallback() {
        buffer().disableCallback();
    }

    void setStrict(bool strict = true) {
        isStrict = strict;
        if (buffered_port) buffered_port->setStrict(strict);
    }
    
private:
    bool isStrict;
    Port port;
    BufferedPort<T> *buffered_port;

    Contactable& active() {
        if (buffered_port) return *buffered_port;
        return port;
    }

    BufferedPort<T>& buffer() {
        if (!buffered_port) {
            buffered_port = new BufferedPort<T>(port);
            if (isStrict) {
                buffered_port->setStrict(isStrict);
            }
            yAssert(buffered_port);
        }
        return *buffered_port;
    }

    void clear() {
        if (!buffered_port) return;
        delete buffered_port;
        buffered_port = YARP_NULLPTR;
    }
};

#endif // YARP_OS_SUBSCRIBER_H
