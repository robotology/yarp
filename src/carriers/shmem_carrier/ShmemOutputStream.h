/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMOUTPUTSTREAM_H
#define YARP_SHMEM_SHMEMOUTPUTSTREAM_H

#include "ShmemPlatform.h"
#include "ShmemTypes.h"

#include <yarp/os/Bytes.h>

#include <memory>
#include <vector>


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
    void unlinkAll();

    bool m_bOpen;

    int m_ResizeNum;
    int m_Port;

    // Protects the shared buffer. A semaphore (used as a binary mutex) is
    // used instead of a mutex, since it can be released by any thread.
    shmem::NamedSemaphore m_AccessMutex;

    // Signalled by the writer when new data is available. It must be a
    // semaphore and not a mutex, since it is released by a different
    // process (and thread) than the one waiting on it.
    shmem::NamedSemaphore m_WaitDataSem;

    std::unique_ptr<shmem::SharedMemory> m_pMap;
    // Segments replaced by a resize, kept alive until the reader has mapped
    // the new segment (on Windows a segment is destroyed when the last handle
    // is closed).
    std::vector<std::unique_ptr<shmem::SharedMemory>> m_OldMaps;
    char* m_pData;
    ShmemHeader_t* m_pHeader;
};

#endif // YARP_SHMEM_SHMEMOUTPUTSTREAM_H
