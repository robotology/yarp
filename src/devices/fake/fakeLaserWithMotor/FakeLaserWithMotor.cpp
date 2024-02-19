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

YARP_LOG_COMPONENT(FAKE_LASER, "yarp.devices.fakeLaserWithMotor")

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

bool FakeLaserWithMotor::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    m_info = "Fake Laser device for test/debugging";
    m_device_status = DEVICE_OK_STANDBY;

#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER) << "%s\n", config.toString().c_str());
#endif

    if (config.check("help"))
    {
        yCInfo(FAKE_LASER,"Some examples:");
        yCInfo(FAKE_LASER,"yarpdev --device fakeLaserWithMotor --help");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test no_obstacles");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_pattern");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_constant --const_distance 0.5");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_constant --const_distance 0.5 --SENSOR::resolution 0.5 --SKIP::min 0 50 --SKIP::max 10 60");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_server /localizationServer");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient --localization_server /localizationServer");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_context context --map_file mymap.map");
        yCInfo(FAKE_LASER,"yarpdev --device rangefinder2D_nws_yarp --subdevice fakeLaserWithMotor --period 0.01 --name /fakeLaser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient --localization_server /localization2D_nws_yarp --localization_device localization2D_nwc_yarp");
        return false;
    }

    std::string string_test_mode = m_test;
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

    if (m_test_mode == USE_SQUARE_TRAP)
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
        if (!m_MAP_MODE_map_context.empty() && !m_MAP_MODE_map_file.empty())
        {
            yarp::os::ResourceFinder rf;
            std::string tmp_filename = m_MAP_MODE_map_file;
            std::string tmp_contextname = m_MAP_MODE_map_context;
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
        else if (!m_MAP_MODE_map_file.empty())
        {
            map_file = m_MAP_MODE_map_file;
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
    dealloc();
    m_rpcPort.interrupt();
    m_rpcPort.close();

    return true;
}

bool FakeLaserWithMotor::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(FAKE_LASER)<<"thread initialising...\n");
    yCDebug(FAKE_LASER)<<"... done!\n");
#endif

    return true;
}

void FakeLaserWithMotor::run()
{
    for (size_t i=0; i<m_njoints; i++)
    {
        if (_controlModes[i] == VOCAB_CM_VELOCITY)
        {
            _encoders[i] = _encoders[i] + _command_speeds[i] * m_GENERAL_period;
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
