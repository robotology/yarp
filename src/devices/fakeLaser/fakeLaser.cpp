/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

// see FakeLaser.h for device documentation

bool FakeLaser::open(yarp::os::Searchable& config)
{
    info = "Fake Laser device for test/debugging";
    device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    if (config.check("help"))
    {
        yInfo("Some examples:");
        yInfo("yarpdev --device fakeLaser --help");
        yInfo("yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test no_obstacles");
        yInfo("yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_pattern");
        yInfo("yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map");
        yInfo("yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i");
        yInfo("yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient");
        return false;
    }

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt32() / 1000.0;
    }

    string string_test_mode = config.check("test", Value(string("use_pattern")), "string to select test mode").asString();
    if      (string_test_mode == "no_obstacles") { m_test_mode = NO_OBSTACLES; }
    else if (string_test_mode == "use_pattern") { m_test_mode = USE_PATTERN; }
    else if (string_test_mode == "use_mapfile") { m_test_mode = USE_MAPFILE; }
    else    { yError() << "invalid/unknown value for param 'test'"; return false; }

    min_distance = 0.1;  //m
    max_distance = 3.5;  //m
    min_angle = 0;       //degrees
    max_angle = 360;     //degrees
    resolution = 1.0;    //degrees

    if (config.check("clip_max")) { max_distance = config.find("clip_max").asFloat64(); }
    if (config.check("clip_min")) { min_distance = config.find("clip_min").asFloat64(); }
    if (config.check("max_angle")) { max_angle = config.find("max_angle").asFloat64(); }
    if (config.check("min_angle")) { min_angle = config.find("min_angle").asFloat64(); }
    if (config.check("resolution")) { resolution = config.find("resolution").asFloat64(); }
    if (max_angle - min_angle <= 0) { yError() << "invalid parameters max_angle/min_angle"; return false; }
    if (max_distance - min_distance <= 0) { yError() << "invalid parameters max_distance/min_distance"; return false; }
    if (resolution <= 0) { yError() << "invalid parameters resolution"; return false; }

    sensorsNum = (int)((max_angle-min_angle)/resolution);
    laser_data.resize(sensorsNum);
    if (m_test_mode == USE_MAPFILE)
    {
        string map_file;
        if (config.check("map_file"))
        {
            map_file = config.check("map_file",Value(string("map.yaml")),"map filename").asString();
        }
        else
        {
            yError() << "Missing map_file";
            return false;
        }
        bool ret = m_map.loadFromFile(map_file);
        if (ret == false)
        {
            yError() << "A problem occurred while opening:" << map_file;
            return false;
        }

        if (config.check("localization_port"))
        {
            string localization_port_name = config.check("localization_port", Value(string("/fakeLaser/location:i")), "name of localization input port").asString();
            m_loc_port = new yarp::os::BufferedPort<yarp::os::Bottle>;
            m_loc_port->open(localization_port_name);
            yInfo() << "Robot localization will be obtained from port" << localization_port_name;
            m_loc_mode = LOC_FROM_PORT;
        }
        else if (config.check("localization_client"))
        {
            Property loc_options;
            string localization_device_name = config.check("localization_client", Value(string("/fakeLaser/localizationClient")), "local name of localization client device").asString();
            loc_options.put("device", "localization2DClient");
            loc_options.put("local", localization_device_name);
            loc_options.put("remote", "/localizationServer");
            loc_options.put("period", 10);
            m_pLoc = new PolyDriver;
            if (m_pLoc->open(loc_options) == false)
            {
                yError() << "Unable to open localization driver";
                return false;
            }
            m_pLoc->view(m_iLoc);
            if (m_iLoc == nullptr)
            {
                yError() << "Unable to open localization interface";
                return false;
            }
            yInfo() << "Robot localization will be obtained from localization_client" << localization_device_name;
            m_loc_mode = LOC_FROM_CLIENT;
        }
        else
        {
            yInfo() << "No localization method selected. Robot location set to 0,0,0";
            m_loc_mode = LOC_NOT_SET;
        }

        m_loc_x=0;
        m_loc_y=0;
        m_loc_t=0;
        max_distance = 8;  //m
    }

    yInfo("Starting debug mode");
    yInfo("max_dist %f, min_dist %f", max_distance, min_distance);
    yInfo("max_angle %f, min_angle %f", max_angle, min_angle);
    yInfo("resolution %f", resolution);
    yInfo("sensors %d", sensorsNum);
    yInfo("test mode: %d", m_test_mode);
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

bool FakeLaser::getDistanceRange(double& min, double& max)
{
    mutex.lock();
    min = min_distance;
    max = max_distance;
    mutex.unlock();
    return true;
}

bool FakeLaser::setDistanceRange(double min, double max)
{
    mutex.lock();
    min_distance = min;
    max_distance = max;
    mutex.unlock();
    return true;
}

bool FakeLaser::getScanLimits(double& min, double& max)
{
    mutex.lock();
    min = min_angle;
    max = max_angle;
    mutex.unlock();
    return true;
}

bool FakeLaser::setScanLimits(double min, double max)
{
    mutex.lock();
    min_angle = min;
    max_angle = max;
    mutex.unlock();
    return true;
}

bool FakeLaser::getHorizontalResolution(double& step)
{
    mutex.lock();
    step = resolution;
    mutex.unlock();
    return true;
}

bool FakeLaser::setHorizontalResolution(double step)
{
    mutex.lock();
    resolution = step;
    mutex.unlock();
    return true;
}

bool FakeLaser::getScanRate(double& rate)
{
    mutex.lock();
    rate = 1.0 / (period);
    mutex.unlock();
    return true;
}

bool FakeLaser::setScanRate(double rate)
{
    mutex.lock();
    period = (1.0 / rate);
    mutex.unlock();
    return false;
}


bool FakeLaser::getRawData(yarp::sig::Vector &out)
{
    mutex.lock();
    out = laser_data;
    mutex.unlock();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}
bool FakeLaser::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    mutex.lock();
#ifdef LASER_DEBUG
    //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = laser_data.size();
    data.resize(size);
    if (max_angle < min_angle)
    {
        yError() << "getLaserMeasurement failed";
        mutex.unlock();
        return false;
    }
    double laser_angle_of_view = max_angle - min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + min_angle)* DEG2RAD;
        data[i].set_polar(laser_data[i], angle);
    }
    mutex.unlock();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool FakeLaser::getDeviceStatus(Device_status &status)
{
    mutex.lock();
    status = device_status;
    mutex.unlock();
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
    mutex.lock();
    laser_data.clear();
    double t      = yarp::os::Time::now();
    static double t_orig = yarp::os::Time::now();
    double size = (t - (t_orig));

    static int test_count = 0;
    static int test = 0;

    if (m_test_mode == USE_PATTERN)
    {
        for (int i = 0; i < sensorsNum; i++)
        {
            double value = 0;
            if (test == 0)
                { value = i / 100.0; }
            else if (test == 1)
                { value = size * 2; }
            else if (test == 2)
            {
                if (i >= 0 && i <= 10) { value = 1.0 + i / 20.0; }
                else if (i >= 90 && i <= 100) { value = 2.0 + (i - 90) / 20.0; }
                else { value = min_distance; }
            }

            if (value < min_distance) { value = min_distance; }
            if (value > max_distance) { value = max_distance; }
            laser_data.push_back(value);
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
        for (int i = 0; i < sensorsNum; i++)
        {
            laser_data.push_back(std::numeric_limits<double>::infinity());
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
                m_loc_x = b->get(0).asFloat64();
                m_loc_y = b->get(1).asFloat64();
                m_loc_t = b->get(2).asFloat64();
            }
        }
        else if (m_loc_mode == LOC_FROM_CLIENT)
        {
            Map2DLocation loc;
            if (m_iLoc->getCurrentPosition(loc))
            {
                m_loc_x = loc.x;
                m_loc_y = loc.y;
                m_loc_t = loc.theta;
            }
            else
            {
                yError() << "Error occurred while getting current position from localization server";
            }
        }
        else
        {
            yDebug() << "No localization mode selected. This branch should be not reachable.";
        }

        for (int i = 0; i < sensorsNum; i++)
        {
            //compute the ray in the robot reference frame
            double robot_curr_t = i*resolution + min_angle;
            double robot_curr_x = max_distance * cos(robot_curr_t*DEG2RAD);
            double robot_curr_y = max_distance * sin(robot_curr_t*DEG2RAD);

            //transforms the ray from the robot to the world reference frame
            XYWorld ray_world;
            ray_world.x = robot_curr_x*cos(m_loc_t*DEG2RAD) - robot_curr_y*sin(m_loc_t*DEG2RAD) + m_loc_x;
            ray_world.y = robot_curr_x*sin(m_loc_t*DEG2RAD) + robot_curr_y*cos(m_loc_t*DEG2RAD) + m_loc_y;
            XYCell src = m_map.world2Cell(XYWorld(m_loc_x, m_loc_y));
            XYCell dst = m_map.world2Cell(ray_world);
            double distance = checkStraightLine(src,dst);
            laser_data.push_back(distance + (*m_dis)(*m_gen));
        }
    }

    mutex.unlock();
    return;
}

double FakeLaser::checkStraightLine(XYCell src, XYCell dst)
{
    XYCell src_final = src;

    //here using the fast Bresenham algorithm
    int dx = abs(dst.x - src.x);
    int dy = abs(dst.y - src.y);
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
    yDebug("FakeLaser Thread releasing...");
    yDebug("... done.");
#endif
}

bool FakeLaser::getDeviceInfo(std::string &device_info)
{
    this->mutex.lock();
    device_info = info;
    this->mutex.unlock();
    return true;
}
