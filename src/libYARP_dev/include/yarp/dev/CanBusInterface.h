// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __YARPCANBUSINTERFACE__
#define __YARPCANBUSINTERFACE__

#include <yarp/dev/DeviceDriver.h>

/*! \file CanBusInterface.h define interface for can bus devices*/

namespace yarp {
    namespace dev {
        class ICanBus;
        class CanMessage;
        class CanMessageBuffer;
    }
}

class yarp::dev::CanMessage
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

class yarp::dev::CanMessageBuffer
{
    yarp::dev::CanMessage **data;
    int size;
 public:
    CanMessageBuffer()
        { data=0; }

    void resize(CanMessage **d, int s)
        {
            size=s;
            data=d;
        }

    CanMessage **getPointer()
        {
            return data;
        }

    CanMessage &operator[](int k)
        {
            return *data[k];
        }
};

template class CanBusMessageBufferFactory<class T>
{
public:
    static CanMessageBuffer createBuffer(int nmessage);
    static void destroyBuffer(CanMessageBuffer &msgs);
};


/**
 * Interface for a can bus device
 */
class yarp::dev::ICanBus
{
 public:
    virtual ~ICanBus(){}

    virtual CanMessageBuffer createBuffer(int nmessage)=0;
    virtual void destroyBuffer(CanMessageBuffer &msgs)=0;

    virtual bool canSetBaudRate(unsigned int rate)=0;
    virtual bool canGetBaudRate(unsigned int *rate)=0;
    virtual bool canIdAdd(unsigned int id)=0;
    virtual bool canIdDelete(unsigned int id)=0;

    virtual bool canRead(CanMessageBuffer &msgs, 
                         unsigned int size, 
                         unsigned int *read,
                         bool wait=false)=0;

    virtual bool canWrite(const CanMessageBuffer &msgs,
                          unsigned int size,
                          unsigned int *sent,
                          bool wait=false)=0;
};

#endif

