/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ShmemOutputStream.h"
#include "ShmemLogComponent.h"

#include <yarp/conf/numeric.h>
#include <yarp/os/Log.h>

#include <climits>
#include <cstring>


ShmemOutputStreamImpl::ShmemOutputStreamImpl() :
        m_bOpen(false),
        m_ResizeNum(0),
        m_Port(0),
        m_pData(nullptr),
        m_pHeader(nullptr)
{
}

ShmemOutputStreamImpl::~ShmemOutputStreamImpl()
{
    close();
}

bool ShmemOutputStreamImpl::isOk() const
{
    return m_bOpen;
}

void ShmemOutputStreamImpl::unlinkAll()
{
    // Remove the names of all the objects created by this writer, they are
    // no longer needed (the objects survive until all the users close them)
    for (int i = 0; i <= m_ResizeNum; ++i) {
        shmem::SharedMemory::unlink(shmem::objectName("data", m_Port, i));
    }
    shmem::NamedSemaphore::unlink(shmem::objectName("mtx", m_Port));
    shmem::NamedSemaphore::unlink(shmem::objectName("sem", m_Port));
}

bool ShmemOutputStreamImpl::open(int port, int size)
{
    m_pData = nullptr;
    m_pHeader = nullptr;
    m_ResizeNum = 0;
    m_Port = port;
    m_OldMaps.clear();

    auto map = std::make_unique<shmem::SharedMemory>();
    if (!map->create(shmem::objectName("data", port, 0), size + sizeof(ShmemHeader_t))) {
        yCError(SHMEMCARRIER, "ShmemOutputStream can't create shared memory");
        return false;
    }

    if (!m_AccessMutex.create(shmem::objectName("mtx", port), 1, 1)
        || !m_WaitDataSem.create(shmem::objectName("sem", port), 0, INT_MAX)) {
        yCError(SHMEMCARRIER, "ShmemOutputStream can't create semaphores");
        m_AccessMutex.close();
        m_WaitDataSem.close();
        unlinkAll();
        return false;
    }

    m_pMap = std::move(map);
    m_pHeader = static_cast<ShmemHeader_t*>(m_pMap->data());
    m_pData = reinterpret_cast<char*>(m_pHeader + 1);

    m_AccessMutex.wait();

    m_pHeader->resize = false;
    m_pHeader->close = false;
    m_pHeader->attached = false;

    m_pHeader->avail = 0;
    m_pHeader->head = 0;
    m_pHeader->size = size;
    m_pHeader->newsize = size;
    m_pHeader->tail = 0;
    m_pHeader->waiting = 0;

    m_AccessMutex.post();

    m_bOpen = true;

    return true;
}

bool ShmemOutputStreamImpl::Resize(int newsize)
{
    yCDebug(SHMEMCARRIER, "output stream resize %d to %d", m_ResizeNum + 1, newsize);

    auto newMap = std::make_unique<shmem::SharedMemory>();
    if (!newMap->create(shmem::objectName("data", m_Port, m_ResizeNum + 1), newsize + sizeof(ShmemHeader_t))) {
        yCError(SHMEMCARRIER, "ShmemOutputStream can't create shared memory");
        return false;
    }
    ++m_ResizeNum;

    auto* pNewHeader = static_cast<ShmemHeader_t*>(newMap->data());
    char* pNewData = reinterpret_cast<char*>(pNewHeader + 1);

    pNewHeader->size = newsize;
    pNewHeader->newsize = newsize;
    pNewHeader->resize = false;
    pNewHeader->close = m_pHeader->close;
    pNewHeader->attached = false;

    pNewHeader->tail = 0;
    pNewHeader->head = pNewHeader->avail = m_pHeader->avail;
    pNewHeader->waiting = m_pHeader->waiting;

    if (m_pHeader->avail) {
        // one or two blocks in circular queue?
        if (m_pHeader->tail < m_pHeader->head) {
            memcpy(pNewData, m_pData + m_pHeader->tail, m_pHeader->avail);
        } else {
            int firstchunk = m_pHeader->size - m_pHeader->tail;
            memcpy(pNewData, m_pData + m_pHeader->tail, firstchunk);
            memcpy(pNewData + firstchunk, m_pData, m_pHeader->head);
        }
    }

    // Tell the reader to move to the new segment
    m_pHeader->newsize = newsize;
    m_pHeader->resize = true;

    m_OldMaps.push_back(std::move(m_pMap));
    m_pMap = std::move(newMap);

    m_pHeader = pNewHeader;
    m_pData = pNewData;

    return true;
}

bool ShmemOutputStreamImpl::write(const yarp::os::Bytes& b)
{
    if (!m_bOpen) {
        return false;
    }

    if (!m_AccessMutex.wait()) {
        return false;
    }

    if (!m_bOpen) {
        m_AccessMutex.post();
        return false;
    }

    if (m_pHeader->close) {
        m_AccessMutex.post();
        close();
        return false;
    }

    // The reader has moved to the current segment, the old ones are no
    // longer needed
    if (m_pHeader->attached && !m_OldMaps.empty()) {
        m_OldMaps.clear();
    }

    if ((int)m_pHeader->size - (int)m_pHeader->avail < (int)b.length()) {
        yarp::conf::ssize_t required = m_pHeader->size + 2 * b.length();
        if (!Resize((int)required)) {
            m_AccessMutex.post();
            close();
            return false;
        }
    }

    if ((int)m_pHeader->head + (int)b.length() <= (int)m_pHeader->size) {
        memcpy(m_pData + m_pHeader->head, b.get(), b.length());
    } else {
        int first_block_size = m_pHeader->size - m_pHeader->head;
        memcpy(m_pData + m_pHeader->head, b.get(), first_block_size);
        memcpy(m_pData, b.get() + first_block_size, b.length() - first_block_size);
    }

    m_pHeader->avail += (int)b.length();
    m_pHeader->head += (int)b.length();
    m_pHeader->head %= m_pHeader->size;

    while (m_pHeader->waiting > 0) {
        --m_pHeader->waiting;
        m_WaitDataSem.post();
    }

    m_AccessMutex.post();

    return true;
}

void ShmemOutputStreamImpl::close()
{
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

    m_OldMaps.clear();
    m_pMap.reset();
    m_pHeader = nullptr;
    m_pData = nullptr;

    unlinkAll();
}
