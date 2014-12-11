// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NETWORKCLOCK_
#define _YARP2_NETWORKCLOCK_

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
    
    bool open(const ConstString& name);

    virtual double now();
    virtual void delay(double seconds);
    virtual bool isValid() const;

    virtual bool read(ConnectionReader& reader);
private:
    void *pwaiters;
    Port port;

    Mutex listMutex;
    Mutex timeMutex;

    YARP_INT32 sec;
    YARP_INT32 nsec;
    double t;
    bool closing;
};

#endif



