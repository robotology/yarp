// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __INERTIALCUBE2__
#define __INERTIALCUBE2__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>

namespace yarp{
    namespace dev{
        class InertiaCube2;
    }
}

struct InertiaCube2Parameters
{
    short comPort;
};

class yarp::dev::InertiaCube2 : public IGenericSensor, DeviceDriver
{
 public:
    InertiaCube2();
    ~InertiaCube2();

    virtual bool read(yarp::sig::Vector &out);
    virtual bool getChannel(int *nc);

    bool open(const InertiaCube2Parameters &par);
    bool close();

 private:
    bool start();
    bool stop();

    void *system_resources;
    int nchannels;
};

#endif
//
