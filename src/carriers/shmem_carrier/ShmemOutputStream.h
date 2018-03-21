/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_SHMEMOUTPUTSTREAM_H
#define YARP_OS_IMPL_SHMEMOUTPUTSTREAM_H

#include <ace/config.h>
#include <ace/Mutex.h>
#include <ace/Process_Mutex.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#if defined(ACE_LACKS_SYSV_SHMEM)
#include <ace/Shared_Memory_MM.h>
#else
#include <ace/Shared_Memory_SV.h>
#endif

#include <yarp/os/OutputStream.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/Logger.h>

#include "ShmemTypes.h"

class ShmemOutputStreamImpl
{
public:
    ShmemOutputStreamImpl();
    ~ShmemOutputStreamImpl();

    bool isOk();
    bool open(int port, int size = SHMEM_DEFAULT_SIZE);
    bool write(const yarp::os::Bytes& b);
    void close();

protected:
    bool Resize(int newsize);

    bool m_bOpen;

    int m_ResizeNum;
    int m_Port;

#if defined(_ACE_USE_SV_SEM)
    ACE_Mutex* m_pAccessMutex;
    ACE_Mutex* m_pWaitDataMutex;
#else
    ACE_Process_Mutex* m_pAccessMutex;
    ACE_Process_Mutex* m_pWaitDataMutex;
#endif

    ACE_Shared_Memory* m_pMap;
    char* m_pData;
    ShmemHeader_t* m_pHeader;
};

#endif // YARP_OS_IMPL_SHMEMOUTPUTSTREAM_H
