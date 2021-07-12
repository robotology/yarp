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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/Mutex.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#include <mutex>

namespace yarp {
namespace os {
namespace impl {

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
#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    yarp::os::Mutex* old_recCallbackLock { nullptr };
YARP_WARNING_POP
#endif
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
#ifndef YARP_NO_DEPRECATED // Since YARP 3.3
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    YARP_DEPRECATED
    bool configCallbackLock(Mutex* lock);
YARP_WARNING_POP
#endif
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

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PORTCOREADAPTER_H
