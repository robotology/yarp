/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCoreAdapter.h>

#include <yarp/os/PortReader.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/LogComponent.h>

namespace {
YARP_OS_LOG_COMPONENT(PORTCOREADAPTER, "yarp.os.impl.PortCoreAdapter")
} // namespace

yarp::os::impl::PortCoreAdapter::PortCoreAdapter(Port& owner)
{
    setContactable(&owner);
}

void yarp::os::impl::PortCoreAdapter::openable()
{
    stateMutex.lock();
    closed = false;
    opened = true;
    stateMutex.unlock();
}

void yarp::os::impl::PortCoreAdapter::alertOnRead()
{
    usedForRead = true;
}

void yarp::os::impl::PortCoreAdapter::alertOnWrite()
{
    usedForWrite = true;
}

void yarp::os::impl::PortCoreAdapter::alertOnRpc()
{
    usedForRpc = true;
}

void yarp::os::impl::PortCoreAdapter::setReadOnly()
{
    commitToRead = true;
}

void yarp::os::impl::PortCoreAdapter::setWriteOnly()
{
    commitToWrite = true;
}

void yarp::os::impl::PortCoreAdapter::setRpc()
{
    commitToRpc = true;
}

void yarp::os::impl::PortCoreAdapter::finishReading()
{
    if (!readBackground) {
        stateMutex.lock();
        closed = true;
        consume.post();
        consume.post();
        stateMutex.unlock();
    }
}

void yarp::os::impl::PortCoreAdapter::finishWriting()
{
    if (isWriting()) {
        double start = SystemClock::nowSystem();
        double pause = 0.01;
        do {
            SystemClock::delaySystem(pause);
            pause *= 2;
        } while (isWriting() && (SystemClock::nowSystem() - start < 3));
        if (isWriting()) {
            yCError(PORTCOREADAPTER, "Closing port that was sending data (slowly)");
        }
    }
}


void yarp::os::impl::PortCoreAdapter::resumeFull()
{
    while (produce.check()) {
    }
    while (readBlock.check()) {
    }
    resume();
    readBlock.post();
}

bool yarp::os::impl::PortCoreAdapter::read(ConnectionReader& reader)
{
    if (permanentReadDelegate != nullptr) {
        bool result = permanentReadDelegate->read(reader);
        return result;
    }

    // called by comms code
    readBlock.wait();

    if (!reader.isValid()) {
        // interrupt
        stateMutex.lock();
        if (readDelegate != nullptr) {
            readResult = readDelegate->read(reader);
        }
        stateMutex.unlock();
        produce.post();
        readBlock.post();
        return false;
    }

    if (closed) {
        yCDebug(PORTCOREADAPTER, "Port::read shutting down");
        readBlock.post();
        return false;
    }

    // wait for happy consumer - don't want to miss a packet
    if (!readBackground) {
        consume.wait();
    }

    stateMutex.lock();
    readResult = false;
    if (readDelegate != nullptr) {
        readResult = readDelegate->read(reader);
    } else {
        // read and ignore
        yCDebug(PORTCOREADAPTER, "data received in Port, no reader for it");
        Bottle b;
        b.read(reader);
    }
    if (!readBackground) {
        readDelegate = nullptr;
        writeDelegate = nullptr;
    }
    bool result = readResult;
    stateMutex.unlock();
    if (!readBackground) {
        produce.post();
    }
    if (result && willReply) {
        consume.wait();
        if (closed) {
            yCDebug(PORTCOREADAPTER, "Port::read shutting down");
            readBlock.post();
            return false;
        }
        if (writeDelegate != nullptr) {
            stateMutex.lock();
            ConnectionWriter* writer = reader.getWriter();
            if (writer != nullptr) {
                result = readResult = writeDelegate->write(*writer);
            }
            stateMutex.unlock();
        }
        if (dropDue) {
            reader.requestDrop();
        }
        produce.post();
    }
    readBlock.post();
    return result;
}

bool yarp::os::impl::PortCoreAdapter::read(PortReader& reader, bool willReply)
{
    // called by user

    // user claimed they would reply to last read, but then
    // decided not to.
    if (replyDue) {
        Bottle emptyMessage;
        reply(emptyMessage, false, false);
        replyDue = false;
        dropDue = false;
    }
    if (willReply) {
        replyDue = true;
    }

    stateMutex.lock();
    readActive = true;
    readDelegate = &reader;
    checkType(reader);
    writeDelegate = nullptr;
    this->willReply = willReply;
    consume.post(); // happy consumer
    stateMutex.unlock();

    produce.wait();
    stateMutex.lock();
    if (!readBackground) {
        readDelegate = nullptr;
    }
    bool result = readResult;
    if (!result) {
        replyDue = false;
    }
    stateMutex.unlock();
    return result;
}

bool yarp::os::impl::PortCoreAdapter::reply(PortWriter& writer, bool drop, bool /*interrupted*/)
{
    // send reply even if interrupt has happened in interim
    if (!replyDue) {
        return false;
    }

    replyDue = false;
    dropDue = drop;
    writeDelegate = &writer;
    consume.post();
    produce.wait();
    bool result = readResult;
    return result;
}

/*
    Configuration of a port that should be remembered
    between opens and closes
*/

void yarp::os::impl::PortCoreAdapter::configReader(PortReader& reader)
{
    stateMutex.lock();
    readActive = true;
    readBackground = true;
    readDelegate = &reader;
    permanentReadDelegate = &reader;
    checkType(reader);
    consume.post(); // just do this once
    stateMutex.unlock();
}

void yarp::os::impl::PortCoreAdapter::configAdminReader(PortReader& reader)
{
    stateMutex.lock();
    adminReadDelegate = &reader;
    setAdminReadHandler(reader);
    stateMutex.unlock();
}

void yarp::os::impl::PortCoreAdapter::configReadCreator(PortReaderCreator& creator)
{
    recReadCreator = &creator;
    setReadCreator(creator);
}

void yarp::os::impl::PortCoreAdapter::configWaitAfterSend(bool waitAfterSend)
{
    if (waitAfterSend && isManual()) {
        yCError(PORTCOREADAPTER, "Cannot use background-mode writes on a fake port");
    }
    recWaitAfterSend = waitAfterSend ? 1 : 0;
    setWaitAfterSend(waitAfterSend);
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
bool yarp::os::impl::PortCoreAdapter::configCallbackLock(Mutex* lock)
{
    recCallbackLock = nullptr;
    old_recCallbackLock = lock;
    haveCallbackLock = true;
    return setCallbackLock(lock);
}
YARP_WARNING_POP
#endif

bool yarp::os::impl::PortCoreAdapter::configCallbackLock(std::mutex* lock)
{
    recCallbackLock = lock;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
    old_recCallbackLock = nullptr;
#endif
    haveCallbackLock = true;
    return setCallbackLock(lock);
}

bool yarp::os::impl::PortCoreAdapter::unconfigCallbackLock()
{
    recCallbackLock = nullptr;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
    old_recCallbackLock = nullptr;
#endif
    haveCallbackLock = false;
    return removeCallbackLock();
}

yarp::os::PortReader* yarp::os::impl::PortCoreAdapter::checkPortReader()
{
    return readDelegate;
}

yarp::os::PortReader* yarp::os::impl::PortCoreAdapter::checkAdminPortReader()
{
    return adminReadDelegate;
}

yarp::os::PortReaderCreator* yarp::os::impl::PortCoreAdapter::checkReadCreator()
{
    return recReadCreator;
}

int yarp::os::impl::PortCoreAdapter::checkWaitAfterSend()
{
    return recWaitAfterSend;
}


bool yarp::os::impl::PortCoreAdapter::isOpened()
{
    return opened;
}

void yarp::os::impl::PortCoreAdapter::setOpen(bool opened)
{
    this->opened = opened;
}

void yarp::os::impl::PortCoreAdapter::includeNodeInName(bool flag)
{
    includeNode = flag;
}
