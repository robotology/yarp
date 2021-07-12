/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CANBUSINTERFACE_H
#define YARP_DEV_CANBUSINTERFACE_H

#include <cstring> // for std::memset

#include <yarp/dev/api.h>
#include <yarp/os/Log.h>

/*! \file CanBusInterface.h define interface for can bus devices*/

namespace yarp {
    namespace dev {
        class ICanBus;
        class ICanBufferFactory;
        class ICanBusErrors;
        class CanMessage;
        class CanBuffer;
        class CanErrors;
        template <class M, class T> class ImplementCanBufferFactory;
    }
}

class YARP_dev_API yarp::dev::CanErrors
{
public:
    CanErrors()
     {
        txCanErrors=0;     //can device tx errors
        rxCanErrors=0;     //can device rx errors
        busoff=false;         //bus off
        rxCanFifoOvr=0;   // can device rx overflow
        txCanFifoOvr=0;   // can device tx overflow
        txBufferOvr=0;    // tx buffer overflow
        rxBufferOvr=0;    // rx buffer overflow
    }

    int txCanErrors;     //can device tx errors
    int rxCanErrors;     //can device rx errors
    bool busoff;         //bus off
    unsigned int rxCanFifoOvr;   // can device rx overflow
    unsigned int txCanFifoOvr;   // can device tx overflow
    unsigned int txBufferOvr;    // tx buffer overflow
    unsigned int rxBufferOvr;    // rx buffer overflow
};

class YARP_dev_API yarp::dev::CanMessage
{
 public:
    virtual ~CanMessage(){}
    virtual CanMessage &operator=(const CanMessage &l)=0;
    virtual unsigned int getId() const =0;
    virtual unsigned char getLen() const =0;
    virtual void setLen(unsigned char len) =0;
    virtual void setId(unsigned int id) =0;
    virtual const unsigned char *getData() const =0;
    virtual unsigned char *getData() =0;
    virtual unsigned char *getPointer()=0;
    virtual const unsigned char *getPointer() const =0;
    virtual void setBuffer(unsigned char *)=0;
};

class YARP_dev_API yarp::dev::CanBuffer
{
    yarp::dev::CanMessage **data;
    int size;

public:
    CanBuffer();

    void resize(CanMessage **d, int s)
    {
        size=s;
        data=d;
    }

    CanMessage **getPointer()
    {
        return data;
    }

    const CanMessage *const *getPointer() const
    {
        return data;
    }

    CanMessage &operator[](int k)
    {
        return *data[k];
    }
    const CanMessage &operator[](int k) const
    {
        return *data[k];
    }
};

class YARP_dev_API yarp::dev::ICanBufferFactory
{
public:
    virtual ~ICanBufferFactory(){}
    virtual CanBuffer createBuffer(int nmessage)=0;
    virtual void destroyBuffer(CanBuffer &msgs)=0;
};

/**
 * Implementation of a ICanBufferFactory.
 * M is the class implementing CanMessage for your type.
 * IMPL is the internal representation of the can message.
 */
template<class M, class IMPL>
class yarp::dev::ImplementCanBufferFactory: public ICanBufferFactory
{
public:
    virtual ~ImplementCanBufferFactory(){}

    virtual CanBuffer createBuffer(int elem)
    {
        CanBuffer ret;
        IMPL *storage=new IMPL[elem];
        CanMessage **messages=new CanMessage *[elem];
        M *tmp=new M[elem];

        std::memset(storage, 0, sizeof(IMPL)*elem);

        for(int k=0;k<elem;k++)
            {
                messages[k]=&tmp[k];
                messages[k]->setBuffer((unsigned char *)(&storage[k]));
            }

        ret.resize(messages, elem);
        return ret;
    }

    virtual void destroyBuffer(CanBuffer &buffer)
    {
        CanMessage **m=buffer.getPointer();
        IMPL *storage=0;
        M *msgs=0;

        if (m==0)
            {
                yError("Warning trying to destroy non valid buffer\n");
                return;
            }

        storage=reinterpret_cast<IMPL *>(m[0]->getPointer());
        msgs=dynamic_cast<M *>(m[0]);

        if ((msgs==0)||(storage==0))
            {
                yError("Warning, troubles destroying memory\n");
                return;
            }

        delete [] storage;
        delete [] msgs;
        delete [] m;
    }
};

/**
 * Interface for a can bus device
 */
class YARP_dev_API yarp::dev::ICanBus
{
 public:
    virtual ~ICanBus(){}


    virtual bool canSetBaudRate(unsigned int rate)=0;
    virtual bool canGetBaudRate(unsigned int *rate)=0;
    virtual bool canIdAdd(unsigned int id)=0;
    virtual bool canIdDelete(unsigned int id)=0;

    virtual bool canRead(CanBuffer &msgs,
                         unsigned int size,
                         unsigned int *read,
                         bool wait=false)=0;

    virtual bool canWrite(const CanBuffer &msgs,
                          unsigned int size,
                          unsigned int *sent,
                          bool wait=false)=0;
};

class YARP_dev_API yarp::dev::ICanBusErrors
{
public:
    virtual bool canGetErrors(yarp::dev::CanErrors &err)=0;

    virtual ~ICanBusErrors(){}
};

#endif // YARP_DEV_CANBUSINTERFACE_H
