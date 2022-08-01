/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include "fakeLaserWithMotor.h"

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

YARP_LOG_COMPONENT(FAKE_LASER, "yarp.devices.fakeLaserWithMotor")

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

// see FakeLaser.h for device documentation

bool FakeLaserWithMotor::open(yarp::os::Searchable& config)
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
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test no_obstacles");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_pattern");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_constant --const_distance 0.5");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_constant --const_distance 0.5 --SENSOR::resolution 0.5 --SKIP::min 0 50 --SKIP::max 10 60");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_mapfile --map_file mymap.map");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_server /localizationServer");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient --localization_server /localizationServer");
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper   --subdevice fakeLaser --period 10   --name /fakeLaser:o --test use_mapfile --map_context context --map_file mymap.map");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaser --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient --localization_server /localization2D_nws_yarp --localization_device localization2D_nwc_yarp");
        return false;
    }

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        m_period = general_config.check("period", Value(0.02), "Period of the sampling thread in s").asFloat64();
        this->setPeriod(m_period);
    }

    std::string string_test_mode = config.check("test", Value(std::string("use_pattern")), "string to select test mode").asString();
    if      (string_test_mode == "no_obstacles") { m_test_mode = NO_OBSTACLES; }
    else if (string_test_mode == "use_pattern") { m_test_mode = USE_PATTERN; }
    else if (string_test_mode == "use_mapfile") { m_test_mode = USE_MAPFILE; }
    else if (string_test_mode == "use_constant") { m_test_mode = USE_CONSTANT_VALUE; }
    else if (string_test_mode == "use_square_trap") { m_test_mode = USE_SQUARE_TRAP; }
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
            m_const_value = config.check("const_distance", Value(1.0), "default constant distance").asFloat64();
        }
    }
    else if (m_test_mode == USE_SQUARE_TRAP)
    {
        m_robot_loc_x = 0;
        m_robot_loc_y = 0;
        m_robot_loc_t = 0;
        m_map.m_map_name = "test_map_10x10m";
        m_map.m_resolution = 0.01; //m/pixel
        m_map.m_origin.setOrigin(-5,-5,0); //m
        m_map.setSize_in_meters(10,10);
        for (size_t y = 0; y < m_map.m_height; y++)
        {
            for (size_t x = 0; x < m_map.m_width; x++)
            {
                m_map.setOccupancyData(yarp::dev::Nav2D::XYCell(x, y),0);
                m_map.setMapFlag(yarp::dev::Nav2D::XYCell(x, y), MapGrid2D::map_flags::MAP_CELL_FREE);
            }
        }
        m_originally_loaded_map = m_map;
        trap_the_robot(3); //3m
        MapGrid2D::map_flags vv;
        m_map.saveToFile("mio1");
    }
    else if (m_test_mode == USE_MAPFILE)
    {
        std::string map_file;
        if (config.check("map_context") && config.check("map_file"))
        {
            yarp::os::ResourceFinder rf;
            std::string tmp_filename = config.find("map_file").asString();
            std::string tmp_contextname = config.find("map_context").asString();
            rf.setDefaultContext(tmp_contextname);
            rf.setDefaultConfigFile(tmp_filename);
            bool b = rf.configure(0, nullptr);
            if (b)
            {
                map_file = rf.findFile(tmp_filename);
                if (map_file == "")
                {
                    yCWarning(FAKE_LASER, "Unable to find file: %s from context: %s\n", tmp_filename.c_str(), tmp_contextname.c_str());
                }
            }
            else
            {
                yCWarning(FAKE_LASER, "Unable to find file: %s from context: %s\n", tmp_filename.c_str(), tmp_contextname.c_str());
            }
        }
        else if (config.check("map_file"))
        {
            map_file = config.check("map_file", Value(std::string("map.yaml")), "map filename").asString();
        }
        else
        {
            yCError(FAKE_LASER) << "Missing `map_file` or `map_context`+`map_file` parameters";
            return false;
        }

        if (map_file=="")
        {
            yCError(FAKE_LASER) << "File not found";
            return false;
        }
        bool ret = m_originally_loaded_map.loadFromFile(map_file);
        if (ret == false)
        {
            yCError(FAKE_LASER) << "A problem occurred while opening:" << map_file;
            return false;
        }
        m_map = m_originally_loaded_map;
        m_robot_loc_x=0;
        m_robot_loc_y=0;
        m_robot_loc_t=0;
    }

    yCInfo(FAKE_LASER) << "Starting debug mode";
    yCInfo(FAKE_LASER) << "test mode:"<< m_test_mode << " i.e. " << string_test_mode;

    //  INIT ALL INTERFACES
    std::vector<double> tmpZeros; tmpZeros.resize(m_njoints, 0.0);
    std::vector<double> tmpOnes;  tmpOnes.resize(m_njoints, 1.0);
    std::vector<int> axisMap;  axisMap.resize(m_njoints, 1.0);
    axisMap[0] = 0;
    axisMap[1] = 1;
    axisMap[2] = 2;
    ImplementEncodersTimed::initialize(m_njoints, axisMap.data(), tmpOnes.data(), nullptr);
    ImplementPositionControl::initialize(m_njoints, axisMap.data(), tmpOnes.data(), nullptr);
    ImplementControlMode::initialize(m_njoints, axisMap.data());
    ImplementVelocityControl::initialize(m_njoints, axisMap.data(), tmpOnes.data(), nullptr);
    ImplementInteractionMode::initialize(m_njoints, axisMap.data(), tmpOnes.data(), nullptr);
    ImplementAxisInfo::initialize(m_njoints, axisMap.data());
    if (!alloc(m_njoints))
    {
        yCError(FAKE_LASER) << "Malloc failed";
        return false;
    }
    _jointType[0] = VOCAB_JOINTTYPE_PRISMATIC;
    _jointType[1] = VOCAB_JOINTTYPE_PRISMATIC;
    _jointType[2] = VOCAB_JOINTTYPE_REVOLUTE;
    _axisName[0] = "joint_x";
    _axisName[1] = "joint_y";
    _axisName[2] = "joint_t";
    _controlModes[0] = VOCAB_CM_IDLE;
    _controlModes[1] = VOCAB_CM_IDLE;
    _controlModes[2] = VOCAB_CM_IDLE;

    if (!m_rpcPort.open("/fakeLaser/rpc:i"))
    {
        yCError(FAKE_LASER, "Failed to open port %s", "/fakeLaser/rpc:i");
        return false;
    }
    m_rpcPort.setReader(*this);

    return PeriodicThread::start();
}

bool FakeLaserWithMotor::close()
{
    PeriodicThread::stop();

    driver.close();

    return true;
}

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
    m_period = (1.0 / rate);
    m_mutex.unlock();
    return false;
}

bool FakeLaserWithMotor::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER)<<"thread initialising...\n");
    yCDebug(FAKE_LASER)<<"... done!\n");
#endif

    return true;
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

void FakeLaserWithMotor::run()
{
    for (size_t i=0; i<m_njoints; i++)
    {
        if (_controlModes[i] == VOCAB_CM_VELOCITY)
        {
            _encoders[i] = _encoders[i] + _command_speeds[i] * m_period;
        }
        else if (_controlModes[i] == VOCAB_CM_POSITION)
        {
         /*   if (a>b)
            {
                _posCtrl_references[i] = _posCtrl_references[i] + _command_speeds[i] / m_period * 1000;
                if (b > a) _posCtrl_references[i] =a;
            }
            else if (b<a)
            {
                _posCtrl_references[i] = _posCtrl_references[i] + _command_speeds[i] / m_period * 1000;
                if (b > a) _posCtrl_references[i] = a;
            }
*/
        }
    }
    m_mutex.lock();
    updateLidarData();
    m_mutex.unlock();
    return;
}

bool FakeLaserWithMotor::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).asString() == "trap")
    {
        if (command.size() == 1)
        {
            trap_the_robot();
            reply.addVocab32(VOCAB_OK);
        }
        else if (command.size() == 2)
        {
            trap_the_robot(command.get(1).asFloat64());
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "wall")
    {
        if (command.size() == 1)
        {
            wall_the_robot(1.0, 1.0);
            wall_the_robot(1.0, 1.05);
            reply.addVocab32(VOCAB_OK);
        }
        else if (command.size() == 2)
        {
            wall_the_robot(command.get(1).asFloat64(), 1.0);
            wall_the_robot(command.get(1).asFloat64(), 1.05);
            reply.addVocab32(VOCAB_OK);
        }
        else if (command.size() == 3)
        {
            wall_the_robot(command.get(1).asFloat64(), command.get(2).asFloat64());
            wall_the_robot(command.get(1).asFloat64(), command.get(2).asFloat64()+0.05);
            reply.addVocab32(VOCAB_OK);
        }
        else
        {
            reply.addVocab32(VOCAB_ERR);
        }
    }
    else if (command.get(0).asString() == "free")
    {
        free_the_robot();
        reply.addVocab32(VOCAB_OK);
    }
    else if (command.get(0).asString() == "help")
    {
        reply.addVocab32("many");
        reply.addString("wall <size> <distance>: creates a wall in front of the robot");
        reply.addString("trap <size>: traps the robot in a box obstacle");
        reply.addString("free: removes all generated obstacles");
    }
    else
    {
        yCError(FAKE_LASER) << "Invalid command";
        reply.addVocab32(VOCAB_ERR);
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }
    return true;
}

void FakeLaserWithMotor::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER) << "FakeLaser Thread releasing...");
    yCDebug(FAKE_LASER) << "... done.");
#endif
}
