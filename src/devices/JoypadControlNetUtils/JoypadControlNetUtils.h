/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <mutex>

//TODO: finish the single port mode.. the struct below is for this purpose
struct JoyData : public yarp::os::Portable
{
    yarp::sig::Vector Buttons;
    yarp::sig::Vector Sticks;
    yarp::sig::Vector Axes;
    yarp::sig::Vector Balls;
    yarp::sig::Vector Touch;
    yarp::sig::VectorOf<unsigned char> Hats;

    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
};

namespace JoypadControl {

struct LoopablePort
{
    bool valid{false};
    unsigned int count{0};
    std::string name;
    yarp::os::Contactable* contactable{nullptr};

    LoopablePort() = default;
    virtual ~LoopablePort() = default;

    virtual void useCallback() = 0;

    virtual void onTimeout(double sec) = 0;
};

template <typename T>
struct JoyPort :
        public JoypadControl::LoopablePort,
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

    void onTimeout(double sec) override
    {
        if((yarp::os::Time::now() - now) > sec)
        {
            mutex.lock();
            storage = T(count);
            mutex.unlock();
        }
    }
};

} // namespace JoypadControl

//----------
template<>
inline int BottleTagMap <unsigned char> () {
    return 64;
  }
