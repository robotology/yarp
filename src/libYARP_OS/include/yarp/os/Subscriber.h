/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
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
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.72
    using Contactable::open;
#endif // YARP_NO_DEPRECATED
    using AbstractContactable::read;

    /**
     *
     * Constructor.
     *
     * @param name optional topic name to publish to.
     *
     */
    Subscriber(const ConstString& name = "") {
        buffered_port = nullptr;
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
     *
     * Read a message from the port.  Waits by default.
     * May return nullptr if the port status has changed.
     *
     * @param shouldWait false if the call should return immediately if no message is available
     * @return a message, or nullptr
     *
     */
    T *read(bool shouldWait = true) {
        return buffer().read(shouldWait);
    }

    virtual Port& asPort() override {
        return port;
    }

    virtual const Port& asPort() const override {
        return port;
    }

    using TypedReaderCallback<T>::onRead;
    virtual void onRead (T &datum) override {
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
        buffered_port = nullptr;
    }
};

#endif // YARP_OS_SUBSCRIBER_H
