/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ShmemInputStream.h"
#include <yarp/os/impl/PlatformTime.h>
#include <ace/Lib_Find.h>


ShmemInputStreamImpl::ShmemInputStreamImpl() :
        m_bOpen(false),
        m_ResizeNum(0),
        m_Port(-1),
        m_pAccessMutex(nullptr),
        m_pWaitDataMutex(nullptr),
        m_pMap(nullptr),
        m_pData(nullptr),
        m_pHeader(nullptr),
        m_pSock(nullptr)
{
}

ShmemInputStreamImpl::~ShmemInputStreamImpl()
{
    close();
}

bool ShmemInputStreamImpl::isOk()
{
    return m_bOpen;
}

bool ShmemInputStreamImpl::open(int port, ACE_SOCK_Stream* pSock, int size)
{
    m_pSock = pSock;

    m_pAccessMutex = m_pWaitDataMutex = nullptr;
    m_pMap = nullptr;
    m_pData = nullptr;
    m_pHeader = nullptr;
    m_ResizeNum = 0;

    m_Port = port;

    char obj_name[1024];
    char temp_dir_path[1024];

    if (ACE::get_temp_dir(temp_dir_path, 1024) == -1) {
        yError("ShmemHybridStream: no temp directory found.");
        return false;
    }

#ifdef ACE_LACKS_SYSV_SHMEM

    sprintf(obj_name, "%sSHMEM_FILE_%d_%d", temp_dir_path, port, 0);

    m_pMap = new ACE_Shared_Memory_MM(obj_name, //const ACE_TCHAR *filename,
                                      size + sizeof(ShmemHeader_t), //int len = -1,
                                      O_RDWR, //int flags = O_RDWR | O_CREAT,
                                      ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
                                      PROT_RDWR, //int prot = PROT_RDWR,
                                      ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

    m_pMap = new ACE_Shared_Memory_SV(port, size + sizeof(ShmemHeader_t));

#endif

    m_pHeader = (ShmemHeader_t*)m_pMap->malloc();
    m_pData = (char*)(m_pHeader + 1);

#ifdef _ACE_USE_SV_SEM
    sprintf(obj_name, "%sSHMEM_ACCESS_MUTEX_%d", temp_dir_path, port);
    m_pAccessMutex = new ACE_Mutex(USYNC_PROCESS, obj_name);
    sprintf(obj_name, "%sSHMEM_WAITDATA_MUTEX_%d", temp_dir_path, port);
    m_pWaitDataMutex = new ACE_Mutex(USYNC_PROCESS, obj_name);
#else
    sprintf(obj_name, "SHMEM_ACCESS_MUTEX_%d", port);
    m_pAccessMutex = new ACE_Process_Mutex(obj_name);
    sprintf(obj_name, "SHMEM_WAITDATA_MUTEX_%d", port);
    m_pWaitDataMutex = new ACE_Process_Mutex(obj_name);
#endif

    m_pWaitDataMutex->acquire();

    m_bOpen = true;

    return true;
}

bool ShmemInputStreamImpl::Resize()
{
    ++m_ResizeNum;

    ACE_Shared_Memory* pNewMap;

    //yDebug("input stream resize %d to %d", m_ResizeNum, m_pHeader->newsize);

#ifdef ACE_LACKS_SYSV_SHMEM

    char file_path[1024];

    if (ACE::get_temp_dir(file_path, 1024) == -1) {
        yError("ShmemHybridStream: no temp directory found.");
        return false;
    }

    char file_name[1024];
    sprintf(file_name, "%sSHMEM_FILE_%d_%d", file_path, m_Port, m_ResizeNum);

    pNewMap = new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
                                       m_pHeader->newsize + sizeof(ShmemHeader_t), //int len = -1,
                                       O_RDWR, //int flags = O_RDWR | O_CREAT,
                                       ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
                                       PROT_RDWR, //int prot = PROT_RDWR,
                                       ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

    int shmemkey = (m_ResizeNum << 16) + m_Port;

    pNewMap = new ACE_Shared_Memory_SV(shmemkey, m_pHeader->size + sizeof(ShmemHeader_t));

#endif

    if (!pNewMap) {
        yError("ShmemOutputStream can't create shared memory");
        return false;
    }

    ShmemHeader_t* pNewHeader = (ShmemHeader_t*)pNewMap->malloc();
    char* pNewData = (char*)(pNewHeader + 1);

    m_pMap->close();
    delete m_pMap;

    m_pMap = pNewMap;
    m_pHeader = pNewHeader;
    m_pData = pNewData;

    return true;
}

int ShmemInputStreamImpl::read(char* data, int len)
{
    m_pAccessMutex->acquire();

    if (m_pHeader->close) {
        m_pAccessMutex->release();
        close();
        return -1;
    }

    while (m_pHeader->resize)
        Resize();

    if (m_pHeader->avail < len) {
        ++m_pHeader->waiting;
        m_pAccessMutex->release();
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

    m_pAccessMutex->release();

    return len;
}

YARP_SSIZE_T ShmemInputStreamImpl::read(const yarp::os::Bytes& b)
{
    m_ReadSerializerMutex.wait();

    if (!m_bOpen) {
        m_ReadSerializerMutex.post();
        return -1;
    }

    char *data = b.get(), buf;
    size_t len = b.length();
    YARP_SSIZE_T ret;

    while (!(ret = read(data, (int)len))) {
#ifdef _ACE_USE_SV_SEM
        yarp::os::impl::YARP_timeval tv = ACE_OS::gettimeofday();
        tv.sec(tv.sec() + 1);
#else
        yarp::os::impl::YARP_timeval tv(1);
#endif

        m_pWaitDataMutex->acquire(tv);

        if (!m_pSock->recv(&buf, 1)) {
            //yDebug("STREAM IS BROKEN");
            close();
            m_ReadSerializerMutex.post();
            return -1;
        }
    }

    m_ReadSerializerMutex.post();

    return ret;
}

void ShmemInputStreamImpl::close()
{
    if (!m_bOpen)
        return;

    m_bOpen = false;

    m_pAccessMutex->acquire();
    while (m_pHeader->waiting > 0) {
        --m_pHeader->waiting;
        m_pWaitDataMutex->release();
    }
    m_pHeader->close = true;
    m_pAccessMutex->release();

    m_pAccessMutex->remove();
    delete m_pAccessMutex;
    m_pAccessMutex = nullptr;

    m_pWaitDataMutex->remove();
    delete m_pWaitDataMutex;
    m_pWaitDataMutex = nullptr;

    m_pMap->close();
    delete m_pMap;
    m_pMap = nullptr;
}
