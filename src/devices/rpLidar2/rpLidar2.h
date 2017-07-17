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
#include <rplidar.h>

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
    typedef rp::standalone::rplidar::RPlidarDriver rplidardrv;

    void                  handleError(u_result error);
    yarp::os::ConstString deviceinfo();
protected:
    yarp::os::Mutex       m_mutex;
    int                   m_sensorsNum;
    int                   m_buffer_life;
    double                m_min_angle;
    double                m_max_angle;
    double                m_min_distance;
    double                m_max_distance;
    double                m_resolution;
    bool                  m_clip_max_enable;
    bool                  m_clip_min_enable;
    bool                  m_do_not_clip_infinity_enable;
    bool                  m_inExpressMode;
    int                   m_pwm_val;
    std::vector <Range_t> m_range_skip_vector;
    yarp::os::ConstString m_info;
    Device_status         m_device_status;
    yarp::sig::Vector     m_laser_data;
    rplidardrv*           m_drv;

public:
    RpLidar2(int period = 10) : RateThread(period)
    {
    }


    ~RpLidar2()
    {
    }

    virtual bool open(yarp::os::Searchable& config) override;
    virtual bool close() override;
    virtual bool threadInit() override;
    virtual void threadRelease() override;
    virtual void run() override;

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector &data) override;
    virtual bool getLaserMeasurement(std::vector<LaserMeasurementData> &data) override;
    virtual bool getDeviceStatus     (Device_status &status) override;
    virtual bool getDeviceInfo       (yarp::os::ConstString &device_info) override;
    virtual bool getDistanceRange    (double& min, double& max) override;
    virtual bool setDistanceRange    (double min, double max) override;
    virtual bool getScanLimits        (double& min, double& max) override;
    virtual bool setScanLimits        (double min, double max) override;
    virtual bool getHorizontalResolution      (double& step) override;
    virtual bool setHorizontalResolution      (double step) override;
    virtual bool getScanRate         (double& rate) override;
    virtual bool setScanRate         (double rate) override;

};

#endif
