/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include "FakeLaserWithMotor.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
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

YARP_LOG_COMPONENT(FAKE_LASER_LASER, "yarp.devices.fakeLaserWithMotor")

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

bool FakeLaserWithMotor::setDistanceRange(double min, double max)
{
    m_mutex.lock();
    m_min_distance = min;
    m_max_distance = max;
    m_mutex.unlock();
    return true;
}

bool FakeLaserWithMotor::setScanLimits(double min, double max)
{
    m_mutex.lock();
    m_min_angle = min;
    m_max_angle = max;
    m_mutex.unlock();
    return true;
}

bool FakeLaserWithMotor::setHorizontalResolution(double step)
{
    m_mutex.lock();
    m_resolution = step;
    m_mutex.unlock();
    return true;
}

bool FakeLaserWithMotor::setScanRate(double rate)
{
    m_mutex.lock();
    m_GENERAL_period = (1.0 / rate);
    m_mutex.unlock();
    return false;
}

bool FakeLaserWithMotor::acquireDataFromHW()
{
    m_laser_data.clear();
    double t = yarp::os::Time::now();
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
            {
                value = i / 100.0;
            }
            else if (test == 1)
            {
                value = size * 2;
            }
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
    else if (m_test_mode == USE_MAPFILE ||
        m_test_mode == USE_SQUARE_TRAP)
    {
        m_robot_loc_x = _encoders[0];//loc.x;
        m_robot_loc_y = _encoders[1];//loc.y;
        m_robot_loc_t = _encoders[2];//loc.theta;

        for (size_t i = 0; i < m_sensorsNum; i++)
        {
            //compute the ray in the robot reference frame
            double ray_angle = i * m_resolution + m_min_angle;
            double ray_target_x = m_max_distance * cos(ray_angle * DEG2RAD);
            double ray_target_y = m_max_distance * sin(ray_angle * DEG2RAD);

            //transforms the ray from the robot to the world reference frame
            XYWorld ray_world;
            ray_world.x = ray_target_x * cos(m_robot_loc_t * DEG2RAD) - ray_target_y * sin(m_robot_loc_t * DEG2RAD) + m_robot_loc_x;
            ray_world.y = ray_target_x * sin(m_robot_loc_t * DEG2RAD) + ray_target_y * cos(m_robot_loc_t * DEG2RAD) + m_robot_loc_y;
            XYCell src = m_map.world2Cell(XYWorld(m_robot_loc_x, m_robot_loc_y));

            //beware! src is the robot position and it is always inside the image.
            //dst is the end of the ray, and it can be out of the image!
            //for this reason i am not going to call world2Cell() on dst, because that functions clips the point to the border without
            //knowing the angular coefficient of the ray. I thus need the unclipped value, and run the LiangBarsky algorithm
            //(which knows the angular coefficient of the ray) on it.
            XYWorld ray_world_rot;
            XYCell_unbounded dst;
            ray_world_rot.x = ray_world.x * m_map.m_origin.get_ca() - ray_world.y * m_map.m_origin.get_sa();
            ray_world_rot.y = ray_world.x * m_map.m_origin.get_sa() + ray_world.y * m_map.m_origin.get_ca();
            dst.x = int((+ray_world_rot.x - this->m_map.m_origin.get_x()) / this->m_map.m_resolution) + 0;
            dst.y = int((-ray_world_rot.y + this->m_map.m_origin.get_y()) / this->m_map.m_resolution) + (int)m_map.m_height - 1;

            XYCell newsrc;
            XYCell newdst;
            double distance;
            if (LiangBarsky_clip(0, (int)m_map.width(), 0, (int)m_map.height(),
                XYCell_unbounded(src.x, src.y), dst, newsrc, newdst))
            {
                distance = checkStraightLine(src, newdst);
                double simulated_noise = (*m_dis)(*m_gen);
                m_laser_data.push_back(distance + simulated_noise);
            }
            else
            {
                //This should never happen, unless the robot is outside the map!
                yDebug() << "Robot is outside the map?!";
                m_laser_data.push_back(std::numeric_limits<double>::infinity());
            }

        }
    }

    //set the device status
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;

    return true;
}
