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

YARP_LOG_COMPONENT(FAKE_LASER_UTILS, "yarp.devices.fakeLaserWithMotor.Utils")

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::dev::Nav2D;

void FakeLaserWithMotor::wall_the_robot(double siz, double dist)
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

void FakeLaserWithMotor::obst_the_robot(double siz, double dist)
{
    //NOT YET IMPLEMENTED
    /*double res;
    m_map.getResolution(res);
    size_t siz_cell = size_t(siz / res);
    size_t dist_cell = size_t(dist / res);
    XYCell robot = m_map.world2Cell(XYWorld(m_robot_loc_x, m_robot_loc_y));*/
}

void FakeLaserWithMotor::trap_the_robot(double siz)
{
    double res;
    m_map.getResolution(res);
    size_t siz_cell = size_t(siz / res);
    size_t x=0;
    size_t y=0;
    XYCell robot  = m_map.world2Cell(XYWorld (m_robot_loc_x, m_robot_loc_y));

    y = robot.y - siz_cell;
    for (x= robot.x- siz_cell; x< robot.x + siz_cell; x++)
    {
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }

    y = robot.y + siz_cell;
    for (x = robot.x - siz_cell; x < robot.x + siz_cell; x++)
    {
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }

    x = robot.x - siz_cell;
    for (y = robot.y - siz_cell; y < robot.y + siz_cell; y++)
    {
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }

    x = robot.x + siz_cell;
    for (y = robot.y - siz_cell; y < robot.y + siz_cell; y++)
    {
        m_map.setMapFlag(XYCell(x, y), yarp::dev::Nav2D::MapGrid2D::map_flags::MAP_CELL_WALL);
    }
}

void FakeLaserWithMotor::free_the_robot()
{
    m_map=m_originally_loaded_map;
}

void FakeLaserWithMotor::drawStraightLine(XYCell src, XYCell dst)
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

double FakeLaserWithMotor::checkStraightLine(XYCell src, XYCell dst)
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

bool FakeLaserWithMotor::LiangBarsky_clip(int edgeLeft, int edgeRight, int edgeTop, int edgeBottom,
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
        if (p == 0 && q < 0) { return false; }   //line is outside (parallel)

        if (p < 0)
        {
            if (r > t1) { return false; }            //line is outside.
            else if (r > t0) { t0 = r; }             //line is clipped
        }
        else if (p > 0)
        {
            if (r < t0) { return false; }        //line is outside.
            else if (r < t1) { t1 = r; }         //line is clipped
        }
    }

    src_clipped.x = size_t(double(src.x) + t0 * xdelta);
    src_clipped.y = size_t(double(src.y) + t0 * ydelta);
    dst_clipped.x = size_t(double(src.x) + t1 * xdelta);
    dst_clipped.y = size_t(double(src.y) + t1 * ydelta);

    return true;        //line is clipped
}
