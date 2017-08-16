/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_NETWORKCLOCK_H
#define YARP_OS_NETWORKCLOCK_H

#include <yarp/os/Clock.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/NetInt32.h>
#include <yarp/conf/numeric.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Mutex.h>

namespace yarp {
    namespace os {
        class NetworkClock;
    }
}


class YARP_OS_API yarp::os::NetworkClock : public Clock, PortReader {
public:
    NetworkClock();
    virtual ~NetworkClock();

    bool open(const ConstString& clockSourcePortName, ConstString localPortName="");

    virtual double now() override;
    virtual void delay(double seconds) override;
    virtual bool isValid() const override;

    virtual bool read(ConnectionReader& reader) override;
private:

    ConstString clockName;
    void *pwaiters;
    Port port;

    Mutex listMutex;
    Mutex timeMutex;

    YARP_INT32 sec;
    YARP_INT32 nsec;
    double _time;
    bool closing;
    bool initted;
};

#endif // YARP_OS_NETWORKCLOCK_H
