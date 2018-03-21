/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_BUFFEREDPORT_INL_H
#define YARP_OS_BUFFEREDPORT_INL_H

#include <yarp/os/BufferedPort.h>

template <typename T>
yarp::os::BufferedPort<T>::BufferedPort() :
        interrupted(false),
        attached(false)
{
    T example;
    port.promiseType(example.getType());
    port.enableBackgroundWrite(true);
}

template <typename T>
yarp::os::BufferedPort<T>::BufferedPort(Port& port) :
        interrupted(false),
        attached(false)
{
    sharedOpen(port);
}

template <typename T>
yarp::os::BufferedPort<T>::~BufferedPort()
{
    close();
}

template <typename T>
bool yarp::os::BufferedPort<T>::open(const ConstString& name)
{
    attachIfNeeded();
    return port.open(name);
}

template <typename T>
bool yarp::os::BufferedPort<T>::open(const Contact& contact, bool registerName)
{
    attachIfNeeded();
    return port.open(contact, registerName);
}

template <typename T>
bool yarp::os::BufferedPort<T>::addOutput(const ConstString& name)
{
    return port.addOutput(name);
}

template <typename T>
bool yarp::os::BufferedPort<T>::addOutput(const ConstString& name, const ConstString& carrier)
{
    return port.addOutput(name, carrier);
}

template <typename T>
bool yarp::os::BufferedPort<T>::addOutput(const Contact& contact)
{
    return port.addOutput(contact);
}

template <typename T>
void yarp::os::BufferedPort<T>::close()
{
    port.close();
    reader.detach();
    writer.detach();
    attached = false;
}

template <typename T>
void yarp::os::BufferedPort<T>::interrupt()
{
    interrupted = true;
    port.interrupt();
}

template <typename T>
void yarp::os::BufferedPort<T>::resume()
{
    port.resume();
    interrupted = false;
}

template <typename T>
int yarp::os::BufferedPort<T>::getPendingReads()
{
    return reader.getPendingReads();
}

template <typename T>
yarp::os::Contact yarp::os::BufferedPort<T>::where() const
{
    return port.where();
}

template <typename T>
yarp::os::ConstString yarp::os::BufferedPort<T>::getName() const
{
    return where().getName();
}

template <typename T>
T& yarp::os::BufferedPort<T>::prepare()
{
    return writer.get();
}

template <typename T>
bool yarp::os::BufferedPort<T>::unprepare()
{
    return writer.unprepare();
}

template <typename T>
void yarp::os::BufferedPort<T>::write(bool forceStrict)
{
    if(isClosed()) {
        return;
    }
    writer.write(forceStrict);
}

template <typename T>
void yarp::os::BufferedPort<T>::writeStrict()
{
    write(true);
}

template <typename T>
void yarp::os::BufferedPort<T>::waitForWrite()
{
    writer.waitForWrite();
}

template <typename T>
void yarp::os::BufferedPort<T>::setStrict(bool strict)
{
    attachIfNeeded();
    reader.setStrict(strict);
}

template <typename T>
T* yarp::os::BufferedPort<T>::read(bool shouldWait)
{
    if (!port.isOpen()) {
        return nullptr;
    }
    if (interrupted) {
        return nullptr;
    }
    T* result = reader.read(shouldWait);
    // in some circs PortReaderBuffer::read(true) may return false
    while (result==nullptr && shouldWait && !reader.isClosed() && !interrupted) {
        result = reader.read(shouldWait);
    }
    return result;
}

template <typename T>
T* yarp::os::BufferedPort<T>::lastRead()
{
    return reader.lastRead();
}

template <typename T>
bool yarp::os::BufferedPort<T>::isClosed()
{
    return reader.isClosed();
}

template <typename T>
void yarp::os::BufferedPort<T>::setReplier(PortReader& reader)
{
    attachIfNeeded();
    this->reader.setReplier(reader);
}

template <typename T>
void yarp::os::BufferedPort<T>::setReader(PortReader& reader)
{
    attachIfNeeded();
    setReplier(reader);
}

template <typename T>
void yarp::os::BufferedPort<T>::setAdminReader(PortReader& reader)
{
    attachIfNeeded();
    port.setAdminReader(reader);
}

template <typename T>
void yarp::os::BufferedPort<T>::onRead(T& datum)
{
    YARP_UNUSED(datum);
    // override this to do something
}

template <typename T>
void yarp::os::BufferedPort<T>::useCallback(TypedReaderCallback<T>& callback)
{
    attachIfNeeded();
    reader.useCallback(callback);
}

template <typename T>
void yarp::os::BufferedPort<T>::useCallback()
{
    attachIfNeeded();
    reader.useCallback(*this);
}

template <typename T>
void yarp::os::BufferedPort<T>::disableCallback()
{
    attachIfNeeded();
    reader.disableCallback();
}

template <typename T>
bool yarp::os::BufferedPort<T>::setEnvelope(PortWriter& envelope)
{
    return port.setEnvelope(envelope);
}


template <typename T>
bool yarp::os::BufferedPort<T>::getEnvelope(PortReader& envelope)
{
    return reader.getEnvelope(envelope);
}

template <typename T>
int yarp::os::BufferedPort<T>::getInputCount()
{
    return port.getInputCount();
}

template <typename T>
int yarp::os::BufferedPort<T>::getOutputCount()
{
    return port.getOutputCount();
}

template <typename T>
bool yarp::os::BufferedPort<T>::isWriting()
{
    return port.isWriting();
}

template <typename T>
void yarp::os::BufferedPort<T>::getReport(PortReport& reporter)
{
    port.getReport(reporter);
}

template <typename T>
void yarp::os::BufferedPort<T>::setReporter(PortReport& reporter)
{
    port.setReporter(reporter);
}

template <typename T>
void yarp::os::BufferedPort<T>::resetReporter()
{
    port.resetReporter();
}

template <typename T>
void* yarp::os::BufferedPort<T>::acquire()
{
    return reader.acquire();
}

template <typename T>
void yarp::os::BufferedPort<T>::release(void* handle)
{
    reader.release(handle);
}


template <typename T>
void yarp::os::BufferedPort<T>::setTargetPeriod(double period)
{
    attachIfNeeded();
    reader.setTargetPeriod(period);
}

template <typename T>
yarp::os::Type yarp::os::BufferedPort<T>::getType()
{
    return port.getType();
}

template <typename T>
void yarp::os::BufferedPort<T>::promiseType(const Type& typ)
{
    port.promiseType(typ);
}

template <typename T>
void yarp::os::BufferedPort<T>::setInputMode(bool expectInput)
{
    port.setInputMode(expectInput);
}

template <typename T>
void yarp::os::BufferedPort<T>::setOutputMode(bool expectOutput)
{
    port.setOutputMode(expectOutput);
}

template <typename T>
void yarp::os::BufferedPort<T>::setRpcMode(bool expectRpc)
{
    port.setRpcMode(expectRpc);
}

template <typename T>
yarp::os::Property* yarp::os::BufferedPort<T>::acquireProperties(bool readOnly)
{
    return port.acquireProperties(readOnly);
}

template <typename T>
void yarp::os::BufferedPort<T>::releaseProperties(Property* prop)
{
    port.releaseProperties(prop);
}

template <typename T>
void yarp::os::BufferedPort<T>::includeNodeInName(bool flag)
{
    return port.includeNodeInName(flag);
}

template <typename T>
bool yarp::os::BufferedPort<T>::setCallbackLock(yarp::os::Mutex* mutex)
{
    return port.setCallbackLock(mutex);
}

template <typename T>
bool yarp::os::BufferedPort<T>::removeCallbackLock()
{
    return port.removeCallbackLock();
}

template <typename T>
bool yarp::os::BufferedPort<T>::lockCallback()
{
    return port.lockCallback();
}

template <typename T>
bool yarp::os::BufferedPort<T>::tryLockCallback()
{
    return port.tryLockCallback();
}

template <typename T>
void yarp::os::BufferedPort<T>::unlockCallback()
{
    return port.unlockCallback();
}

template <typename T>
void yarp::os::BufferedPort<T>::attachIfNeeded()
{
    if (!attached) {
        reader.attach(port);
        writer.attach(port);
        attached = true;
    }
}

template <typename T>
bool yarp::os::BufferedPort<T>::sharedOpen(Port& port)
{
    bool ok = this->port.sharedOpen(port);
    if (!ok) {
        return false;
    }
    reader.attach(port);
    writer.attach(port);
    attached = true;
    return true;
}


#endif // YARP_OS_BUFFEREDPORT_INL_H
