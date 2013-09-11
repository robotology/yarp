// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Alessandro Scalzo alessandro@liralab.it
* Copyright (C) 2007 RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/ShmemInputStream.h>
#include <yarp/os/impl/PlatformTime.h>

using namespace yarp::os::impl;
using namespace yarp::os::impl;
using namespace yarp::os;

bool ShmemInputStreamImpl::open(int port,ACE_SOCK_Stream *pSock,int size)
{
	m_pSock=pSock;

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

	sprintf(obj_name,"%sSHMEM_FILE_%d_%d",temp_dir_path,port,0);

	m_pMap=new ACE_Shared_Memory_MM(obj_name, //const ACE_TCHAR *filename,
		size+sizeof(ShmemHeader_t), //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	m_pMap=new ACE_Shared_Memory_SV(port,size+sizeof(ShmemHeader_t));

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

	m_pWaitDataMutex->acquire();

	m_bOpen=true;

	return true;
}

bool ShmemInputStreamImpl::Resize()
{
	++m_ResizeNum;

	ACE_Shared_Memory* pNewMap;

	//printf("input stream resize %d to %d\n",m_ResizeNum,m_pHeader->newsize);
	//fflush(stdout);

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
		m_pHeader->newsize+sizeof(ShmemHeader_t), //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	int shmemkey=(m_ResizeNum<<16)+m_Port;

	pNewMap=new ACE_Shared_Memory_SV(shmemkey,m_pHeader->size+sizeof(ShmemHeader_t));

#endif

	if (!pNewMap)
	{
		YARP_ERROR(Logger::get(),String("ShmemOutputStream can't create shared memory"));
		return false;
	}

	ShmemHeader_t *pNewHeader=(ShmemHeader_t*)pNewMap->malloc();
	char *pNewData=(char*)(pNewHeader+1);

	m_pMap->close();
	delete m_pMap;

	m_pMap=pNewMap;
	m_pHeader=pNewHeader;
	m_pData=pNewData;

	return true;
}

int ShmemInputStreamImpl::read(char *data,int len)
{
	m_pAccessMutex->acquire();

	if (m_pHeader->close)
	{
		m_pAccessMutex->release();
		close();
		return -1;
	}

	while (m_pHeader->resize) Resize();

	if (m_pHeader->avail<len)
	{
		++m_pHeader->waiting;
		m_pAccessMutex->release();
		return 0;
	}

	if (m_pHeader->tail+len>m_pHeader->size)
	{
		int first_block_size=m_pHeader->size-m_pHeader->tail;

		memcpy((void*)data,(void*)(m_pData+m_pHeader->tail),first_block_size);
		memcpy((void*)(data+first_block_size),(void*)m_pData,len-first_block_size);
	}
	else
	{
		memcpy((void*)data,(void*)(m_pData+m_pHeader->tail),len);
	}

	m_pHeader->avail-=len;
	m_pHeader->tail+=len;
	m_pHeader->tail%=m_pHeader->size;

	m_pAccessMutex->release();

	return len;
}

YARP_SSIZE_T ShmemInputStreamImpl::read(const Bytes& b)
{
	m_ReadSerializerMutex.wait();
	
	if (!m_bOpen)
	{
	 	m_ReadSerializerMutex.post();
	    return -1;
	}

	char *data=b.get(),buf;
	size_t len=b.length();
	YARP_SSIZE_T ret;

	while (!(ret=read(data,(int)len)))
	{
	    #ifdef _ACE_USE_SV_SEM
		ACE_Time_Value tv=ACE_OS::gettimeofday();
		tv.sec(tv.sec()+1);
	    #else
	    ACE_Time_Value tv(1);
	    #endif 
	    
	    m_pWaitDataMutex->acquire(tv);
	    
		if (!m_pSock->recv(&buf,1))
		{
			//printf("STREAM IS BROKEN\n");
			//fflush(stdout);
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
