// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PUBLISHER_
#define _YARP2_PUBLISHER_

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
    using Contactable::open;
    using AbstractContactable::write;

    /**
     *
     * Constructor.
     *
     * @param name optional topic name to publish to.
     *
     */
    Publisher(const ConstString& name = "") {
        buffered_port = 0 /*NULL*/;
        T example;
        port.promiseType(example.getType());
        port.setInputMode(false);
        port.setOutputMode(true);
        port.setRpcMode(false);
        if (name!="") {
            YARP_ASSERT(topic(name));
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
    virtual bool open(const ConstString& name) {
        clear();
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        clear();
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual void close() {
        active().close();
    }

    // documentation provided in Contactable
    virtual void interrupt() {
        active().interrupt();        
    }

    // documentation provided in Contactable
    virtual void resume() {
        active().resume();        
    }

    // documented in Contactable
    void setReader(PortReader& reader) {
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

    virtual Port& asPort() {
        return port;
    }

    virtual const Port& asPort() const {
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
            YARP_ASSERT(buffered_port);
        }
        return *buffered_port;
    }

    void clear() {
        if (!buffered_port) return;
        delete buffered_port;
        buffered_port = 0 /*NULL*/;
    }
};

#endif

