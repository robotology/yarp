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
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
struct Range_t
{
    double min;
    double max;
};

//---------------------------------------------------------------------------------------------------------------

class RpLidar2 : public RateThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    yarp::os::Mutex mutex;

    int sensorsNum;

    double min_angle;
    double max_angle;
    double min_distance;
    double max_distance;
    double resolution;
    bool clip_max_enable;
    bool clip_min_enable;
    bool do_not_clip_infinity_enable;
    std::vector <Range_t> range_skip_vector;

    std::string info;
    Device_status device_status;

    yarp::sig::Vector laser_data;

public:
    RpLidar2(int period = 10) : RateThread(period)
    {
    }


    ~RpLidar2()
    {
    }

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector &data);
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData> &data);
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

};

#endif
