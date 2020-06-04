/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#define _USE_MATH_DEFINES

#include "fakeLaser.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Vec2D.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <cstdlib>
#include <cmath>

//#define LASER_DEBUG
#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

YARP_LOG_COMPONENT(FAKE_LASER, "yarp.devices.fakeLaser")

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

// see FakeLaser.h for device documentation

bool FakeLaser::open(yarp::os::Searchable& config)
{
    m_info = "Fake Laser device for test/debugging";
    m_device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER) << "%s\n", config.toString().c_str());
#endif

    if (config.check("help"))
    {
        yCInfo(FAKE_LASER,"Some examples:");
        yCInfo(FAKE_LASER,"yarpdev --device fakeLaser --help");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test no_obstacles");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_pattern");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_constant --const_distance 0.5");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_constant --const_distance 0.5 --SENSOR::resolution 0.5 --SKIP::min 0 50 --SKIP::max 10 60");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_file mymap.map");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_server /localizationServer");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient --localization_server /localizationServer");
        return false;
    }

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        m_period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt32() / 1000.0;
    }

    string string_test_mode = config.check("test", Value(string("use_pattern")), "string to select test mode").asString();
    if      (string_test_mode == "no_obstacles") { m_test_mode = NO_OBSTACLES; }
    else if (string_test_mode == "use_pattern") { m_test_mode = USE_PATTERN; }
    else if (string_test_mode == "use_mapfile") { m_test_mode = USE_MAPFILE; }
    else if (string_test_mode == "use_constant") { m_test_mode = USE_CONSTANT_VALUE; }
    else    { yCError(FAKE_LASER) << "invalid/unknown value for param 'test'"; return false; }

    //parse all the parameters related to the linear/angular range of the sensor
    if (this->parseConfiguration(config) == false)
    {
        yCError(FAKE_LASER) << ": error parsing parameters";
        return false;
    }

    //the different fake laser modalities
    else if (m_test_mode == USE_CONSTANT_VALUE)
    {
        if (config.check("const_distance"))
        {
            m_const_value = config.check("const_distance", Value(1.0), "default constant distance").asDouble();
        }
    }
    else if (m_test_mode == USE_MAPFILE)
    {
        string map_file;
        if (config.check("map_file"))
        {
            map_file = config.check("map_file",Value(string("map.yaml")),"map filename").asString();
        }
        else
        {
            yCError(FAKE_LASER) << "Missing map_file";
            return false;
        }
        bool ret = m_map.loadFromFile(map_file);
        if (ret == false)
        {
            yCError(FAKE_LASER) << "A problem occurred while opening:" << map_file;
            return false;
        }

        if (config.check("localization_port"))
        {
            string localization_port_name = config.check("localization_port", Value(string("/fakeLaser/location:i")), "name of localization input port").asString();
            m_loc_port = new yarp::os::BufferedPort<yarp::os::Bottle>;
            m_loc_port->open(localization_port_name);
            yCInfo(FAKE_LASER) << "Robot localization will be obtained from port" << localization_port_name;
            m_loc_mode = LOC_FROM_PORT;
        }
        else if (config.check("localization_client") ||
                 config.check("localization_server" ))
        {
            Property loc_options;
            string localization_client_name = config.check("localization_client", Value(string("/fakeLaser/localizationClient")), "local name of localization client device").asString();
            string localization_server_name = config.check("localization_server", Value(string("/localizationServer")), "the name of the remote localization server device").asString();
            loc_options.put("device", "localization2DClient");
            loc_options.put("local", localization_client_name);
            loc_options.put("remote", localization_server_name);
            loc_options.put("period", 10);
            m_pLoc = new PolyDriver;
            if (m_pLoc->open(loc_options) == false)
            {
                yCError(FAKE_LASER) << "Unable to open localization driver";
                return false;
            }
            m_pLoc->view(m_iLoc);
            if (m_iLoc == nullptr)
            {
                yCError(FAKE_LASER) << "Unable to open localization interface";
                return false;
            }
            yCInfo(FAKE_LASER) << "Robot localization will be obtained from localization server: " << localization_server_name;
            m_loc_mode = LOC_FROM_CLIENT;
        }
        else
        {
            yCInfo(FAKE_LASER) << "No localization method selected. Robot location set to 0,0,0";
            m_loc_mode = LOC_NOT_SET;
        }

        m_robot_loc_x=0;
        m_robot_loc_y=0;
        m_robot_loc_t=0;
    }

    yCInfo(FAKE_LASER) << "Starting debug mode";
    yCInfo(FAKE_LASER) << "test mode:"<< m_test_mode;
    return PeriodicThread::start();
}

bool FakeLaser::close()
{
    PeriodicThread::stop();

    driver.close();

    if (m_loc_port)
    {
        m_loc_port->interrupt();
        m_loc_port->close();
    }
    return true;
}

bool FakeLaser::setDistanceRange(double min, double max)
{
    m_mutex.lock();
    m_min_distance = min;
    m_max_distance = max;
    m_mutex.unlock();
    return true;
}

bool FakeLaser::setScanLimits(double min, double max)
{
    m_mutex.lock();
    m_min_angle = min;
    m_max_angle = max;
    m_mutex.unlock();
    return true;
}

bool FakeLaser::setHorizontalResolution(double step)
{
    m_mutex.lock();
    m_resolution = step;
    m_mutex.unlock();
    return true;
}

bool FakeLaser::setScanRate(double rate)
{
    m_mutex.lock();
    m_period = (1.0 / rate);
    m_mutex.unlock();
    return false;
}

bool FakeLaser::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER)<<"thread initialising...\n");
    yCDebug(FAKE_LASER)<<"... done!\n");
#endif

    return true;
}

void FakeLaser::run()
{
    m_mutex.lock();
    m_laser_data.clear();
    double t      = yarp::os::Time::now();
    static double t_orig = yarp::os::Time::now();
    double size = (t - (t_orig));

    static int test_count = 0;
    static int test = 0;

    if (m_test_mode == USE_PATTERN)
    {
        for (size_t i = 0; i < m_sensorsNum; i++)
        {
            double value = 0;
            if (test == 0)
                { value = i / 100.0; }
            else if (test == 1)
                { value = size * 2; }
            else if (test == 2)
            {
                if (i <= 10) { value = 1.0 + i / 20.0; }
                else if (i >= 90 && i <= 100) { value = 2.0 + (i - 90) / 20.0; }
                else { value = m_min_distance; }
            }
            m_laser_data.push_back(value);
        }

        test_count++;
        if (test_count == 60)
        {
            test_count = 0; test++; if (test > 2) { test = 0; }
            t_orig = yarp::os::Time::now();
        }
    }
    else if (m_test_mode == NO_OBSTACLES)
    {
        for (size_t i = 0; i < m_sensorsNum; i++)
        {
            m_laser_data.push_back(std::numeric_limits<double>::infinity());
        }
    }
    else if (m_test_mode == USE_CONSTANT_VALUE)
    {
        for (size_t i = 0; i < m_sensorsNum; i++)
        {
            m_laser_data.push_back(m_const_value);
        }
    }
    else if (m_test_mode == USE_MAPFILE)
    {
        if (m_loc_mode == LOC_NOT_SET)
        {
            //do nothing
        }
        else if (m_loc_mode == LOC_FROM_PORT)
        {
            Bottle* b = m_loc_port->read(false);
            if (b)
            {
                m_robot_loc_x = b->get(0).asFloat64();
                m_robot_loc_y = b->get(1).asFloat64();
                m_robot_loc_t = b->get(2).asFloat64();
            }
        }
        else if (m_loc_mode == LOC_FROM_CLIENT)
        {
            Map2DLocation loc;
            if (m_iLoc->getCurrentPosition(loc))
            {
                m_robot_loc_x = loc.x;
                m_robot_loc_y = loc.y;
                m_robot_loc_t = loc.theta;
            }
            else
            {
                yCError(FAKE_LASER) << "Error occurred while getting current position from localization server";
            }
        }
        else
        {
            yCDebug(FAKE_LASER) << "No localization mode selected. This branch should be not reachable.";
        }

        for (size_t i = 0; i < m_sensorsNum; i++)
        {
            //compute the ray in the robot reference frame
            double ray_angle = i* m_resolution + m_min_angle;
            double ray_target_x = m_max_distance * cos(ray_angle * DEG2RAD);
            double ray_target_y = m_max_distance * sin(ray_angle * DEG2RAD);

            //transforms the ray from the robot to the world reference frame
            XYWorld ray_world;
            ray_world.x = ray_target_x *cos(m_robot_loc_t*DEG2RAD) - ray_target_y *sin(m_robot_loc_t*DEG2RAD) + m_robot_loc_x;
            ray_world.y = ray_target_x *sin(m_robot_loc_t*DEG2RAD) + ray_target_y *cos(m_robot_loc_t*DEG2RAD) + m_robot_loc_y;
            XYCell src = m_map.world2Cell(XYWorld(m_robot_loc_x, m_robot_loc_y));
            if (m_map.isInsideMap(ray_world))
            {
                XYCell dst = m_map.world2Cell(ray_world);
                double distance = checkStraightLine(src, dst);
                double simulated_noise = (*m_dis)(*m_gen);
                m_laser_data.push_back(distance + simulated_noise);
            }
            else
            {
                m_laser_data.push_back(std::numeric_limits<double>::infinity());
            }
        }
    }
    //for all the different types of tests, apply the limits
    applyLimitsOnLaserData();

    //set the device status
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;

    m_mutex.unlock();
    return;
}

double FakeLaser::checkStraightLine(XYCell src, XYCell dst)
{
    XYCell src_final = src;

    //here using the fast Bresenham algorithm
    int dx = abs(int(dst.x - src.x));
    int dy = abs(int(dst.y - src.y));
    int err = dx - dy;

    int sx;
    int sy;
    if (src.x < dst.x) { sx = 1; } else { sx = -1; }
    if (src.y < dst.y) { sy = 1; } else { sy = -1; }

    while (true)
    {
        //if (m_map.isFree(src) == false)
        if (m_map.isWall(src))
        {
            XYWorld world_start =  m_map.cell2World(src);
            XYWorld world_end =  m_map.cell2World(src_final);
            return sqrt(pow(world_start.x - world_end.x, 2) + pow(world_start.y - world_end.y, 2));
        }
        if (src.x == dst.x && src.y == dst.y) break;
        int e2 = err * 2;
        if (e2 > -dy)
        {
            err = err - dy;
            src.x += sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            src.y += sy;
        }
    }
    return std::numeric_limits<double>::infinity();
}

void FakeLaser::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER) << "FakeLaser Thread releasing...");
    yCDebug(FAKE_LASER) << "... done.");
#endif
}