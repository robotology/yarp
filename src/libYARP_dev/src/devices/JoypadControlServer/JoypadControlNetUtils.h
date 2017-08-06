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

//TODO: finish the single port mode.. the struct below is for this purpose
struct JoyData : public yarp::os::Portable
{
    yarp::sig::Vector Buttons;
    yarp::sig::Vector Sticks;
    yarp::sig::Vector Axes;
    yarp::sig::Vector Balls;
    yarp::sig::Vector Touch;
    yarp::sig::VectorOf<unsigned char> Hats;

    bool read(yarp::os::ConnectionReader& connection) override
    {
        Buttons.resize(connection.expectInt());
        Sticks.resize(connection.expectInt());
        Axes.resize(connection.expectInt());
        Balls.resize(connection.expectInt());
        Touch.resize(connection.expectInt());
        Hats.resize(connection.expectInt());
        connection.expectBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
        connection.expectBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
        connection.expectBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
        connection.expectBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
        connection.expectBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
        connection.expectBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
        return !connection.isError();
    }

    bool write(yarp::os::ConnectionWriter& connection) override
    {
        connection.appendInt(Buttons.length());
        connection.appendInt(Sticks.length());
        connection.appendInt(Axes.length()  );
        connection.appendInt(Balls.length() );
        connection.appendInt(Touch.length() );
        connection.appendInt(Hats.size()  );
        connection.appendBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
        connection.appendBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
        connection.appendBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
        connection.appendBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
        connection.appendBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
        connection.appendBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
        connection.convertTextMode();
        return !connection.isError();
    }
};

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
