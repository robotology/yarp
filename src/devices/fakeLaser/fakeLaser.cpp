/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/
 
#include <fakeLaser.h>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

//#define LASER_DEBUG
#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

using namespace std;

bool FakeLaser::open(yarp::os::Searchable& config)
{
    info = "Fake Laser device for test/debugging";
    device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt();
    }

    min_distance = 0.1; //m
    max_distance = 3.5;  //m
    min_angle = 0;      //degrees
    max_angle = 359;    //degrees
    resolution = 1.0;   //degrees

    sensorsNum = (int)((max_angle-min_angle)/resolution);
    laser_data.resize(sensorsNum);
    
    yInfo("Starting debug mode");
    yInfo("max_dist %f, min_dist %f", max_distance, min_distance);
    yInfo("max_angle %f, min_angle %f", max_angle, min_angle);
    yInfo("resolution %f", resolution);
    yInfo("sensors %d", sensorsNum);
    Time::turboBoost();
    RateThread::start();
    return true;
}

bool FakeLaser::close()
{
    RateThread::stop();

    driver.close();
    return true;
}

bool FakeLaser::getDistanceRange(double& min, double& max)
{
    mutex.wait();
    min = min_distance;
    max = max_distance;
    mutex.post();
    return true;
}

bool FakeLaser::setDistanceRange(double min, double max)
{
    mutex.wait();
    min_distance = min;
    max_distance = max;
    mutex.post();
    return true;
}

bool FakeLaser::getScanLimits(double& min, double& max)
{
    mutex.wait();
    min = min_angle;
    max = max_angle;
    mutex.post();
    return true;
}

bool FakeLaser::setScanLimits(double min, double max)
{
    mutex.wait();
    min_angle = min;
    max_angle = max;
    mutex.post();
    return true;
}

bool FakeLaser::getHorizontalResolution(double& step)
{
    mutex.wait();
    step = resolution;
    mutex.post();
    return true;
}

bool FakeLaser::setHorizontalResolution(double step)
{
    mutex.wait();
    resolution = step;
    mutex.post();
    return true;
}

bool FakeLaser::getScanRate(double& rate)
{
    mutex.wait();
    rate = 1.0 / (period * 1000);
    mutex.post();
    return true;
}

bool FakeLaser::setScanRate(double rate)
{
    mutex.wait();
    period = (int)((1.0 / rate) / 1000.0);
    mutex.post();
    return false;
}


bool FakeLaser::getRawMeasurementData(yarp::sig::Vector &out)
{
    mutex.wait();
    out = laser_data;
    mutex.post();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool FakeLaser::getPolarMeasurementData(std::vector<PolarMeasurementData> &data)
{
    mutex.wait();
#ifdef LASER_DEBUG
    //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = laser_data.size();
    data.resize(size);
    double laser_angle_of_view = fabs(min_angle) + fabs(max_angle);
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + min_angle)* DEG2RAD;
        data[i].distance = laser_data[i];
        data[i].angle = angle;
    }
    mutex.post();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool FakeLaser::getCartesianMeasurementData(std::vector<CartesianMeasurementData> &data)
{
    mutex.wait();
#ifdef LASER_DEBUG
    //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = laser_data.size();
    data.resize(size);
    double laser_angle_of_view = fabs(min_angle) + fabs(max_angle);
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + min_angle)* DEG2RAD;
        data[i].x = laser_data[i] * cos(angle);
        data[i].y = laser_data[i] * sin(angle);
    }
    mutex.post();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool FakeLaser::getDeviceStatus(Device_status &status)
{
    mutex.wait();
    status = device_status;
    mutex.post();
    return true; 
}

bool FakeLaser::threadInit()
{
#ifdef LASER_DEBUG
    yDebug("FakeLaser:: thread initialising...\n");
    yDebug("... done!\n");
#endif 

    return true;
}

void FakeLaser::run()
{
    mutex.wait();
    laser_data.clear();
    double t = yarp::os::Time::now();
    double size = (t - int(t));
    
    static int test_count = 0;
    static int test = 0;

    for (int i = 0; i < sensorsNum; i++)
    {
        double value = 0;
        if (test == 0)
            value = i / 100.0;
        else if (test == 1)
            value = size*2;

        if (value < min_distance) value = min_distance;
        if (value > max_distance) value = max_distance;
        laser_data.push_back(value);
    }

    test_count++;
    if (test_count == 40)
    {
        test_count = 0; test++; if (test > 1) test = 0;
    }

    mutex.post();
    return;
}

void FakeLaser::threadRelease()
{
#ifdef LASER_DEBUG
    yDebug("FakeLaser Thread releasing...");
    yDebug("... done.");
#endif
}

bool FakeLaser::getDeviceInfo(yarp::os::ConstString &device_info)
{
    this->mutex.wait();
    device_info = info;
    this->mutex.post();
    return true;
}
