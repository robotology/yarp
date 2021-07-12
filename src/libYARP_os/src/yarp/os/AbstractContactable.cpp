/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/AbstractContactable.h>
#include <yarp/os/Type.h>

bool yarp::os::AbstractContactable::open(const std::string& name)
{
    return asPort().open(name);
}

bool yarp::os::AbstractContactable::open(const Contact& contact,
                                         bool registerName)
{
    return asPort().open(contact, registerName);
}

bool yarp::os::AbstractContactable::addOutput(const std::string& name)
{
    return asPort().addOutput(name);
}

bool yarp::os::AbstractContactable::addOutput(const std::string& name,
                                              const std::string& carrier)
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

std::string yarp::os::AbstractContactable::getName() const
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

void yarp::os::AbstractContactable::releaseProperties(Property* prop)
{
    asPort().releaseProperties(prop);
}

bool yarp::os::AbstractContactable::write(const PortWriter& writer,
                                          const PortWriter* callback) const
{
    return asPort().write(writer, callback);
}

bool yarp::os::AbstractContactable::write(const PortWriter& writer,
                                          PortReader& reader,
                                          const PortWriter* callback) const
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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
bool yarp::os::AbstractContactable::setCallbackLock(yarp::os::Mutex* mutex)
{
    return asPort().setCallbackLock(mutex);
}
YARP_WARNING_POP
#endif

bool yarp::os::AbstractContactable::setCallbackLock(std::mutex* mutex)
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
