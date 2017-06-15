/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
//-----Openable and JoyPort are for confortable loop managing of ports
namespace yarp
{
    namespace dev
    {
        namespace JoypadControl
        {
            struct                       LoopablePort;
            template <typename T> struct JoyPort;
        }
    }
}
struct yarp::dev::JoypadControl::LoopablePort
{
    bool                  valid;
    unsigned int          count;
    yarp::os::ConstString name;
    virtual ~LoopablePort(){}
    LoopablePort():valid(false),count(0){}

    virtual void useCallback() = 0;

    yarp::os::Contactable* contactable;
};

template <typename T>
struct yarp::dev::JoypadControl::JoyPort : public  yarp::dev::JoypadControl::LoopablePort,
                                           public  yarp::os::BufferedPort<T>
{
    typedef yarp::os::BufferedPort<T> bufferedPort;

    T               storage;
    yarp::os::Mutex mutex;

    JoyPort() {contactable = this;}
    using bufferedPort::useCallback;
    virtual void useCallback() YARP_OVERRIDE {bufferedPort::useCallback();}

    using yarp::os::TypedReaderCallback<T>::onRead;
    virtual void onRead(T& datum) YARP_OVERRIDE
    {
        mutex.lock();
        storage = datum;
        mutex.unlock();
    }
};

//----------
template<>
inline int BottleTagMap <unsigned char> () {
    return 64;
  }
