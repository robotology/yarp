/*
* Author: Alessandro Scalzo alessandro@liralab.it
* Copyright (C) 2007 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/ShmemOutputStream.h>
#include <ace/Lib_Find.h>

using namespace yarp::os::impl;
using namespace yarp::os::impl;

using namespace yarp::os;

bool ShmemOutputStreamImpl::open(int port,int size)
{
    m_pAccessMutex=m_pWaitDataMutex=0;
    m_pMap=0;
    m_pData=0;
    m_pHeader=0;
    m_ResizeNum=0;
    m_Port=port;

    char obj_name[1024];
    char temp_dir_path[1024];

    if (ACE::get_temp_dir(temp_dir_path,1024)==-1)
    {
        YARP_ERROR(Logger::get(),"ShmemHybridStream: no temp directory found.");
        return false;
    }

#ifdef ACE_LACKS_SYSV_SHMEM

    sprintf(obj_name,"%sSHMEM_FILE_%d_0",temp_dir_path,port);

    m_pMap=new ACE_Shared_Memory_MM(obj_name, //const ACE_TCHAR *filename,
        size+sizeof(ShmemHeader_t), //int len = -1,
        O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
        ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
        PROT_RDWR, //int prot = PROT_RDWR,
        ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

    m_pMap=new ACE_Shared_Memory_SV(port,size+sizeof(ShmemHeader_t),ACE_Shared_Memory_SV::ACE_CREATE);

#endif

    m_pHeader=(ShmemHeader_t*)m_pMap->malloc();
    m_pData=(char*)(m_pHeader+1);

#ifdef _ACE_USE_SV_SEM
    sprintf(obj_name,"%sSHMEM_ACCESS_MUTEX_%d",temp_dir_path,port);
    m_pAccessMutex=new ACE_Mutex(USYNC_PROCESS,obj_name);
    sprintf(obj_name,"%sSHMEM_WAITDATA_MUTEX_%d",temp_dir_path,port);
    m_pWaitDataMutex=new ACE_Mutex(USYNC_PROCESS,obj_name);
#else
    sprintf(obj_name,"SHMEM_ACCESS_MUTEX_%d",port);
    m_pAccessMutex=new ACE_Process_Mutex(obj_name);
    sprintf(obj_name,"SHMEM_WAITDATA_MUTEX_%d",port);
    m_pWaitDataMutex=new ACE_Process_Mutex(obj_name);
#endif

    m_pAccessMutex->acquire();

    m_pHeader->resize=false;
    m_pHeader->close=false;

    m_pHeader->avail=0;
    m_pHeader->head=0;
    m_pHeader->size=size;
    m_pHeader->tail=0;
    m_pHeader->waiting=0;

    m_pAccessMutex->release();

    m_bOpen=true;

    return true;
}

bool ShmemOutputStreamImpl::Resize(int newsize)
{
    ++m_ResizeNum;

    //printf("output stream resize %d to %d\n",m_ResizeNum,newsize);
    //fflush(stdout);

    ACE_Shared_Memory* pNewMap;

    m_pHeader->resize=true;
    m_pHeader->newsize=newsize;

#ifdef ACE_LACKS_SYSV_SHMEM

    char file_path[1024];

    if (ACE::get_temp_dir(file_path,1024)==-1)
    {
        YARP_ERROR(Logger::get(),"ShmemHybridStream: no temp directory found.");
        return false;
    }

    char file_name[1024];
    sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_Port,m_ResizeNum);

    pNewMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
        newsize+sizeof(ShmemHeader_t), //int len = -1,
        O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
        ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
        PROT_RDWR, //int prot = PROT_RDWR,
        ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

    int shmemkey=(m_ResizeNum<<16)+m_Port;

    pNewMap=new ACE_Shared_Memory_SV(shmemkey,newsize+sizeof(ShmemHeader_t),ACE_Shared_Memory_SV::ACE_CREATE);

#endif

    if (!pNewMap)
    {
        YARP_ERROR(Logger::get(),ConstString("ShmemOutputStream can't create shared memory"));
        return false;
    }

    ShmemHeader_t *pNewHeader=(ShmemHeader_t*)pNewMap->malloc();
    char *pNewData=(char*)(pNewHeader+1);

    pNewHeader->size=newsize;
    pNewHeader->resize=false;
    pNewHeader->close=m_pHeader->close;

    pNewHeader->tail=0;
    pNewHeader->head=pNewHeader->avail=m_pHeader->avail;
    pNewHeader->waiting=m_pHeader->waiting;

    if (m_pHeader->avail)
    {
        // one or two blocks in circular queue?
        if (m_pHeader->tail<m_pHeader->head)
        {
            memcpy(pNewData,m_pData+m_pHeader->tail,m_pHeader->avail);
        }
        else
        {
            int firstchunk=m_pHeader->size-m_pHeader->tail;
            memcpy(pNewData,m_pData+m_pHeader->tail,firstchunk);
            memcpy(pNewData+firstchunk,m_pData,m_pHeader->head);
        }
    }

    m_pMap->close();
    delete m_pMap;
    m_pMap=pNewMap;

    m_pHeader=pNewHeader;
    m_pData=pNewData;

    return true;
}

bool ShmemOutputStreamImpl::write(const Bytes& b)
{
    if (!m_bOpen) return false;

    m_pAccessMutex->acquire();

    if (!m_bOpen) return false;

    if (m_pHeader->close)
    {
        m_pAccessMutex->release();
        close();
        return false;
    }

    if ((int)m_pHeader->size-(int)m_pHeader->avail<(int)b.length())
    {
        YARP_SSIZE_T required=m_pHeader->size+2*b.length();
        Resize((int)required);
    }

    if ((int)m_pHeader->head+(int)b.length()<=(int)m_pHeader->size)
    {
        memcpy(m_pData+m_pHeader->head,b.get(),b.length());
    }
    else
    {
        int first_block_size=m_pHeader->size-m_pHeader->head;
        memcpy(m_pData+m_pHeader->head,b.get(),first_block_size);
        memcpy(m_pData,b.get()+first_block_size,b.length()-first_block_size);
    }

    m_pHeader->avail+=(int)b.length();
    m_pHeader->head+=(int)b.length();
    m_pHeader->head%=m_pHeader->size;

    while (m_pHeader->waiting>0)
    {
        --m_pHeader->waiting;
        m_pWaitDataMutex->release();
    }

    m_pAccessMutex->release();

    return true;
}

void ShmemOutputStreamImpl::close()
{
    if (!m_bOpen) return;

    m_bOpen=false;

    m_pAccessMutex->acquire();
    while (m_pHeader->waiting>0)
    {
        --m_pHeader->waiting;
        m_pWaitDataMutex->release();
    }
    m_pHeader->close=true;
    m_pAccessMutex->release();

    m_pAccessMutex->remove();
    delete m_pAccessMutex;
    m_pAccessMutex=0;

    m_pWaitDataMutex->remove();
    delete m_pWaitDataMutex;
    m_pWaitDataMutex=0;

    m_pMap->close();
    delete m_pMap;
    m_pMap=0;
}

#endif
