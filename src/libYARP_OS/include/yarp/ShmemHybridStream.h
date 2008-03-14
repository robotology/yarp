// *-- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Alessandro Scalzo alessandro@liralab.it
 * Copyright (C) 2007 RobotCub Consortium
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __SHMEM_HYBRID_STREAM__
#define __SHMEM_HYBRID_STREAM__

#include <yarp/InputStream.h>
#include <yarp/OutputStream.h>
#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

namespace yarp {
	class ShmemHybridStream;
};

#include <yarp/ShmemInputStream.h>
#include <yarp/ShmemOutputStream.h>

using namespace yarp;

/**
 * A stream abstraction for shared memory communication.
 */
class yarp::ShmemHybridStream : public TwoWayStream
{
public:
	ShmemHybridStream()
	{
	}
	
	virtual ~ShmemHybridStream()
	{
		close();
	}
	
	void close()
	{
		in.close();
		out.close();
	}

	void write(const Bytes& b)
	{
		out.write(b);
	}

	int read(const Bytes& b)
	{
		return in.read(b);
	}

	int accept();

	int open(const Address& yarp_address,bool sender) 
	{
		m_bLinked=false;

		ACE_INET_Addr ace_address(yarp_address.getPort(),yarp_address.getName().c_str());

		if (sender)
		{				
			return connect(ace_address);
		}
		else
		{
			ACE_INET_Addr ace_server_addr(ace_address.get_port_number());
			//int result = m_Acceptor.open(ace_server_addr,1);
			int result = m_Acceptor.open(ace_server_addr);

			if (result<0)
			{
				YARP_ERROR(Logger::get(),
                           String("ShmemHybridStream open result")
                           +NetType::toString(result));
				return result;
			}

			m_Acceptor.get_local_addr(ace_server_addr);

			m_LocalAddress = Address(ace_server_addr.get_host_addr(),ace_server_addr.get_port_number());
			m_RemoteAddress = m_LocalAddress; // finalized in call to accept()

			return result;
		}

		return 1;
	}

	// TwoWayStrem implementation
	virtual InputStream& getInputStream(){ return in; }
	virtual OutputStream& getOutputStream(){ return out; }
	virtual bool isOk(){ return m_bLinked; }

	virtual void reset(){}
	virtual void beginPacket(){}
	virtual void endPacket(){}
	/*
	virtual void endPacket()
	{
		m_SendSerializerMutex.wait();

		ShmemPacket_t write_data;
		write_data.command=WRITE;
		write_data.size=m_PacketDataSent;
		m_PacketDataSent=0;
		int ret=m_SockStream.send_n(&write_data,sizeof write_data);

		if (ret<=0)
		{
			YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream socket writing error ")
                   +NetType::toString(ret));
			Close();
		}

		m_SendSerializerMutex.post();
	}
	*/
	virtual const Address& getLocalAddress(){ return m_LocalAddress; }
	virtual const Address& getRemoteAddress(){ return m_RemoteAddress; }

protected:
	enum {CONNECT=0,ACKNOWLEDGE,READ,WRITE,CLOSE,WAKE_UP_MF,RESIZE};
	
	// DATA
	
	bool m_bLinked;

	Address m_LocalAddress,m_RemoteAddress;
	ACE_SOCK_Stream m_SockStream;
	ACE_SOCK_Acceptor m_Acceptor;

	yarp::ShmemInputStream in;
	yarp::ShmemOutputStream out;

	// FUNCTIONS

	int connect(const ACE_INET_Addr &address);

	inline void ReadAck(int size);
	inline void WriteAck(int size);
};

#endif
