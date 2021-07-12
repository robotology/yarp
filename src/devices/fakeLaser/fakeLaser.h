/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_LASER_H
#define FAKE_LASER_H


#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>

#include <mutex>
#include <random>
#include <string>

/**
* @ingroup dev_impl_fake dev_impl_lidar
*
* @brief `fakeLaser` : fake sensor device driver for testing purposes and reference for IRangefinder2D devices.
*
* | YARP device name |
* |:-----------------:|
* | `fakeLaser` |
*
* \section fakeLaser_device_parameters Description of input parameters
*
* Parameters accepted in the config argument of the open method:
* | Parameter name      | Type   | Units | Default Value | Required | Description | Notes |
* |:-------------------:|:------:|:-----:|:-------------:|:--------:|:-----------:|:-----:|
* | test                | string |   -   |       -       | Yes      | Choose the modality   | It can be one of the following: no_obstacles, use_pattern, use_mapfile |
* | localization_port   | string |   -   |       -       | No       | Full name of the port to which device connects to receive the localization data   |  |
* | localization_client | string |   -   |       -       | No       | Full name of the local transformClient opened by the device | It cannot be used togheter if localization_port parameter is set |
* | map_file            | string |   -   |       -       | No       | Full path to a .map file   | Mandatory if --test use_mapfile option has been set |
* | clip_max            | double |   m   | 3.5           | No       | Maximum detectable distance for an obstacle | - |
* | clip_min            | double |   m   | 0.1           | No       | Minimum detectable distance for an obstacle | - |
* | max_angle           | double |  deg  | 360           | No       | Angular range of the sensor  | - |
* | min_angle           | double |  deg  |   0           | No       | Angular range of the sensor  | - |
* | resolution          | double |  deg  | 1.0           | No       | Device resolution          | - |
*
* \section Usage examples:
* yarpdev --device fakeLaser --help
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test no_obstacles
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_pattern
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i
* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient
*/

class FakeLaser : public yarp::os::PeriodicThread,
                  public yarp::dev::Lidar2DDeviceBase,
                  public yarp::dev::DeviceDriver,
                  public yarp::os::PortReader
{
protected:
    enum test_mode_t { NO_OBSTACLES = 0, USE_PATTERN =1, USE_MAPFILE =2, USE_CONSTANT_VALUE =3 };
    enum localization_mode_t { LOC_NOT_SET=0, LOC_FROM_PORT = 1, LOC_FROM_CLIENT = 2 };

    yarp::dev::PolyDriver driver;
    test_mode_t m_test_mode;
    localization_mode_t m_loc_mode;

    double m_period;
    yarp::dev::Nav2D::MapGrid2D   m_originally_loaded_map;
    yarp::dev::Nav2D::MapGrid2D   m_map;
    yarp::os::BufferedPort<yarp::os::Bottle>* m_loc_port;
    yarp::dev::PolyDriver*      m_pLoc;
    yarp::dev::Nav2D::ILocalization2D* m_iLoc;

    //this is the position of the localized robot in the map
    double m_robot_loc_x;
    double m_robot_loc_y;
    double m_robot_loc_t;

    std::random_device* m_rd;
    std::mt19937* m_gen;
    std::uniform_real_distribution<>* m_dis;
    double m_const_value=1;

    yarp::os::Port  m_rpcPort;

public:
    FakeLaser(double period = 0.02) : PeriodicThread(period),
        m_test_mode(test_mode_t::NO_OBSTACLES),
        m_loc_mode(localization_mode_t::LOC_NOT_SET),
        m_loc_port(nullptr),
        m_pLoc(nullptr),
        m_iLoc(nullptr),
        m_robot_loc_x(0.0),
        m_robot_loc_y(0.0),
        m_robot_loc_t(0.0)
    {
        //default parameters
        m_min_distance = 0.1;  //m
        m_max_distance = 8.0;  //m
        m_min_angle = 0;       //degrees
        m_max_angle = 360;     //degrees
        m_resolution = 1.0;    //degrees

        //noise generator
        m_rd  = new std::random_device;
        m_gen = new std::mt19937((*m_rd)());
        m_dis = new std::uniform_real_distribution<>(0, 0.01);
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

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    void drawStraightLine(yarp::dev::Nav2D::XYCell src, yarp::dev::Nav2D::XYCell dst);
    void wall_the_robot(double siz = 1.0, double dist = 1.0);
    void obst_the_robot(double siz = 1.0, double dist = 1.0);
    void trap_the_robot(double siz = 1.0);
    void free_the_robot();

    double checkStraightLine(yarp::dev::Nav2D::XYCell src, yarp::dev::Nav2D::XYCell dst);
    bool   LiangBarsky_clip(int edgeLeft, int edgeRight, int edgeTop, int edgeBottom,
                            yarp::dev::Nav2D::XYCell_unbounded src, yarp::dev::Nav2D::XYCell_unbounded dst,
                            yarp::dev::Nav2D::XYCell& src_clipped, yarp::dev::Nav2D::XYCell& dst_clipped);

public:
    //IRangefinder2D interface
    bool setDistanceRange    (double min, double max) override;
    bool setScanLimits        (double min, double max) override;
    bool setHorizontalResolution      (double step) override;
    bool setScanRate         (double rate) override;

public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;

public:
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif
