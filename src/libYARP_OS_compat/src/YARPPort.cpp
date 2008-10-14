// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


///
/// $Id: YARPPort.cpp,v 1.9 2008-10-14 15:56:48 eshuy Exp $
//
/// Based on: Id: YARPPort.cpp,v 2.0 2005/11/06 22:21:26 gmetta Exp
//
/// Now, this is a compatibility layer, building YARPPorts from the 
/// new PortCore infrastructure.
///

/**
 * \file YARPPort.cpp It contains the implementation of the YARPPort object
 * and some additional private helper classes.
 *
 */



#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/PortReaderBuffer.h>

#define DEBUG_H_INC
#include <yarp/YARPPort.h>

using namespace yarp::os::impl;
using namespace yarp::os;


#ifdef __WIN32__
// library initialization.
#pragma init_seg(lib)
#endif

using namespace std;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class ConnectionWriter_to_YARPPortWriter : public YARPPortWriter {
private:
    ConnectionWriter& writer;
public:
    ConnectionWriter_to_YARPPortWriter(ConnectionWriter& writer) : writer(writer) {}

    virtual ~ConnectionWriter_to_YARPPortWriter() {}

    virtual int Write(char *buffer, int length) {
        //ACE_OS::printf("Writing %d bytes\n", length);
        //writer.appendBlock(Bytes(buffer,length));
        writer.appendBlock(buffer,length);
        return 1;
    }
};

class ConnectionReader_to_YARPPortReader : public YARPPortReader {
private:
    ConnectionReader& reader;
public:
    ConnectionReader_to_YARPPortReader(ConnectionReader& reader) : reader(reader) {}

    virtual ~ConnectionReader_to_YARPPortReader() {}

    virtual int Read(char *buffer, int length) {
        //ACE_OS::printf("Reading %d bytes\n", length);
        reader.expectBlock(buffer,length);
        return 1;
    }
};

class WritableContent : public Writable {
private:
    YARPPortContent& content;
public:
    WritableContent(YARPPortContent& content) : content(content) {}

    virtual ~WritableContent() {}

    virtual bool write(ConnectionWriter& writer) {
        ConnectionWriter_to_YARPPortWriter delegate(writer);
        int ok = content.Write(delegate);
        return ok!=0;
    }
};


class ReadableContent : public Readable {
private:
    YARPPortContent& content;
    int own;
public:
    ReadableContent(YARPPortContent& content) : content(content) {
        own = 0;
    }

    ReadableContent(YARPPortContent *content) : content(*content) {
        own = 1;
    }

    virtual ~ReadableContent() {
        if (own) {
            delete &content;
        }
    }

    virtual bool read(ConnectionReader& reader) {
        ConnectionReader_to_YARPPortReader delegate(reader);
        int result = content.Read(delegate);
        return (result!=0);
    }

    YARPPortContent& getYarpPortContent() {
        return content;
    }
};


/**
 * PortData is a simple convenient container for the generic Port class.
 */
class PortData : public Readable, public PortReaderBufferBaseCreator
{
public:
    PortCore core;
    Semaphore incoming;
    int service_type;

    PortData() : incoming(0) {
        ypc = NULL;
        service_type = YARPInputPort::TRIPLE_BUFFERS;
        buffer = NULL;
        currentBuffer = NULL;
    }

    virtual ~PortData() {
        if (ypc!=NULL) {
            delete ypc;
            ypc = NULL;
        }
        if (buffer!=NULL) {
            delete buffer;
            buffer = NULL;
        }
    }

    virtual PortReader *create() {
        if (in_owner!=NULL) {
            return new ReadableContent(in_owner->CreateContent());
        }
        return NULL;
    }

    /** The owner, if input port. */
    YARPInputPort *in_owner;
  
    /** The owner, if output port. */
    YARPOutputPort *out_owner;

    YARPPortContent *ypc;

    PortReaderBufferBase *buffer;
    ReadableContent *currentBuffer;
  
    /** The OnRead() callback function. */
    virtual void OnRead() { if (in_owner!=NULL) in_owner->OnRead(); }
  
    /** The OnWrite() callback function. */
    virtual void OnSend() { if (out_owner!=NULL) out_owner->OnWrite(); }

    void End() {
        core.close();
    }

    int SetName (const char *name, const char *net_name) {
        NameClient& nic = NameClient::getNameClient();
        Address address = nic.registerName(name);
        int result = core.listen(address);
        core.setReadHandler(*this);
        core.start();
        return result;
    }

    int CountClients() {
        ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
        ACE_OS::exit(1);
        return 0;
    }

    int IsSending() {
        ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
        ACE_OS::exit(1);
        return 0;
    }

    void FinishSend() {
        YARP_DEBUG(Logger::get(),"FinishSend does not need to do anything at the time of writing");
    }

    int Say(const char *str) {
        return Companion::connect(core.getName().c_str(),str);
    }

    void Deactivate() {
        core.close();
    }

    void Send() {
        YARPPortContent *content = ypc;
        YARP_ASSERT(content!=NULL);
        WritableContent wc(*content);
        //ACE_OS::printf("writing!\n");
        core.send(wc);
    }

    int Read(bool wait) {
        currentBuffer = NULL;
        if (wait) {
            if (buffer!=NULL) {
                currentBuffer = (ReadableContent *)buffer->readBase();
            }
            //printf("have a current buffer\n");
            return (currentBuffer!=NULL);
        } 
        return buffer->check();
    }

    virtual bool read(ConnectionReader& reader) {
        if (buffer!=NULL) {
            buffer->read(reader);
            OnRead();
        } else {
            ACE_OS::printf("an unexpected handler received data\n");
        }

        return true;
    }

    YARPPortContent *getYarpPortContent() {
        if (currentBuffer!=NULL) {
            return &(currentBuffer->getYarpPortContent());
        }
        return NULL;
    }
};

PortData& CastPortData(void *system_resource)
{
	ACE_ASSERT(system_resource!=NULL);
	return *((PortData *)system_resource);
}

#define PD CastPortData(system_resource)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


YARPPort::YARPPort()
{
	system_resource = new PortData;
	ACE_ASSERT(system_resource!=NULL);
	PD.in_owner = NULL;
	PD.out_owner = NULL;
}


YARPPort::~YARPPort()
{
	if (system_resource != NULL)
		delete ((PortData*)system_resource);
}


void YARPPort::End()
{
    PD.End();
}

int YARPPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
    Content();
    return 0==(PD.SetName (name, net_name));
}

int YARPPort::Unregister(void)
{
    PD.End ();
    return YARP_OK;
}

int YARPPort::IsReceiving()
{
    return PD.CountClients();
}

int YARPPort::IsSending()
{
    return PD.IsSending();
}

void YARPPort::FinishSend()
{
    PD.FinishSend();
}

int YARPPort::Connect(const char *name)
{
    int result = PD.Say(name);
    return (result==0);
}


int YARPPort::Connect(const char *src_name, const char *dest_name)
{
    int result = Companion::connect(src_name,dest_name);
    return (result==0);
}


YARPPortContent& YARPPort::Content()
{
    YARPPortContent *con = PD.getYarpPortContent();
    //printf("scanning for new content.........\n");
    if (con!=NULL) {
        //printf("new content\n");
        content = con;
        return *con;
    }
    if (PD.ypc==NULL) {
        PD.ypc = CreateContent();
    }
    content = PD.ypc;
    return *(PD.ypc);
}


void YARPPort::Deactivate()
{
	PD.Deactivate();
}


void YARPPort::DeactivateAll()
{
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    ACE_OS::exit(1);
}


void YARPPort::SetRequireAck(int require_ack) 
{
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    //  PD.SetRequireAck(require_ack);
}


int YARPPort::GetRequireAck()
{
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    return 1;
    //return PD.GetRequireAck();
}



YARPInputPort::YARPInputPort(int n_service_type, int n_protocol_type)
{
    PD.service_type = n_service_type;
    //PD.protocol_type = n_protocol_type;
    int ct = 0;
    if (n_service_type==YARPInputPort::NO_BUFFERS) {
        ct = 1;
    } else if (n_service_type==YARPInputPort::DOUBLE_BUFFERS) {
        ct = 2;
    } else {
        ct = 3;
    }
    PD.buffer = new PortReaderBufferBase(ct);
    YARP_ASSERT(PD.buffer!=NULL);
    PD.buffer->setCreator(&PD);
    PD.in_owner = this;
}


YARPInputPort::~YARPInputPort()
{
}


int YARPInputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
	return YARPPort::Register(name, net_name);
}


bool YARPInputPort::Read(bool wait)
{
    Content();
    bool result = PD.Read(wait);
    Content();
    return result;
}


YARPOutputPort::YARPOutputPort(int n_service_type, int n_protocol_type)
{
    PD.out_owner = this;
}


YARPOutputPort::~YARPOutputPort()
{
}


int YARPOutputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
    return YARPPort::Register(name, net_name);
}


YARPPortContent& YARPOutputPort::Content()
{
    return YARPPort::Content();
}



void YARPOutputPort::Write(bool wait)
{
    PD.Send();
}


void YARPOutputPort::SetAllowShmem(int flag)
{
    if (flag) {
        ACE_OS::printf("%s:%d -- shared memory implemention is currently partial\n",
                       __FILE__,__LINE__);
        //PD.SetAllowShmem(flag);
    }
}


int YARPOutputPort::GetAllowShmem(void)
{
    ACE_OS::printf("%s:%d -- shared memory implemention is currently partial\n",
                   __FILE__,__LINE__);
    //return PD.GetAllowShmem();
    return 0;
}
