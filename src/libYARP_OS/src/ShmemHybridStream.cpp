// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/ShmemHybridStream.h>

bool yarp::ShmemHybridStream::Close(bool bCloseRemote)
{
	if (!m_bLinked) return false;

	m_bLinked=false;

	if (bCloseRemote)
	{
		ShmemPacket_t packet;
		packet.command=CLOSE;
		m_SockStream.send_n(&packet,sizeof packet);
	}

	stop();

	if (m_pRecvMap)
	{
		m_pRecvMap->close();
		delete m_pRecvMap;
		m_pRecvMap=0;
	}

	if (m_pSendMap)
	{
		m_pSendMap->close();
		delete m_pSendMap;
		m_pSendMap=0;
	}

	m_SockStream.close();

	return true;
}

int yarp::ShmemHybridStream::accept()
{
	if (m_bLinked) return -1;

	int result=m_Acceptor.accept(m_SockStream);

	if (result<0)
	{
		YARP_ERROR(Logger::get(),
			       String("ShmemHybridStream server returned ")
                   +NetType::toString(result));

		Close();
		return result;
	}

    ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Address(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Address(remote.get_host_addr(),remote.get_port_number());

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_name[1024];
	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_LocalAddress.getPort(),0);

	m_pSendMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
		m_SendBuffSize, //int len = -1,
		O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,
#else
	int shmemkey=m_LocalAddress.getPort();

	m_pSendMap=new ACE_Shared_Memory_SV(shmemkey,m_SendBuffSize,ACE_Shared_Memory_SV::ACE_CREATE);
#endif

	if (!m_pSendMap)
	{
		YARP_ERROR(Logger::get(),
				   String("ShmemHybridStream can't create shared memory"));

		Close();
		return -1;
	}

	m_pSendBuffer=(char*)m_pSendMap->malloc();

	ShmemPacket_t send_conn_data;
	send_conn_data.command=ACKNOWLEDGE;
	send_conn_data.size=m_SendBuffSize;
	
	int ret=m_SockStream.send_n(&send_conn_data,sizeof send_conn_data);

	if (ret<=0)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream socket writing error"));
		Close();
		return -1;
	}

	// data from client

	ShmemPacket_t recv_conn_data;

	ret=m_SockStream.recv_n(&recv_conn_data,sizeof send_conn_data);

	if (ret<=0 || recv_conn_data.command!=CONNECT)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream server returned ")
                   +NetType::toString(ret));
		Close();
		return -1;
	}

	m_RecvNFree=m_RecvBuffSize=recv_conn_data.size;

#if defined(ACE_LACKS_SYSV_SHMEM)

	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_RemoteAddress.getPort(),0);

	m_pRecvMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
		m_RecvBuffSize, //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else
	shmemkey=m_RemoteAddress.getPort();

	m_pRecvMap=new ACE_Shared_Memory_SV(shmemkey,m_RecvBuffSize);
#endif

	if (!m_pRecvMap)
	{
		YARP_ERROR(Logger::get(),
				   String("ShmemHybridStream can't create shared memory"));

		return -1;
	}

	m_pRecvBuffer=(char*)m_pRecvMap->malloc();

	m_bLinked=true;

	start();

	return result;
}

int yarp::ShmemHybridStream::connect(const ACE_INET_Addr& ace_address)
{
	if (m_bLinked) return -1;

	ACE_SOCK_Connector connector;
	int result=connector.connect(m_SockStream,ace_address);

	if (result<0)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream client returned ")
                   +NetType::toString(result));
		
		Close();

		return result;
	}

	ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Address(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Address(remote.get_host_addr(),remote.get_port_number());

	ShmemPacket_t recv_conn_data;

	int ret=m_SockStream.recv_n(&recv_conn_data,sizeof recv_conn_data);

	if (ret<=0 || recv_conn_data.command!=ACKNOWLEDGE)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream server returned ")
                   + NetType::toString(ret));
		Close();
		return -1;
	}

	m_RecvNFree=m_RecvBuffSize=recv_conn_data.size;

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_name[1024];
	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_RemoteAddress.getPort(),0);

	m_pRecvMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
		m_RecvBuffSize, //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	int shmemkey=m_RemoteAddress.getPort();

	m_pRecvMap=new ACE_Shared_Memory_SV(shmemkey,m_RecvBuffSize);			

#endif

	if (!m_pRecvMap)
	{
		YARP_ERROR(Logger::get(),
				   String("ShmemHybridStream can't create shared memory"));

		Close();
		return -1;
	}

	m_pRecvBuffer=(char*)m_pRecvMap->malloc();

#if defined(ACE_LACKS_SYSV_SHMEM)

	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_LocalAddress.getPort(),0);

	m_pSendMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
		m_SendBuffSize, //int len = -1,
		O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	shmemkey=m_LocalAddress.getPort();

	m_pSendMap=new ACE_Shared_Memory_SV(shmemkey,m_SendBuffSize,ACE_Shared_Memory_SV::ACE_CREATE);			

#endif

	if (!m_pSendMap)
	{
		YARP_ERROR(Logger::get(),
				   String("ShmemHybridStream can't create shared memory"));

		Close();
		return -1;
	}

	m_pSendBuffer=(char*)m_pSendMap->malloc();

	// send data to server

	ShmemPacket_t send_conn_data;
	send_conn_data.command=CONNECT;
	send_conn_data.size=m_SendBuffSize;

	ret=m_SockStream.send_n(&send_conn_data,sizeof send_conn_data);

	if (ret<=0)
	{
		YARP_ERROR(Logger::get(),
				   String("ShmemHybridStream socket writing error"));

		Close();
		return -1;
	}

	m_bLinked=true;

	start();

	return result;
}

void yarp::ShmemHybridStream::run()
{
	ShmemPacket_t packet;

	while (m_bLinked)
	{
		int ret=m_SockStream.recv_n(&packet,sizeof packet);

		if (ret<=0)
		{
			Close();
			return;
		}

		switch (packet.command)
		{
		case READ:
			{
				ReadAck(packet.size);
				break;
			}
		case WRITE:
			{
				WriteAck(packet.size);
				break;
			}
		case RESIZE:
			{
				m_ResizeMutex.wait();
				RecvResize(packet.size);
				m_ResizeMutex.post();
				break;
			}
		case CLOSE:
			{
				Close();
				return;
			}
		default:
			{
				YARP_ERROR(Logger::get(),
                           String("ShmemHybridStream received unknown command")
                           +NetType::toString(packet.command));
			}
		}
	}
}

bool yarp::ShmemHybridStream::RecvResize(int new_size)
{
	YARP_DEBUG(Logger::get(),
               String("ShmemHybridStream received RESIZE command to ")
               +NetType::toString(new_size));

	m_pRecvMap->close();
	delete m_pRecvMap;

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_name[1024];
	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_RemoteAddress.getPort(),++m_RecvResizeNum);

	m_pRecvMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
									    new_size, //int len = -1,
									    O_RDWR, //int flags = O_RDWR | O_CREAT,
									    ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
									    PROT_RDWR, //int prot = PROT_RDWR,
									    ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,
#else
	++m_RecvResizeNum;

	int shmemkey=(m_RecvResizeNum<<16)+m_RemoteAddress.getPort();

	m_pRecvMap=new ACE_Shared_Memory_SV(shmemkey,new_size);
#endif

	if (!m_pRecvMap)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream can't create shared memory"));
		Close();
		return false;
	}

	m_pRecvBuffer=(char*)m_pRecvMap->malloc();

	if (m_RecvNData && m_RecvTail>=m_RecvHead)
	{
		m_RecvTail+=new_size-m_RecvBuffSize;
	}

	m_RecvBuffSize=new_size;
	m_RecvNFree=m_RecvBuffSize-m_RecvNData;

	return true;
}

bool yarp::ShmemHybridStream::SendResize(int new_size)
{
	YARP_DEBUG(Logger::get(),
               String("ShmemHybridStream send RESIZE command to ")
               +NetType::toString(new_size)+String(" bytes"));

	ACE_Shared_Memory* pNewMap;

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_name[1024];
	file_name[0]=0;

	sprintf(file_name,"%sSHMEM_FILE_%d_%d",file_path,m_LocalAddress.getPort(),++m_SendResizeNum);

	pNewMap=new ACE_Shared_Memory_MM(file_name, //const ACE_TCHAR *filename,
									 new_size, //int len = -1,
									 O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
									 ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
									 PROT_RDWR, //int prot = PROT_RDWR,
									 ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,
#else
	++m_SendResizeNum;

	int shmemkey=(m_SendResizeNum<<16)+m_LocalAddress.getPort();

	pNewMap=new ACE_Shared_Memory_SV(shmemkey,new_size,ACE_Shared_Memory_SV::ACE_CREATE);
#endif

	if (!pNewMap)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream can't create shared memory"));
		Close();	
		return false;
	}

	char *pNewBuffer=(char*)pNewMap->malloc();

	if (m_SendNData)
	{
		//distinguere tra uno o due blocchi
		if (m_SendTail<m_SendHead)
		{
			memcpy(pNewBuffer+m_SendTail,m_pSendBuffer+m_SendTail,m_SendNData);
		}
		else
		{
			int NewSendTail=m_SendTail+new_size-m_SendBuffSize;
			memcpy(pNewBuffer,m_pSendBuffer,m_SendHead);
			memcpy(pNewBuffer+NewSendTail,m_pSendBuffer+m_SendTail,m_SendBuffSize-m_SendTail);
			m_SendTail=NewSendTail;
		}
	}

	m_SendBuffSize=new_size;
	m_SendNFree=m_SendBuffSize-m_SendNData;

	m_pSendBuffer=pNewBuffer;
	m_pSendMap->close();
	delete m_pSendMap;
	m_pSendMap=pNewMap;

	ShmemPacket_t resize_packet;
	resize_packet.command=RESIZE;
	resize_packet.size=new_size;

	int ret=m_SockStream.send_n(&resize_packet,sizeof resize_packet);

	if (ret<=0)	
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream socket reading error"));	
		
		Close();
		return false;
	}

	return true;
}