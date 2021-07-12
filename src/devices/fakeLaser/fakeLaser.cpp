/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include "fakeLaser.h"

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
        yCInfo(FAKE_LASER,"yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_context context --map_file mymap.map");
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
            m_const_value = config.check("const_distance", Value(1.0), "default constant distance").asFloat64();
        }
    }
    else if (m_test_mode == USE_MAPFILE)
    {
        string map_file;
        if (config.check("map_context") && config.check("map_file"))
        {
            yarp::os::ResourceFinder rf;
            string tmp_filename = config.find("map_file").asString();
            string tmp_contextname = config.find("map_context").asString();
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
            map_file = config.check("map_file", Value(string("map.yaml")), "map filename").asString();
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

    if (!m_rpcPort.open("/fakeLaser/rpc:i"))
    {
        yCError(FAKE_LASER, "Failed to open port %s", "/fakeLaser/rpc:i");
        return false;
    }
    m_rpcPort.setReader(*this);

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


bool FakeLaser::LiangBarsky_clip(int edgeLeft, int edgeRight, int edgeTop, int edgeBottom,
                                 XYCell_unbounded src, XYCell_unbounded dst,
                                 XYCell& src_clipped, XYCell& dst_clipped)
{
    double t0 = 0.0;    double t1 = 1.0;
    double xdelta = double(dst.x - src.x);
    double ydelta = double(dst.y - src.y);
    double p, q, r;

    for (int edge = 0; edge < 4; edge++)
    {
        if (edge == 0) { p = -xdelta;    q = -(edgeLeft - src.x); }
        if (edge == 1) { p = xdelta;     q = (edgeRight - src.x); }
        if (edge == 2) { p = -ydelta;    q = -(edgeTop - src.y); }
        if (edge == 3) { p = ydelta;     q = (edgeBottom - src.y); }
        r = q / p;
        if (p == 0 && q < 0) {return false;}   //line is outside (parallel)

        if (p < 0)
        {
            if (r > t1) {return false;}            //line is outside.
            else if (r > t0) {t0 = r;}             //line is clipped
        }
        else if (p > 0)
        {
            if (r < t0) {return false;}        //line is outside.
            else if (r < t1) {t1 = r;}         //line is clipped
        }
    }

    src_clipped.x = size_t(double(src.x) + t0 * xdelta);
    src_clipped.y = size_t(double(src.y) + t0 * ydelta);
    dst_clipped.x = size_t(double(src.x) + t1 * xdelta);
    dst_clipped.y = size_t(double(src.y) + t1 * ydelta);

    return true;        //line is clipped
}

bool FakeLaser::acquireDataFromHW()
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

void FakeLaser::run()
{
    m_mutex.lock();
    updateLidarData();
    m_mutex.unlock();
    return;
}

void FakeLaser::wall_the_robot(double siz, double dist)
{
    //double res;
    //m_map.getResolution(res);
    //size_t siz_cell = siz / res;
    //size_t dist_cell = dist / res;
    XYCell robot = m_map.world2Cell(XYWorld(m_robot_loc_x, m_robot_loc_y));

    XYWorld ray_start;
    XYWorld start (0+dist, 0 - siz);
    ray_start.x = start.x * cos(m_robot_loc_t * DEG2RAD) - start.y * sin(m_robot_loc_t * DEG2RAD) + m_robot_loc_x;
    ray_start.y = start.x * sin(m_robot_loc_t * DEG2RAD) + start.y * cos(m_robot_loc_t * DEG2RAD) + m_robot_loc_y;
    XYCell start_cell = m_map.world2Cell(ray_start);

    XYWorld ray_end;
    XYWorld end(0 + dist, 0 + siz);
    ray_end.x = end.x * cos(m_robot_loc_t * DEG2RAD) - end.y * sin(m_robot_loc_t * DEG2RAD) + m_robot_loc_x;
    ray_end.y = end.x * sin(m_robot_loc_t * DEG2RAD) + end.y * cos(m_robot_loc_t * DEG2RAD) + m_robot_loc_y;
    XYCell end_cell = m_map.world2Cell(ray_end);

    drawStraightLine(start_cell,end_cell);
}

void FakeLaser::obst_the_robot(double siz, double dist)
{
    //NOT YET IMPLEMENTED
    /*double res;
    m_map.getResolution(res);
    size_t siz_cell = size_t(siz / res);
    size_t dist_cell = size_t(dist / res);
    XYCell robot = m_map.world2Cell(XYWorld(m_robot_loc_x, m_robot_loc_y));*/
}

void FakeLaser::trap_the_robot(double siz)
{
    double res;
    m_map.getResolution(res);
    size_t siz_cell = size_t(siz / res);
    XYCell robot  = m_map.world2Cell(XYWorld (m_robot_loc_x, m_robot_loc_y));
    for (size_t x= robot.x- siz_cell; x< robot.x + siz_cell; x++)
    {
        size_t y=robot.y- siz_cell;
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }
    for (size_t x = robot.x - siz_cell; x < robot.x + siz_cell; x++)
    {
        size_t y = robot.y + siz_cell;
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }
    for (size_t y = robot.y - siz_cell; y < robot.y + siz_cell; y++)
    {
        size_t x = robot.x - siz_cell;
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }
    for (size_t y = robot.y - siz_cell; y < robot.y + siz_cell; y++)
    {
        size_t x = robot.x + siz_cell;
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }
}

void FakeLaser::free_the_robot()
{
    m_map=m_originally_loaded_map;
}

bool FakeLaser::read(yarp::os::ConnectionReader& connection)
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


void FakeLaser::drawStraightLine(XYCell src, XYCell dst)
{
    long int x, y;
    long int dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = (long int)dst.x - (long int)src.x;
    dy = (long int)dst.y - (long int)src.y;
    dx1 = abs(dx);
    dy1 = abs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = (long int)src.x;
            y = (long int)src.y;
            xe = (long int)dst.x;
        }
        else
        {
            x = (long int)dst.x;
            y = (long int)dst.y;
            xe = (long int)src.x;
        }
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
            {
                px = px + 2 * dy1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    y = y + 1;
                }
                else
                {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = (long int)src.x;
            y = (long int)src.y;
            ye = (long int)dst.y;
        }
        else
        {
            x = (long int)dst.x;
            y = (long int)dst.y;
            ye = (long int)src.y;
        }
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
            {
                py = py + 2 * dx1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x = x + 1;
                }
                else
                {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
        }
    }
}

double FakeLaser::checkStraightLine(XYCell src, XYCell dst)
{
    //BEWARE: src and dest must be already clipped and >0 in this function
    XYCell test_point;
    test_point.x = src.x;
    test_point.y = src.y;

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
        //the test point is going to move from src until it reaches dst OR
        //if it reaches the boundaries of the image
        if (test_point.x==0 || test_point.y ==0 || test_point.x>=m_map.width() || test_point.y>=m_map.height())
        {
            break;
        }

        //if (m_map.isFree(src) == false)
        if (m_map.isWall(XYCell(test_point.x,test_point.y)))
        {
            XYWorld world_start =  m_map.cell2World(src);
            XYWorld world_end =  m_map.cell2World(XYCell(test_point.x, test_point.y));
            double dist = sqrt(pow(world_start.x - world_end.x, 2) + pow(world_start.y - world_end.y, 2));
            return dist;
        }

        if (test_point.x == dst.x && test_point.y == dst.y)
        {
            break;
        }

        int e2 = err * 2;
        if (e2 > -dy)
        {
            err = err - dy;
            test_point.x += sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            test_point.y += sy;
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
