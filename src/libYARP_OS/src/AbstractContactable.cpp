/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/AbstractContactable.h>

bool yarp::os::AbstractContactable::open(const ConstString& name)
{
    return asPort().open(name);
}

bool yarp::os::AbstractContactable::open(const Contact& contact,
                                         bool registerName)
{
    return asPort().open(contact, registerName);
}

bool yarp::os::AbstractContactable::addOutput(const ConstString& name)
{
    return asPort().addOutput(name);
}

bool yarp::os::AbstractContactable::addOutput(const ConstString& name,
                                              const ConstString& carrier)
{
    return asPort().addOutput(name, carrier);
}

bool yarp::os::AbstractContactable::addOutput(const Contact& contact)
{
    return asPort().addOutput(contact);
}

void yarp::os::AbstractContactable::close()
{
    asPort().close();
}

void yarp::os::AbstractContactable::interrupt()
{
    asPort().interrupt();
}

void yarp::os::AbstractContactable::resume()
{
    asPort().resume();
}

yarp::os::Contact yarp::os::AbstractContactable::where() const
{
    return asPort().where();
}

yarp::os::ConstString yarp::os::AbstractContactable::getName() const
{
    return asPort().getName();
}

bool yarp::os::AbstractContactable::setEnvelope(PortWriter& envelope)
{
    return asPort().setEnvelope(envelope);
}

bool yarp::os::AbstractContactable::getEnvelope(PortReader& envelope)
{
    return asPort().getEnvelope(envelope);
}

int yarp::os::AbstractContactable::getInputCount()
{
    return asPort().getInputCount();
}

int yarp::os::AbstractContactable::getOutputCount()
{
    return asPort().getOutputCount();
}

void yarp::os::AbstractContactable::getReport(PortReport& reporter)
{
    asPort().getReport(reporter);
}

void yarp::os::AbstractContactable::setReporter(PortReport& reporter)
{
    asPort().setReporter(reporter);
}

void yarp::os::AbstractContactable::resetReporter()
{
    asPort().resetReporter();
}

bool yarp::os::AbstractContactable::isWriting()
{
    return asPort().isWriting();
}

void yarp::os::AbstractContactable::setReader(PortReader& reader)
{
    asPort().setReader(reader);
}

void yarp::os::AbstractContactable::setAdminReader(PortReader& reader)
{
    asPort().setAdminReader(reader);
}

void yarp::os::AbstractContactable::setInputMode(bool expectInput)
{
    asPort().setInputMode(expectInput);
}

void yarp::os::AbstractContactable::setOutputMode(bool expectOutput)
{
    asPort().setOutputMode(expectOutput);
}

void yarp::os::AbstractContactable::setRpcMode(bool expectRpc)
{
    asPort().setRpcMode(expectRpc);
}

yarp::os::Type yarp::os::AbstractContactable::getType()
{
    return asPort().getType();
}

void yarp::os::AbstractContactable::promiseType(const Type& typ)
{
    asPort().promiseType(typ);
}

yarp::os::Property* yarp::os::AbstractContactable::acquireProperties(bool readOnly)
{
    return asPort().acquireProperties(readOnly);
}

void yarp::os::AbstractContactable::releaseProperties(Property *prop)
{
    asPort().releaseProperties(prop);
}

bool yarp::os::AbstractContactable::write(PortWriter& writer,
                                          PortWriter* callback) const
{
    return asPort().write(writer, callback);
}

bool yarp::os::AbstractContactable::write(PortWriter& writer,
                                          PortReader& reader,
                                          PortWriter *callback) const
{
    return asPort().write(writer, reader, callback);
}

bool yarp::os::AbstractContactable::read(PortReader& reader,
                                         bool willReply)
{
    return asPort().read(reader, willReply);
}

bool yarp::os::AbstractContactable::reply(PortWriter& writer)
{
    return asPort().reply(writer);
}

bool yarp::os::AbstractContactable::replyAndDrop(PortWriter& writer)
{
    return asPort().replyAndDrop(writer);
}

void yarp::os::AbstractContactable::includeNodeInName(bool flag)
{
    asPort().includeNodeInName(flag);
}

bool yarp::os::AbstractContactable::setCallbackLock(yarp::os::Mutex *mutex)
{
    return asPort().setCallbackLock(mutex);
}

bool yarp::os::AbstractContactable::removeCallbackLock()
{
    return asPort().removeCallbackLock();
}

bool yarp::os::AbstractContactable::lockCallback()
{
    return asPort().lockCallback();
}

bool yarp::os::AbstractContactable::tryLockCallback()
{
    return asPort().tryLockCallback();
}

void yarp::os::AbstractContactable::unlockCallback()
{
    asPort().unlockCallback();
}
