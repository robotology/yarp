/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_ABSTRACTCONTACTABLE_H
#define YARP_OS_ABSTRACTCONTACTABLE_H

#include <yarp/os/Contactable.h>
#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/Port.h>

namespace yarp {
    namespace os {
        class AbstractContactable;
    }
}

/**
 * A default implementation of an abstract port. The class hierarchy
 * for ports is misshapen because we want to keep it SWIG compatible,
 * and SWIG can't yet produce good translations of code that in C++
 * looks like multiple inheritance but is in fact just implementing
 * multiple interfaces.  There's work in SWIG on supporting
 * interfaces, so this can be simplified a lot at that point.
 *
 */
class YARP_OS_API yarp::os::AbstractContactable : public UnbufferedContactable {

public:
    using Contactable::open;

    /**
     *
     * get the concrete Port being used for communication
     *
     */
    virtual Port& asPort() = 0;

    /**
     *
     * get the concrete Port being used for communication, constant version
     *
     */
    virtual const Port& asPort() const = 0;

    virtual bool open(const ConstString& name) YARP_OVERRIDE {
        return asPort().open(name);
    }

    virtual bool open(const Contact& contact, bool registerName = true) YARP_OVERRIDE {
        return asPort().open(contact, registerName);
    }

    virtual bool addOutput(const ConstString& name) YARP_OVERRIDE {
        return asPort().addOutput(name);
    }

    virtual bool addOutput(const ConstString& name, const ConstString& carrier) YARP_OVERRIDE {
        return asPort().addOutput(name, carrier);
    }

    virtual bool addOutput(const Contact& contact) YARP_OVERRIDE {
        return asPort().addOutput(contact);
    }

    virtual void close() YARP_OVERRIDE {
        asPort().close();
    }

    virtual void interrupt() YARP_OVERRIDE {
        asPort().interrupt();
    }

    virtual void resume() YARP_OVERRIDE {
        asPort().resume();
    }

    virtual Contact where() const YARP_OVERRIDE {
        return asPort().where();
    }

    virtual ConstString getName() const YARP_OVERRIDE {
        return asPort().getName();
    }

    virtual bool setEnvelope(PortWriter& envelope) YARP_OVERRIDE {
        return asPort().setEnvelope(envelope);
    }

    virtual bool getEnvelope(PortReader& envelope) YARP_OVERRIDE {
        return asPort().getEnvelope(envelope);
    }

    virtual int getInputCount() YARP_OVERRIDE {
        return asPort().getInputCount();
    }

    virtual int getOutputCount() YARP_OVERRIDE {
        return asPort().getOutputCount();
    }

    virtual void getReport(PortReport& reporter) YARP_OVERRIDE {
        return asPort().getReport(reporter);
    }

    virtual void setReporter(PortReport& reporter) YARP_OVERRIDE {
        return asPort().setReporter(reporter);
    }

    virtual void resetReporter() YARP_OVERRIDE {
        return asPort().resetReporter();
    }

    virtual bool isWriting() YARP_OVERRIDE {
        return asPort().isWriting();
    }

    virtual void setReader(PortReader& reader) YARP_OVERRIDE {
        asPort().setReader(reader);
    }

    virtual void setAdminReader(PortReader& reader) YARP_OVERRIDE {
        asPort().setAdminReader(reader);
    }

    virtual void setInputMode(bool expectInput) YARP_OVERRIDE {
        asPort().setInputMode(expectInput);
    }

    virtual void setOutputMode(bool expectOutput) YARP_OVERRIDE {
        asPort().setOutputMode(expectOutput);
    }

    virtual void setRpcMode(bool expectRpc) YARP_OVERRIDE {
        asPort().setRpcMode(expectRpc);
    }

    virtual Type getType() YARP_OVERRIDE {
        return asPort().getType();
    }

    virtual void promiseType(const Type& typ) YARP_OVERRIDE {
        asPort().promiseType(typ);
    }

    virtual Property *acquireProperties(bool readOnly) YARP_OVERRIDE {
        return asPort().acquireProperties(readOnly);
    }

    virtual void releaseProperties(Property *prop) YARP_OVERRIDE {
        asPort().releaseProperties(prop);
    }

    virtual bool write(PortWriter& writer,
                       PortWriter *callback = YARP_NULLPTR) const YARP_OVERRIDE {
        return asPort().write(writer, callback);
    }

    virtual bool write(PortWriter& writer, PortReader& reader,
                       PortWriter *callback = YARP_NULLPTR) const YARP_OVERRIDE {
        return asPort().write(writer, reader, callback);
    }

    virtual bool read(PortReader& reader, bool willReply = false) YARP_OVERRIDE {
        return asPort().read(reader, willReply);
    }

    virtual bool reply(PortWriter& writer) YARP_OVERRIDE {
        return asPort().reply(writer);
    }

    virtual bool replyAndDrop(PortWriter& writer) YARP_OVERRIDE {
        return asPort().replyAndDrop(writer);
    }

    virtual void includeNodeInName(bool flag) YARP_OVERRIDE {
        return asPort().includeNodeInName(flag);
    }

    virtual bool setCallbackLock(yarp::os::Mutex *mutex = YARP_NULLPTR) YARP_OVERRIDE {
        return asPort().setCallbackLock(mutex);
    }

    virtual bool removeCallbackLock() YARP_OVERRIDE {
        return asPort().removeCallbackLock();
    }

    virtual bool lockCallback() YARP_OVERRIDE {
        return asPort().lockCallback();
    }

    virtual bool tryLockCallback() YARP_OVERRIDE {
        return asPort().tryLockCallback();
    }

    virtual void unlockCallback() YARP_OVERRIDE {
        return asPort().unlockCallback();
    }
};

#endif // YARP_OS_ABSTRACTCONTACTABLE_H
