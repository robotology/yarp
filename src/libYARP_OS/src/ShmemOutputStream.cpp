// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Author: Alessandro Scalzo alessandro@liralab.it
* Copyright (C) 2007 RobotCub Consortium
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

#include <yarp/ShmemOutputStream.h>

bool ShmemOutputStream::open(int port,int size)
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

	sprintf(obj_name,"SHMEM_ACCESS_MUTEX_%d",port);
	m_pAccessMutex=new ACE_Process_Semaphore(1,obj_name);

	sprintf(obj_name,"SHMEM_WAITDATA_MUTEX_%d",port);
	m_pWaitDataMutex=new ACE_Process_Semaphore(0,obj_name);

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

bool ShmemOutputStream::Resize(int newsize)
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
		YARP_ERROR(Logger::get(),String("ShmemOutputStream can't create shared memory"));
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
		//distinguere tra uno o due blocchi
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

void ShmemOutputStream::write(const Bytes& b)
{
	//printf("OUT: m_pAccessMutex->acquire();\n");
	//fflush(stdout);
	m_pAccessMutex->acquire();

	if (m_pHeader->close)
	{
		m_pAccessMutex->release();
		close();
		return;
	}

	if (m_pHeader->size-m_pHeader->avail<b.length())
	{
		int required=m_pHeader->size+2*b.length();
		Resize(required);
	}

	if (m_pHeader->head+b.length()<=m_pHeader->size)
	{
		memcpy(m_pData+m_pHeader->head,b.get(),b.length());
	}
	else
	{
		int first_block_size=m_pHeader->size-m_pHeader->head;
		memcpy(m_pData+m_pHeader->head,b.get(),first_block_size);
		memcpy(m_pData,b.get()+first_block_size,b.length()-first_block_size);
	}

	m_pHeader->avail+=b.length();
	m_pHeader->head+=b.length();
	m_pHeader->head%=m_pHeader->size;

	while (m_pHeader->waiting)
	{
		//printf("OUT: m_pWaitDataMutex->release();\n");
		//fflush(stdout);
		--m_pHeader->waiting;
		m_pWaitDataMutex->release();
	}

	//printf("OUT: m_pAccessMutex->release();\n");
	//fflush(stdout);
	m_pAccessMutex->release();
}

void ShmemOutputStream::close()
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
