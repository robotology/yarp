/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <mutex>

//-----Openable and JoyPort are for comfortable loop managing of ports
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
        Buttons.resize(connection.expectInt32());
        Sticks.resize(connection.expectInt32());
        Axes.resize(connection.expectInt32());
        Balls.resize(connection.expectInt32());
        Touch.resize(connection.expectInt32());
        Hats.resize(connection.expectInt32());
        connection.expectBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
        connection.expectBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
        connection.expectBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
        connection.expectBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
        connection.expectBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
        connection.expectBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
        return !connection.isError();
    }

    bool write(yarp::os::ConnectionWriter& connection) const override
    {
        connection.appendInt32(Buttons.length());
        connection.appendInt32(Sticks.length());
        connection.appendInt32(Axes.length()  );
        connection.appendInt32(Balls.length() );
        connection.appendInt32(Touch.length() );
        connection.appendInt32(Hats.size()  );
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
    std::mutex mutex;

    JoyPort() : now(yarp::os::Time::now())
    {
        contactable = this;
    }
    using bufferedPort::useCallback;
    void useCallback() override {bufferedPort::useCallback();}

    using yarp::os::TypedReaderCallback<T>::onRead;
    void onRead(T& datum) override
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
