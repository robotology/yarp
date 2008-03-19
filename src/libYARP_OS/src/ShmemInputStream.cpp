// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Alessandro Scalzo alessandro@liralab.it
* Copyright (C) 2007 RobotCub Consortium
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

#include <yarp/ShmemInputStream.h>

bool ShmemInputStream::open(int port,int size)
{
	m_pAccessMutex=m_pWaitDataMutex=0;
	m_pMap=0;
	m_pData=0;
	m_pHeader=0;
	m_ResizeNum=0;

	m_Port=port;

	char obj_name[1024];

#ifdef ACE_LACKS_SYSV_SHMEM

	char temp_dir_path[1024];

	if (ACE::get_temp_dir(temp_dir_path,1024)==-1)
	{
		YARP_ERROR(Logger::get(),"ShmemHybridStream: no temp directory found.");
		return false;
	}

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

	sprintf(obj_name,"SHMEM_ACCESS_MUTEX_%d",port);
	m_pAccessMutex=new ACE_Process_Semaphore(1,obj_name);

	sprintf(obj_name,"SHMEM_WAITDATA_MUTEX_%d",port);
	m_pWaitDataMutex=new ACE_Process_Semaphore(0,obj_name);

	m_bOpen=true;

	return true;
}

bool ShmemInputStream::Resize()
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

int ShmemInputStream::read(char *data,int len)
{
	//printf("IN: m_pAccessMutex->acquire();\n");
	//fflush(stdout);
	m_pAccessMutex->acquire();

	if (m_pHeader->close)
	{
		m_pAccessMutex->release();
		close();
		return -1;
	}

	while (m_pHeader->resize) Resize();

	//printf ("IN: avail=%d\n",m_pHeader->avail);
	//fflush(stdout);

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

	//printf("IN: m_pAccessMutex->release();\n");
	//fflush(stdout);
	m_pAccessMutex->release();

	return len;
}

int ShmemInputStream::read(const Bytes& b/*,bool bBlocking=true*/)
{
	m_SerializerMutex.wait();

	char* data=b.get();
	int len=b.length(),ret;

	while (!(ret=read(data,len)))
	{
		//printf("IN: m_pWaitDataMutex->acquire();\n");
		//fflush(stdout);
		m_pWaitDataMutex->acquire();
	}

	m_SerializerMutex.post();

	return ret;
}

void ShmemInputStream::close()
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
