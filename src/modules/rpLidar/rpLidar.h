// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/


#ifndef RPLIDAR_H
#define RPLIDAR_H

#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/SerialInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;

class RpLidar : public RateThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    PolyDriver driver;
    ISerialDevice *pSerial;

    yarp::os::Semaphore mutex;

    int period;
    int sensorsNum;

    double min_angle;
    double max_angle;
    double min_distance;
    double max_distance;
    double resolution;

    std::string info;
    Device_status device_status;

    yarp::sig::Vector laser_data;

public:
    RpLidar(int period = 20) : RateThread(period), mutex(1)
    {}
    

    ~RpLidar()
    {
    }

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

public:
    //IRangefinder2D interface
    virtual bool getMeasurementData  (yarp::sig::Vector &out);
    virtual bool getDeviceStatus     (Device_status &status);
    virtual bool getDeviceInfo       (yarp::os::ConstString &device_info);
    virtual bool getDistanceRange    (double& min, double& max);
    virtual bool setDistanceRange    (double min, double max);
    virtual bool getScanLimits        (double& min, double& max);
    virtual bool setScanLimits        (double min, double max);
    virtual bool getHorizontalResolution      (double& step);
    virtual bool setHorizontalResolution      (double step);
    virtual bool getScanRate         (double& rate);
    virtual bool setScanRate         (double rate);

private:
    bool  HW_getHealth();
    bool  HW_reset();
    bool  HW_start();
    bool  HW_stop();
    bool  HW_getInfo(std::string& s_info);

};

#endif
