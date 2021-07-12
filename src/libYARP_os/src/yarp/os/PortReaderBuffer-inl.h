/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTREADERBUFFER_INL_H
#define YARP_OS_PORTREADERBUFFER_INL_H


template <typename T>
yarp::os::PortReaderBuffer<T>::PortReaderBuffer(unsigned int maxBuffer) :
        implementation(maxBuffer),
        autoDiscard(true),
        last(nullptr),
        default_value(nullptr),
        reader(nullptr)
{
    implementation.setCreator(this);
    setStrict(false);
}

template <typename T>
yarp::os::PortReaderBuffer<T>::~PortReaderBuffer()
{
    detach();
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::detach()
{
    // it would also help to close the port, so
    // that incoming inputs are interrupted
    if (reader != nullptr) {
        reader->stop();
        delete reader;
        reader = nullptr;
    }
    if (default_value != nullptr) {
        delete default_value;
        default_value = nullptr;
    }
    last = nullptr;
    implementation.clear();
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::setStrict(bool strict)
{
    autoDiscard = !strict;
    // do discard at earliest time possible
    implementation.setPrune(autoDiscard);
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::check()
{
    return implementation.check() > 0;
}

template <typename T>
int yarp::os::PortReaderBuffer<T>::getPendingReads()
{
    return implementation.check();
}


template <typename T>
T* yarp::os::PortReaderBuffer<T>::read(bool shouldWait)
{
    if (!shouldWait) {
        if (!check()) {
            last = nullptr;
            return last;
        }
    }
    bool missed = false;
    T* prev = last;
    last = static_cast<T*>(implementation.readBase(missed, false));
    if (last != nullptr) {
        if (autoDiscard) {
            // go up to date
            while (check()) {
                // printf("Dropping something\n");
                bool tmp;
                last = static_cast<T*>(implementation.readBase(tmp, true));
            }
        }
    }
    if (missed) {
        // we've been asked to enforce a period
        last = prev;
        if (last == nullptr) {
            if (default_value == nullptr) {
                default_value = new T;
            }
            last = default_value;
        }
    }
    return last;
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::interrupt()
{
    implementation.interrupt();
}

template <typename T>
T* yarp::os::PortReaderBuffer<T>::lastRead()
{
    return last;
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::attach(Port& port)
{
    // port.setReader(*this);
    implementation.attachBase(port);
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::useCallback(TypedReaderCallback<T>& callback)
{
    if (reader != nullptr) {
        reader->stop();
        delete reader;
        reader = nullptr;
    }
    reader = new TypedReaderThread<T>(*this, callback);
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::disableCallback()
{
    if (reader != nullptr) {
        reader->stop();
        delete reader;
        reader = nullptr;
    }
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::read(ConnectionReader& connection)
{
    return implementation.read(connection);
}

template <typename T>
yarp::os::PortReader* yarp::os::PortReaderBuffer<T>::create() const
{
    return new T;
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::setReplier(PortReader& reader)
{
    implementation.setReplier(reader);
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::getEnvelope(PortReader& envelope)
{
    return implementation.getEnvelope(envelope);
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::isClosed()
{
    return implementation.isClosed();
}

template <typename T>
std::string yarp::os::PortReaderBuffer<T>::getName() const
{
    return implementation.getName();
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::acceptObject(T* obj, PortWriter* wrapper)
{
    return implementation.acceptObjectBase(obj, wrapper);
}

template <typename T>
bool yarp::os::PortReaderBuffer<T>::forgetObject(T* obj, yarp::os::PortWriter* wrapper)
{
    return implementation.forgetObjectBase(obj, wrapper);
}

template <typename T>
void* yarp::os::PortReaderBuffer<T>::acquire()
{
    return implementation.acquire();
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::release(void* handle)
{
    implementation.release(handle);
}

template <typename T>
void yarp::os::PortReaderBuffer<T>::setTargetPeriod(double period)
{
    implementation.setTargetPeriod(period);
}


#endif // YARP_OS_PORTREADERBUFFER_INL_H
