/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMOUTPUTSTREAM_H
#define YARP_SHMEM_SHMEMOUTPUTSTREAM_H


#include "ShmemTypes.h"

#include <yarp/os/OutputStream.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

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
// In one the ACE headers there is a definition of "main" for WIN32
# ifdef main
#  undef main
# endif


class ShmemOutputStreamImpl
{
public:
    ShmemOutputStreamImpl();
    ~ShmemOutputStreamImpl();

    bool isOk() const;
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

#endif // YARP_SHMEM_SHMEMOUTPUTSTREAM_H
