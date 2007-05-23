// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/ShmemHybridStream.h>

#include <yarp/NetType.h>

bool yarp::ShmemHybridStream::Close(bool bCloseRemote)
{
	if (!m_bLinked) return false;

	m_bLinked=false;

	if (bCloseRemote)
	{
		ShmemReadWrite_t packet;
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
		return result;
	}

    ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Address(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Address(remote.get_host_addr(),remote.get_port_number());

	ShmemConnect_t send_conn_data;
	send_conn_data.command=ACKNOWLEDGE;
	send_conn_data.size=m_SendBuffSize;

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_path[256];
	file_path[0]=0;

	int	tmpres=ACE::get_temp_dir(file_path,256);

	if (tmpres!=-1)
	{
		sprintf(send_conn_data.filename,"%s/SRV_SHMEM_FILE_%d",file_path,m_LocalAddress.getPort());
	}
	else
	{
		sprintf(send_conn_data.filename,"SRV_SHMEM_FILE_%d",m_LocalAddress.getPort());
		YARP_DEBUG(Logger::get(),"ShmemHybridStream: no temp directory found, using Local.");
	}

	m_pSendMap=new ACE_Shared_Memory_MM(send_conn_data.filename, //const ACE_TCHAR *filename,
		m_SendBuffSize, //int len = -1,
		O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,
#else
	int shmemkey=2*m_Port;

	m_pSendMap=new ACE_Shared_Memory_SV(shmemkey,m_SendBuffSize,ACE_Shared_Memory_SV::ACE_CREATE);
#endif

	m_pSendBuffer=(char*)m_pSendMap->malloc();

	m_SockStream.send_n(&send_conn_data,sizeof send_conn_data);

	// data from client

	ShmemConnect_t recv_conn_data;

	int ret=m_SockStream.recv_n(&recv_conn_data,sizeof send_conn_data);

	if (ret<=0 || recv_conn_data.command!=CONNECT)
	{
		YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream server returned ")
                   + NetType::toString(ret));
		Close();
		return -1;
	}

	m_RecvNFree=m_RecvBuffSize=recv_conn_data.size;

#if defined(ACE_LACKS_SYSV_SHMEM)

	m_pRecvMap=new ACE_Shared_Memory_MM(recv_conn_data.filename, //const ACE_TCHAR *filename,
		m_RecvBuffSize, //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else
	shmemkey=2*m_Port+1;

	m_pRecvMap=new ACE_Shared_Memory_SV(shmemkey,m_RecvBuffSize);
#endif

	m_pRecvBuffer=(char*)m_pRecvMap->malloc();

	m_bLinked=true;

	start();

	return result;
}

int yarp::ShmemHybridStream::connect(const Address& address)
{
	if (m_bLinked) return -1;

	ACE_INET_Addr addr(address.getPort(),address.getName().c_str());
	ACE_SOCK_Connector connector;
	int result=connector.connect(m_SockStream,addr);

	if (result<0)
	{
		return result;
	}

	ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Address(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Address(remote.get_host_addr(),remote.get_port_number());
	
	ShmemConnect_t recv_conn_data;

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

	m_pRecvMap=new ACE_Shared_Memory_MM(recv_conn_data.filename, //const ACE_TCHAR *filename,
		m_RecvBuffSize, //int len = -1,
		O_RDWR, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	int shmemkey=2*m_Port;

	m_pRecvMap=new ACE_Shared_Memory_SV(shmemkey,m_RecvBuffSize);			

#endif

	m_pRecvBuffer=(char*)m_pRecvMap->malloc();

	// send data to server

	ShmemConnect_t send_conn_data;
	send_conn_data.command=CONNECT;
	send_conn_data.size=m_SendBuffSize;

#if defined(ACE_LACKS_SYSV_SHMEM)

	char file_path[256];
	file_path[0]=0;

	int	tmpres=ACE::get_temp_dir(file_path,256);

	if (tmpres!=-1)
	{
		sprintf(send_conn_data.filename,"%s/CLN_SHMEM_FILE_%d",file_path,m_LocalAddress.getPort());
	}
	else
	{
		sprintf(send_conn_data.filename,"CLN_SHMEM_FILE_%d",m_LocalAddress.getPort());
		YARP_DEBUG(Logger::get(),"ShmemHybridStream: no temp directory found, using Local.");
	}

	m_pRecvMap=new ACE_Shared_Memory_MM(send_conn_data.filename, //const ACE_TCHAR *filename,
		m_SendBuffSize, //int len = -1,
		O_RDWR | O_CREAT, //int flags = O_RDWR | O_CREAT,
		ACE_DEFAULT_FILE_PERMS, //int mode = ACE_DEFAULT_FILE_PERMS,
		PROT_RDWR, //int prot = PROT_RDWR,
		ACE_MAP_SHARED); //int share = ACE_MAP_PRIVATE,

#else

	shmemkey=2*m_Port+1;

	m_pSendMap=new ACE_Shared_Memory_SV(shmemkey,m_SendBuffSize,ACE_Shared_Memory_SV::ACE_CREATE);			

#endif

	m_pSendBuffer=(char*)m_pRecvMap->malloc();

	ret=m_SockStream.send_n(&send_conn_data,sizeof send_conn_data);

	if (ret<=0)
	{
		Close();

		return -1;
	}

	m_bLinked=true;

	start();

	return result;
}

void yarp::ShmemHybridStream::run()
{
	ShmemReadWrite_t packet;

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
		case CLOSE:
			{
				Close();
				return;
			}
		}
	}
}
