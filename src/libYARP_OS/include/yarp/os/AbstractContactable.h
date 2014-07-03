// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_ABSTRACTCONTACTABLE_
#define _YARP2_ABSTRACTCONTACTABLE_

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

    virtual bool open(const ConstString& name) {
        return asPort().open(name);
    }

    virtual bool open(const Contact& contact, bool registerName = true) {
        return asPort().open(contact,registerName);
    }

    virtual bool addOutput(const ConstString& name) {
        return asPort().addOutput(name);
    }

    virtual bool addOutput(const ConstString& name, const ConstString& carrier) {
        return asPort().addOutput(name,carrier);
    }

    virtual bool addOutput(const Contact& contact) {
        return asPort().addOutput(contact);
    }

    virtual void close() {
        asPort().close();
    }

    virtual void interrupt() {
        asPort().interrupt();
    }

    virtual void resume() {
        asPort().resume();
    }

    virtual Contact where() const {
        return asPort().where();
    }

    virtual ConstString getName() const {
        return asPort().getName();
    }

    virtual bool setEnvelope(PortWriter& envelope) {
        return asPort().setEnvelope(envelope);
    }

    virtual bool getEnvelope(PortReader& envelope) {
        return asPort().getEnvelope(envelope);
    }

    virtual int getInputCount() {
        return asPort().getInputCount();
    }

    virtual int getOutputCount() {
        return asPort().getOutputCount();
    }

    virtual void getReport(PortReport& reporter) {
        return asPort().getReport(reporter);
    }

    virtual void setReporter(PortReport& reporter) {
        return asPort().setReporter(reporter);
    }

    virtual bool isWriting() {
        return asPort().isWriting();
    }

    virtual void setReader(PortReader& reader) {
        asPort().setReader(reader);
    }

    virtual void setInputMode(bool expectInput) {
        asPort().setInputMode(expectInput);
    }

    virtual void setOutputMode(bool expectOutput) {
        asPort().setOutputMode(expectOutput);
    }

    virtual void setRpcMode(bool expectRpc) {
        asPort().setRpcMode(expectRpc);
    }

    virtual Type getType() {
        return asPort().getType();
    }

    virtual void promiseType(const Type& typ) {
        asPort().promiseType(typ);
    }

    virtual Property *acquireProperties(bool readOnly) {
        asPort().acquireProperties(readOnly);
    }

    virtual void releaseProperties(Property *prop) {
        asPort().releaseProperties(prop);
    }

    virtual bool write(PortWriter& writer, 
                       PortWriter *callback = 0 /*NULL*/) const {
        return asPort().write(writer,callback);
    }

    virtual bool write(PortWriter& writer, PortReader& reader,
                       PortWriter *callback = 0 /*NULL*/) const {
        return asPort().write(writer,reader,callback);
    }

    virtual bool read(PortReader& reader, bool willReply = false) {
        return asPort().read(reader,willReply);
    }

    virtual bool reply(PortWriter& writer) {
        return asPort().reply(writer);
    }

    virtual bool replyAndDrop(PortWriter& writer) {
        return asPort().replyAndDrop(writer);
    }

    virtual void includeNodeInName(bool flag) {
        return asPort().includeNodeInName(flag);
    }
};

#endif
