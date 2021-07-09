/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMINPUTSTREAM_H
#define YARP_SHMEM_SHMEMINPUTSTREAM_H

#include "ShmemTypes.h"

#include <yarp/os/InputStream.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

#include <mutex>

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



class ShmemInputStreamImpl
{
public:
    ShmemInputStreamImpl();
    ~ShmemInputStreamImpl();

    bool isOk() const;
    bool open(int port, ACE_SOCK_Stream* pSock, int size = SHMEM_DEFAULT_SIZE);
    yarp::conf::ssize_t read(yarp::os::Bytes& b);
    void close();

protected:
    int read(char* data, int len);
    bool Resize();
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

    std::mutex m_ReadSerializerMutex;

    ACE_Shared_Memory* m_pMap;
    char* m_pData;
    ShmemHeader_t* m_pHeader;

    ACE_SOCK_Stream* m_pSock;
};

#endif
