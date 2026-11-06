/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMINPUTSTREAM_H
#define YARP_SHMEM_SHMEMINPUTSTREAM_H

#include "ShmemPlatform.h"
#include "ShmemTypes.h"

#include <yarp/conf/numeric.h>
#include <yarp/os/Bytes.h>

#include <atomic>
#include <memory>
#include <mutex>


class ShmemInputStreamImpl
{
public:
    ShmemInputStreamImpl();
    ~ShmemInputStreamImpl();

    bool isOk() const;
    bool open(int port, shmem::Socket* pSock);
    yarp::conf::ssize_t read(yarp::os::Bytes& b);
    void close();

protected:
    int read(char* data, int len);
    bool Resize();
    bool m_bOpen;

    // Set by close() before waiting for a pending read to finish, so that a
    // read waiting for data can give up
    std::atomic<bool> m_bInterrupt;

    int m_ResizeNum;
    int m_Port;

    // Protects the shared buffer. A semaphore (used as a binary mutex) is
    // used instead of a mutex, since it can be released by any thread.
    shmem::NamedSemaphore m_AccessMutex;

    // Signalled by the writer when new data is available. It must be a
    // semaphore and not a mutex, since it is released by a different
    // process (and thread) than the one waiting on it.
    shmem::NamedSemaphore m_WaitDataSem;

    std::mutex m_ReadSerializerMutex;

    // Mutex for the whole class, to avoid concurrent close and read
    std::recursive_mutex m_generalMutex;

    std::unique_ptr<shmem::SharedMemory> m_pMap;
    char* m_pData;
    ShmemHeader_t* m_pHeader;

    shmem::Socket* m_pSock;
};

#endif
