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
class yarp::ShmemHybridStream : public TwoWayStream, InputStream, OutputStream
{
public:
	ShmemHybridStream(){ m_bLinked=false; }
	virtual ~ShmemHybridStream(){ close(); }
	int open(const Address& yarp_address,bool sender); 
	int accept();

	virtual void close()
	{
		m_bLinked=false;
		in.close();
		out.close();
	}

	virtual void interrupt()
	{
		printf("INTERRUPT\n");
		fflush(stdout);
		close(); 
	}

	virtual void write(const Bytes& b)
	{
		if (!out.write(b)) close();
	}

	virtual int read(const Bytes& b)
	{
		int ret=in.read(b);
		if (ret==-1) close();
		return ret;
	}

	// TwoWayStrem implementation
	virtual InputStream& getInputStream(){ return *this; }
	virtual OutputStream& getOutputStream(){ return *this; }
	virtual bool isOk(){ return m_bLinked && in.isOk() && out.isOk(); }

	virtual void reset()
	{
		//printf("RECEIVED RESET COMMAND\n");
		//fflush(stdout);
		close();
	}

	virtual void beginPacket(){}
	virtual void endPacket(){}

	virtual const Address& getLocalAddress(){ return m_LocalAddress; }
	virtual const Address& getRemoteAddress(){ return m_RemoteAddress; }

protected:
	enum {CONNECT=0,ACKNOWLEDGE,READ,WRITE,CLOSE,WAKE_UP_MF,RESIZE};
	
	// DATA
	
	bool m_bLinked;

	Address m_LocalAddress,m_RemoteAddress;
	ACE_SOCK_Stream m_SockStream;
	ACE_SOCK_Acceptor m_Acceptor;

	yarp::ShmemInputStreamImpl in;
	yarp::ShmemOutputStreamImpl out;

	// FUNCTIONS
	int connect(const ACE_INET_Addr &address);
};

#endif

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
	
