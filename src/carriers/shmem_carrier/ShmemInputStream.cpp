/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ShmemInputStream.h"
#include "ShmemLogComponent.h"

#include <yarp/os/Log.h>

#include <cstring>


ShmemInputStreamImpl::ShmemInputStreamImpl() :
        m_bOpen(false),
        m_bInterrupt(false),
        m_ResizeNum(0),
        m_Port(-1),
        m_pData(nullptr),
        m_pHeader(nullptr),
        m_pSock(nullptr)
{
}

ShmemInputStreamImpl::~ShmemInputStreamImpl()
{
    close();
}

bool ShmemInputStreamImpl::isOk() const
{
    return m_bOpen;
}

bool ShmemInputStreamImpl::open(int port, shmem::Socket* pSock)
{
    std::lock_guard<std::recursive_mutex> l_guard(m_generalMutex);

    m_pSock = pSock;
    m_bInterrupt = false;

    m_pData = nullptr;
    m_pHeader = nullptr;
    m_ResizeNum = 0;

    m_Port = port;

    // The objects are created by the writer before the handshake. Once they
    // are opened by the reader, their names are no longer needed, and they
    // are removed, so that nothing is left behind if a process crashes.
    const std::string data_name = shmem::objectName("data", port, 0);
    auto map = std::make_unique<shmem::SharedMemory>();
    if (!map->open(data_name)) {
        yCError(SHMEMCARRIER, "ShmemInputStream can't open shared memory");
        return false;
    }
    shmem::SharedMemory::unlink(data_name);

    const std::string mtx_name = shmem::objectName("mtx", port);
    const std::string sem_name = shmem::objectName("sem", port);
    if (!m_AccessMutex.open(mtx_name) || !m_WaitDataSem.open(sem_name)) {
        yCError(SHMEMCARRIER, "ShmemInputStream can't open semaphores");
        m_AccessMutex.close();
        m_WaitDataSem.close();
        return false;
    }
    shmem::NamedSemaphore::unlink(mtx_name);
    shmem::NamedSemaphore::unlink(sem_name);

    m_pMap = std::move(map);
    m_pHeader = static_cast<ShmemHeader_t*>(m_pMap->data());
    m_pData = reinterpret_cast<char*>(m_pHeader + 1);

    m_AccessMutex.wait();
    m_pHeader->attached = true;
    m_AccessMutex.post();

    m_bOpen = true;

    return true;
}

bool ShmemInputStreamImpl::Resize()
{
    std::lock_guard<std::recursive_mutex> l_guard(m_generalMutex);

    yCDebug(SHMEMCARRIER, "input stream resize %d to %d", m_ResizeNum + 1, m_pHeader->newsize);

    const std::string data_name = shmem::objectName("data", m_Port, m_ResizeNum + 1);
    auto pNewMap = std::make_unique<shmem::SharedMemory>();
    if (!pNewMap->open(data_name)) {
        yCError(SHMEMCARRIER, "ShmemInputStream can't open shared memory");
        return false;
    }
    shmem::SharedMemory::unlink(data_name);
    ++m_ResizeNum;

    m_pMap = std::move(pNewMap);
    m_pHeader = static_cast<ShmemHeader_t*>(m_pMap->data());
    m_pData = reinterpret_cast<char*>(m_pHeader + 1);

    // Tell the writer that the previous segments can be released
    m_pHeader->attached = true;

    return true;
}

int ShmemInputStreamImpl::read(char* data, int len)
{
    std::lock_guard<std::recursive_mutex> l_guard(m_generalMutex);

    if (!m_AccessMutex.wait()) {
        return -1;
    }

    if (m_pHeader->close) {
        m_AccessMutex.post();
        close();
        return -1;
    }

    while (m_pHeader->resize) {
        if (!Resize()) {
            m_AccessMutex.post();
            close();
            return -1;
        }
    }

    if (m_pHeader->avail < len) {
        ++m_pHeader->waiting;
        m_AccessMutex.post();
        return 0;
    }

    if (m_pHeader->tail + len > m_pHeader->size) {
        int first_block_size = m_pHeader->size - m_pHeader->tail;

        memcpy((void*)data, (void*)(m_pData + m_pHeader->tail), first_block_size);
        memcpy((void*)(data + first_block_size), (void*)m_pData, len - first_block_size);
    } else {
        memcpy((void*)data, (void*)(m_pData + m_pHeader->tail), len);
    }

    m_pHeader->avail -= len;
    m_pHeader->tail += len;
    m_pHeader->tail %= m_pHeader->size;

    m_AccessMutex.post();

    return len;
}

yarp::conf::ssize_t ShmemInputStreamImpl::read(yarp::os::Bytes& b)
{
    std::lock_guard<std::recursive_mutex> l_guard(m_generalMutex);
    std::lock_guard<std::mutex> l_serializer(m_ReadSerializerMutex);

    if (!m_bOpen) {
        return -1;
    }

    char* data = b.get();
    size_t len = b.length();
    if (len == 0) {
        return 0;
    }

    yarp::conf::ssize_t ret;
    while (!(ret = read(data, (int)len))) {
        m_WaitDataSem.waitFor(1.0);

        if (m_bInterrupt) {
            return -1;
        }

        // The socket is used only to detect if the other side closed the
        // connection or died
        if (m_pSock->isPeerClosed()) {
            yCDebug(SHMEMCARRIER, "STREAM IS BROKEN");
            close();
            return -1;
        }
    }

    return ret;
}

void ShmemInputStreamImpl::close()
{
    // Make a pending read give up, otherwise it would hold the lock until new
    // data is available
    m_bInterrupt = true;

    std::lock_guard<std::recursive_mutex> l_guard(m_generalMutex);

    if (!m_bOpen) {
        return;
    }

    m_bOpen = false;

    m_AccessMutex.wait();
    while (m_pHeader->waiting > 0) {
        --m_pHeader->waiting;
        m_WaitDataSem.post();
    }
    m_pHeader->close = true;
    m_AccessMutex.post();

    m_AccessMutex.close();
    m_WaitDataSem.close();

    m_pMap.reset();
    m_pHeader = nullptr;
    m_pData = nullptr;
}
