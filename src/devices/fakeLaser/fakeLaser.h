/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/


#ifndef FAKE_LASER_H
#define FAKE_LASER_H

#include <string>

#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;

/**
* @ingroup dev_impl_wrapper
* FakeLaser sensor device for testing purposes and reference for IRangefinder2D devices
*
* \section Description of input parameters
*
* Parameters accepted in the config argument of the open method:
* | Parameter name      | Type   | Units | Default Value | Required | Description | Notes |
* |:-------------------:|:------:|:-----:|:-------------:|:--------:|:-----------:|:-----:|
* | test                | string |   -   |       -       | Yes      | Choose the modality   | It can be one of the following: no_obstacles, use_pattern, use_mapfile | 
* | localization_port   | string |   -   |       -       | No       | Full name of the port to which device connects to receive the localization data   |  | 
* | localization_client | string |   -   |       -       | No       | Full name of the local transformClient opened by the device | It cannot be used togheter if localization_port parameter is set | 
* | map_file            | string |   -   |       -       | No       | Full path to a .map file   | Mandatory if --test use_mapfile option has been set | 
*
* \section Usage examples:
* yarpdev --device fakeLaser --help
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test no_obstacles
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_pattern
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient
*/

class FakeLaser : public RateThread, public yarp::dev::IRangefinder2D, public DeviceDriver
{
protected:
    enum test_mode_t { NO_OBSTACLES = 0, USE_PATTERN =1, USE_MAPFILE =2 };
    enum localization_mode_t { LOC_NOT_SET=0, LOC_FROM_PORT = 1, LOC_FROM_CLIENT = 2 };

    PolyDriver driver;
    test_mode_t m_test_mode;
    localization_mode_t m_loc_mode;
    yarp::os::Semaphore mutex;

    int period;
    int sensorsNum;

    double min_angle;
    double max_angle;
    double min_distance;
    double max_distance;
    double resolution;

    yarp::dev::MapGrid2D   m_map;
    yarp::os::BufferedPort<yarp::os::Bottle>* m_loc_port;
    PolyDriver*      m_pLoc;
    ILocalization2D* m_iLoc;
    double m_loc_x;
    double m_loc_y;
    double m_loc_t;

    std::string info;
    Device_status device_status;

    yarp::sig::Vector laser_data;

    std::random_device* m_rd;
    std::mt19937* m_gen;
    std::uniform_real_distribution<>* m_dis;

public:
    FakeLaser(int period = 20) : RateThread(period), mutex(1)
    {
        m_rd = new std::random_device;
        m_gen = new std::mt19937((*m_rd)());
        m_dis = new std::uniform_real_distribution<>(0, 0.01);
        m_loc_port = 0;
        m_pLoc = 0;
        m_iLoc = 0;
    }
    
    ~FakeLaser()
    {
        delete m_rd;
        delete m_gen;
        delete m_dis;
        m_rd = 0;
        m_gen = 0;
        m_dis = 0;
        if (m_loc_port)
        {
           delete m_loc_port;
           m_loc_port = 0;
        }
        if (m_pLoc)
        {
            delete m_pLoc;
            m_pLoc = 0;
        }
    }

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

private:
    double checkStraightLine(MapGrid2D::XYCell src, MapGrid2D::XYCell dst);

public:
    //IRangefinder2D interface
    virtual bool getRawData(yarp::sig::Vector &out);
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
