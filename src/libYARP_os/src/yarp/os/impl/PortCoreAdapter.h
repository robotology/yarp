/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PORTCOREADAPTER_H
#define YARP_OS_IMPL_PORTCOREADAPTER_H

#include <yarp/os/Port.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/PortCore.h>
#include <mutex>

namespace yarp::os::impl {

class PortCoreAdapter :
        public PortCore
{
private:
    std::mutex stateMutex;
    PortReader* readDelegate { nullptr };
    PortReader* permanentReadDelegate { nullptr };
    PortReader* adminReadDelegate { nullptr };
    PortWriter* writeDelegate { nullptr };
    //PortReaderCreator *readCreatorDelegate { nullptr };
    bool readResult { false };
    bool readActive { false };
    bool readBackground { false };
    bool willReply { false };
    bool closed { false };
    bool opened { false };
    bool replyDue { false };
    bool dropDue { false };
    yarp::os::Semaphore produce { 0 };
    yarp::os::Semaphore consume { 0 };
    yarp::os::Semaphore readBlock { 1 };
    PortReaderCreator* recReadCreator { nullptr };
    int recWaitAfterSend { -1 };
    bool usedForRead { false };
    bool usedForWrite { false };
    bool usedForRpc { false };

public:
    bool includeNode { false };
    bool commitToRead { false };
    bool commitToWrite { false };
    bool commitToRpc { false };
    bool active { false };
    std::mutex* recCallbackLock { nullptr };
    bool haveCallbackLock { false };

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
    bool configCallbackLock(std::mutex* lock);
    bool unconfigCallbackLock();
    PortReader* checkPortReader();
    PortReader* checkAdminPortReader();
    PortReaderCreator* checkReadCreator();
    int checkWaitAfterSend();
    bool isOpened();
    void setOpen(bool opened);
    void includeNodeInName(bool flag);
};

} // namespace yarp::os::impl

#endif // YARP_OS_IMPL_PORTCOREADAPTER_H
