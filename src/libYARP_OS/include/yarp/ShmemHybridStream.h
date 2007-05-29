// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __SHMEM_HYBRID_STREAM__
#define __SHMEM_HYBRID_STREAM__

#include <ace/config.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#if defined(ACE_LACKS_SYSV_SHMEM)
#include <ace/Shared_Memory_MM.h>
#else
#include <ace/Shared_Memory_SV.h>
#endif

#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/TwoWayStream.h>
#include <yarp/IOException.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

struct ShmemPacket_t
{
	int command;
	int size;
};

namespace yarp {
	class ShmemHybridStream;
}

/**
 * A stream abstraction for shared memory / socket hybrid communication.
 */
class yarp::ShmemHybridStream : public TwoWayStream, InputStream, OutputStream,
								protected yarp::os::Thread
{
public:
	ShmemHybridStream() : m_WaitDataMutex(0),m_WaitSpaceMutex(0)
	{
		m_bLinked=false;
		m_bDataRequest=false;
		m_bSpaceRequest=false;

		m_PacketDataSent=0;

		m_SendResizeNum=m_RecvResizeNum=0;

		file_path[0]=0;

		if (ACE::get_temp_dir(file_path,256)==-1)
		{
			file_path[0]=0;
			YARP_DEBUG(Logger::get(),"ShmemHybridStream: no temp directory found, using Local.");
		}
	}
	
	virtual ~ShmemHybridStream()
	{
		Close();
	}
	
	bool Close(bool bCloseRemote=false);
	int  accept();
	inline int send(char* data,int size,bool bNonBlocking=false);
	inline int recv(char* data,int size,bool bNonBlocking=false);
	bool SendResize(int new_size);
	bool RecvResize(int new_size);

	int open(const Address& yarp_address,bool sender,int sendbuffsize=4096) 
	{
		m_bLinked=false;
		m_bDataRequest=false;
		m_bSpaceRequest=false;

		m_SendNFree=m_SendBuffSize=sendbuffsize;
		m_RecvNFree=m_RecvBuffSize=0;

		m_pRecvMap=0;
		m_pSendMap=0;

		m_RecvHead=m_RecvTail=0;
		m_SendHead=m_SendTail=0;
		m_RecvNData=m_SendNData=0;

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
	virtual InputStream& getInputStream(){ return *this; }
	virtual OutputStream& getOutputStream(){ return *this; }
	virtual bool isOk(){ return m_bLinked; }

	virtual void close(){ Close(); }
	virtual void reset(){}
	virtual void beginPacket(){}
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
	virtual const Address& getLocalAddress(){ return m_LocalAddress; }
	virtual const Address& getRemoteAddress(){ return m_RemoteAddress; }

	// InputStream implementation
	virtual int read(const Bytes& b)
	{
		return recv(b.get(),b.length());
	}

	// OutputStream implementation
    virtual void write(const Bytes& b)
	{
		send(b.get(),b.length());
	}

protected:
	enum {CONNECT=0,ACKNOWLEDGE,READ,WRITE,CLOSE,WAKE_UP_MF,RESIZE};
	
	// DATA
	
	bool m_bLinked;

	int m_SendResizeNum,m_RecvResizeNum;

	int m_PacketDataSent;

	Address m_LocalAddress,m_RemoteAddress;

	ACE_Shared_Memory *m_pSendMap,*m_pRecvMap;

	char *m_pSendBuffer,*m_pRecvBuffer;

	char file_path[256];

	ACE_SOCK_Stream m_SockStream;

	ACE_SOCK_Acceptor m_Acceptor;

	int m_SendBuffSize,m_RecvBuffSize;

	int m_RecvHead,m_RecvTail;
	int m_SendHead,m_SendTail;
	int m_SendNData,m_RecvNData;
	int m_SendNFree,m_RecvNFree;

	bool m_bDataRequest,m_bSpaceRequest;

	yarp::os::Semaphore m_SendQueueMutex,m_RecvQueueMutex;
	yarp::os::Semaphore m_WaitDataMutex,m_WaitSpaceMutex;
	yarp::os::Semaphore m_SendSerializerMutex,m_RecvSerializerMutex;
	yarp::os::Semaphore m_ResizeMutex;

	// FUNCTIONS

	// Thread run() function implementation
	void run();

	int connect(const ACE_INET_Addr &address);

	inline void ReadAck(int size);
	inline void WriteAck(int size);

	inline int write_buff(char* data,int size,bool bNonBlocking=false);
	inline int read_buff(char* data,int size,bool bNonBlocking=false);
};

int yarp::ShmemHybridStream::send(char* data,int size,bool bNonBlocking)
{
	if (!m_bLinked)
	{
        if (!isStopping()) {
            YARP_ERROR(Logger::get(),"ShmemHybridStream: not connected.");
        }
		return -1;
	}

	m_SendSerializerMutex.wait();

	if (size>m_SendBuffSize)
	{
		SendResize(3*size);
	}

	int nSent=0;

	while (size>0)
	{
		int bytes_req=size>m_SendBuffSize?m_SendBuffSize:size;

		int bytes_num=write_buff(data,bytes_req,bNonBlocking);

		if (bNonBlocking)
		{
			m_SendSerializerMutex.post();

			return bytes_num;
		}

		size-=bytes_num;
		nSent+=bytes_num;
		data+=bytes_num;
	}

	m_SendSerializerMutex.post();

	return nSent;
}

int yarp::ShmemHybridStream::recv(char* data,int size,bool bNonBlocking)
{
	if (!m_bLinked)
	{
        if (!isStopping()) {
            YARP_ERROR(Logger::get(),"ShmemHybridStream: not connected.");
        }
		return -1;
	}

	m_RecvSerializerMutex.wait();

	int nReceived=0;

	while (size>0)
	{
		m_ResizeMutex.wait();
		int bytes_req=size>m_RecvBuffSize?m_RecvBuffSize:size;
		m_ResizeMutex.post();

		int bytes_num=read_buff(data,bytes_req,bNonBlocking);
        if (bytes_num<0) {
            return bytes_num;
        }

		if (bNonBlocking)
		{
			m_RecvSerializerMutex.post();

			return bytes_num;
		}

		size-=bytes_num;
		nReceived+=bytes_num;
		data+=bytes_num;
	}

	m_RecvSerializerMutex.post();

	return nReceived;
}

void yarp::ShmemHybridStream::ReadAck(int size)
{
	m_SendQueueMutex.wait();

	if (m_SendNData>=size)
	{
		m_SendNData-=size;
		m_SendNFree+=size;
		m_SendTail+=size;
		m_SendTail%=m_SendBuffSize;
	}
	else
	{
		YARP_ERROR(Logger::get(),"ShmemHybridStream: Read() buffer is empty!!!");
	}

	if (m_bSpaceRequest)
	{
		m_bSpaceRequest=false;
		// Wake up pending write_buff
		m_WaitSpaceMutex.post();
	}

	m_SendQueueMutex.post();
}

void yarp::ShmemHybridStream::WriteAck(int size)
{
	m_RecvQueueMutex.wait();

	if (m_RecvBuffSize-m_RecvNData>=size)
	{
		m_RecvNData+=size;
		m_RecvNFree-=size;
		m_RecvHead+=size;
		m_RecvHead%=m_RecvBuffSize;
	}
	else
	{
		YARP_ERROR(Logger::get(),"ShmemHybridStream: Write() buffer is full!!!");	
	}

	if (m_bDataRequest)
	{
		m_bDataRequest=false;
		// Wake up pending read_buff
		m_WaitDataMutex.post();
	}

	m_RecvQueueMutex.post();
}

int yarp::ShmemHybridStream::write_buff(char* data,int size,bool bNonBlocking)
{
	m_SendQueueMutex.wait();

	if (m_SendNFree>0) // free buff space available
	{
		int bytes_num=size<m_SendNFree?size:m_SendNFree;

		if (m_SendHead+bytes_num>m_SendBuffSize)
		{
			int FirstBlockLen=m_SendBuffSize-m_SendHead;

			memcpy((void*)(m_pSendBuffer+m_SendHead),(void*)data,FirstBlockLen);
			memcpy((void*)m_pSendBuffer,(void*)(data+FirstBlockLen),bytes_num-FirstBlockLen);
		}
		else
		{
			memcpy((void*)(m_pSendBuffer+m_SendHead),(void*)data,bytes_num);
		}

		m_SendNData+=bytes_num;
		m_SendNFree-=bytes_num;
		m_SendHead+=bytes_num;
		m_SendHead%=m_SendBuffSize;



		m_PacketDataSent+=bytes_num;



		m_SendQueueMutex.post();
		/*
		ShmemPacket_t write_data;
		write_data.command=WRITE;
		write_data.size=bytes_num;
		int ret=m_SockStream.send_n(&write_data,sizeof write_data);

		if (ret<=0)
		{
			YARP_ERROR(Logger::get(),
                   String("ShmemHybridStream socket writing error ")
                   +NetType::toString(ret));
			Close();
			return -1;
		}
		*/

		return bytes_num;
	}

	// no free buff space available

	if (bNonBlocking)
	{
		m_SendQueueMutex.post();

		return 0;
	}

	m_bSpaceRequest=true;
	m_SendQueueMutex.post();

	//ShmemPacket_t read_data;
	//read_data.command=WAKE_UP_MF;
	//m_SockStream.send_n(&read_data,sizeof read_data);

	// wait for free buff space
	m_WaitSpaceMutex.wait();

	return 0;
}

int yarp::ShmemHybridStream::read_buff(char* data,int size,bool bNonBlocking)
{
	m_RecvQueueMutex.wait();
	m_ResizeMutex.wait();

	if (m_RecvNData>0) // data available
	{
		int bytes_num=size<m_RecvNData?size:m_RecvNData;

		if (m_RecvTail+bytes_num>m_RecvBuffSize)
		{
			int FirstBlockLen=m_RecvBuffSize-m_RecvTail;

			memcpy((void*)data,(void*)(m_pRecvBuffer+m_RecvTail),FirstBlockLen);
			memcpy((void*)(data+FirstBlockLen),(void*)m_pRecvBuffer,bytes_num-FirstBlockLen);
		}
		else
		{
			memcpy((void*)data,(void*)(m_pRecvBuffer+m_RecvTail),bytes_num);
		}

		m_RecvNData-=bytes_num;
		m_RecvNFree+=bytes_num;
		m_RecvTail+=bytes_num;
		m_RecvTail%=m_RecvBuffSize;

		m_ResizeMutex.post();
		m_RecvQueueMutex.post();

		ShmemPacket_t read_data;
		read_data.command=READ;
		read_data.size=bytes_num;
		int ret=m_SockStream.send_n(&read_data,sizeof read_data);

		if (ret<=0)
		{
            if (!isStopping()) {
                YARP_ERROR(Logger::get(),
                           String("ShmemHybridStream socket writing error ")
                           +NetType::toString(ret));
            }
			Close();
			return -1;
		}

		return bytes_num;
	}

	if (bNonBlocking)
	{
		m_ResizeMutex.post();
		m_RecvQueueMutex.post();

		return 0;
	}

	// no data available

	m_bDataRequest=true;
	
	m_ResizeMutex.post();
	m_RecvQueueMutex.post();

	//ShmemPacket_t read_data;
	//read_data.command=WAKE_UP_MF;
	//m_SockStream.send_n(&read_data,sizeof read_data);

	// wait for data available
	m_WaitDataMutex.wait();

	return isStopping()?-1:0;
}

#endif
