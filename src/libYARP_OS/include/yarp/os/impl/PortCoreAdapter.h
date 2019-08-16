/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PORTCOREADAPTER_H
#define YARP_OS_IMPL_PORTCOREADAPTER_H

#include <yarp/os/Mutex.h>
#include <yarp/os/Port.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/PortCore.h>

#include <mutex>

namespace yarp {
namespace os {
namespace impl {

class PortCoreAdapter :
        public PortCore
{
private:
    std::mutex stateMutex;
    PortReader* readDelegate;
    PortReader* permanentReadDelegate;
    PortReader* adminReadDelegate;
    PortWriter* writeDelegate;
    //PortReaderCreator *readCreatorDelegate;
    bool readResult;
    bool readActive;
    bool readBackground;
    bool willReply;
    bool closed;
    bool opened;
    bool replyDue;
    bool dropDue;
    yarp::os::Semaphore produce;
    yarp::os::Semaphore consume;
    yarp::os::Semaphore readBlock;
    PortReaderCreator* recReadCreator;
    int recWaitAfterSend;
    bool usedForRead;
    bool usedForWrite;
    bool usedForRpc;

public:
    bool includeNode;
    bool commitToRead;
    bool commitToWrite;
    bool commitToRpc;
    bool active;
    Mutex* recCallbackLock;
    bool haveCallbackLock;

    PortCoreAdapter(Port& owner);
    void openable();
    void alertOnRead();
    void alertOnWrite();
    void alertOnRpc();
    void setReadOnly();
    void setWriteOnly();
    void setRpc();
    void finishReading();
    void finishWriting();
    void resumeFull();
    bool read(ConnectionReader& reader) override;
    bool read(PortReader& reader, bool willReply = false);
    bool reply(PortWriter& writer, bool drop, bool interrupted);
    void configReader(PortReader& reader);
    void configAdminReader(PortReader& reader);
    void configReadCreator(PortReaderCreator& creator);
    void configWaitAfterSend(bool waitAfterSend);
    bool configCallbackLock(Mutex* lock);
    bool unconfigCallbackLock();
    PortReader* checkPortReader();
    PortReader* checkAdminPortReader();
    PortReaderCreator* checkReadCreator();
    int checkWaitAfterSend();
    bool isOpened();
    void setOpen(bool opened);
    void includeNodeInName(bool flag);
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREADAPTER_H
