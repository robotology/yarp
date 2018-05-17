/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
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
    std::string name;
    yarp::os::Contactable* contactable;

    virtual ~LoopablePort(){}
    LoopablePort():
        valid(false),
        count(0),
        contactable(nullptr)
    {}

    virtual void useCallback() = 0;

    virtual void onTimeout(double sec) = 0;
};

template <typename T>
struct yarp::dev::JoypadControl::JoyPort : public  yarp::dev::JoypadControl::LoopablePort,
                                           public  yarp::os::BufferedPort<T>
{
    typedef yarp::os::BufferedPort<T> bufferedPort;

    double          now;
    T               storage;
    yarp::os::Mutex mutex;

    JoyPort() : now(yarp::os::Time::now())
    {
        contactable = this;
    }
    using bufferedPort::useCallback;
    virtual void useCallback() override {bufferedPort::useCallback();}

    using yarp::os::TypedReaderCallback<T>::onRead;
    virtual void onRead(T& datum) override
    {
        now = yarp::os::Time::now();
        mutex.lock();
        storage = datum;
        mutex.unlock();
    }

    virtual void onTimeout(double sec) YARP_OVERRIDE
    {
        if((yarp::os::Time::now() - now) > sec)
        {
            mutex.lock();
            storage = T(count);
            mutex.unlock();
        }
    }
};

//----------
template<>
inline int BottleTagMap <unsigned char> () {
    return 64;
  }
