// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Alessandro Scalzo alessandro@liralab.it
 * Copyright (C) 2007 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/ShmemHybridStream.h>

using namespace yarp::os::impl;

int ShmemHybridStream::open(const Contact& yarp_address,bool sender)
{
	m_bLinked=false;

	ACE_INET_Addr ace_address(yarp_address.getPort(),yarp_address.getHost().c_str());

	if (sender)
	{				
		return connect(ace_address);
	}
	else
	{
		ACE_INET_Addr ace_server_addr(ace_address.get_port_number());

		int result = m_Acceptor.open(ace_server_addr);

		if (result<0)
		{
			YARP_ERROR(Logger::get(),String("ShmemHybridStream open result")+NetType::toString(result));
			return result;
		}

		m_Acceptor.get_local_addr(ace_server_addr);

		m_LocalAddress = Contact(ace_server_addr.get_host_addr(),ace_server_addr.get_port_number());
		m_RemoteAddress = m_LocalAddress; // finalized in call to accept()

		return result;
	}

	return 1;
}

int ShmemHybridStream::accept()
{
	if (m_bLinked) return -1;

	YARP_SSIZE_T result=m_Acceptor.accept(m_SockStream);

	if (result<0)
	{
		YARP_ERROR(Logger::get(),String("ShmemHybridStream server returned ")+NetType::toString((int)result));
		close();
		return -1;
	}

    ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Contact(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Contact(remote.get_host_addr(),remote.get_port_number());

	ShmemPacket_t recv_conn_data;
	result=m_SockStream.recv_n(&recv_conn_data,sizeof recv_conn_data);
	if (result<=0)
	{
		YARP_ERROR(Logger::get(),String("Socket returned ")+NetType::toString((int)result));
		close();
		return -1;
	}

	if (!in.open(m_RemoteAddress.getPort(),&m_SockStream))
	{
		YARP_ERROR(Logger::get(),String("ShmemHybridStream can't create shared memory"));
		close();
		return -1;
	}

	if (!out.open(m_LocalAddress.getPort()))
	{
		YARP_ERROR(Logger::get(),String("ShmemHybridStream can't create shared memory"));
		close();
		return -1;
	}

	ShmemPacket_t send_conn_data;
	send_conn_data.command=ACKNOWLEDGE;
	if (m_SockStream.send_n(&send_conn_data,sizeof send_conn_data)<=0)
	{
		YARP_ERROR(Logger::get(),String("ShmemHybridStream socket writing error"));
		close();
		return -1;
	}	

	m_bLinked=true;

	m_SockStream.enable(ACE_NONBLOCK);

	return 0;
}

int ShmemHybridStream::connect(const ACE_INET_Addr& ace_address)
{
	if (m_bLinked) return -1;

	ACE_SOCK_Connector connector;
	YARP_SSIZE_T result=connector.connect(m_SockStream,ace_address);
	if (result<0)
	{
		YARP_ERROR(Logger::get(),String("ShmemHybridStream client returned ")+NetType::toString((int)result));
		close();
		return -1;
	}

	ACE_INET_Addr local,remote;
    m_SockStream.get_local_addr(local);
    m_SockStream.get_remote_addr(remote);
    m_LocalAddress=Contact(local.get_host_addr(),local.get_port_number());
    m_RemoteAddress=Contact(remote.get_host_addr(),remote.get_port_number());

	out.open(m_LocalAddress.getPort());

	ShmemPacket_t send_conn_data;
	send_conn_data.command=CONNECT;
	send_conn_data.size=SHMEM_DEFAULT_SIZE;
	result=m_SockStream.send_n(&send_conn_data,sizeof send_conn_data);
	if (result<=0)
	{
		YARP_ERROR(Logger::get(),String("Socket returned ")+NetType::toString((int)result));
		close();
		return -1;
	}

	ShmemPacket_t recv_conn_data;
	result=m_SockStream.recv_n(&recv_conn_data,sizeof recv_conn_data);
	if (result<=0)
	{
		YARP_ERROR(Logger::get(),String("Socket returned ")+NetType::toString((int)result));
		close();
		return -1;
	}

	in.open(m_RemoteAddress.getPort(),&m_SockStream);

	m_bLinked=true;

	m_SockStream.enable(ACE_NONBLOCK);

	return 0;
}


#endif
