/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "laserFromPointCloud.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/math/Math.h>

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <mutex>


#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

using namespace yarp::sig;

YARP_LOG_COMPONENT(LASER_FROM_POINTCLOUD, "yarp.devices.laserFromPointCloud")

/*

yarpdev --device rangefinder2D_nws_yarp --subdevice laserFromPointCloud \
        --SENSOR \
        --RGBD_SENSOR_CLIENT::localImagePort    /clientRgbPort:i     \
        --RGBD_SENSOR_CLIENT::localDepthPort    /clientDepthPort:i   \
        --RGBD_SENSOR_CLIENT::localRpcPort      /clientRpcPort       \
        --RGBD_SENSOR_CLIENT::remoteImagePort   /SIM_CER_ROBOT/depthCamera/rgbImage:o \
        --RGBD_SENSOR_CLIENT::remoteDepthPort   /SIM_CER_ROBOT/depthCamera/depthImage:o \
        --RGBD_SENSOR_CLIENT::remoteRpcPort     /SIM_CER_ROBOT/depthCamera/rpc:i    \
        --TRANSFORM_CLIENT::local               /laserFromDepth/tfClient    \
        --TRANSFORM_CLIENT::remote              /transformServer    \
        --Z_CLIPPING_PLANES::floor_height       0.15 \
        --Z_CLIPPING_PLANES::ceiling_height     3.0  \
        --Z_CLIPPING_PLANES::camera_frame_id depth_center \
        --Z_CLIPPING_PLANES::ground_frame_id ground_link \
        --period 0.10 \
        --name /outlaser:o
*/

//-------------------------------------------------------------------------------------

bool LaserFromPointCloud::open(yarp::os::Searchable& config)
{
    Property subConfig;
    m_info = "LaserFromPointCloud device";

#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_POINTCLOUD) << "%s\n", config.toString().c_str();
#endif

    m_min_distance = 0.1; //m
    m_max_distance = 5.0;  //m
    m_floor_height = 0.1; //m
    m_pointcloud_max_distance = 10.0; //m
    m_ceiling_height = 2; //m

    m_sensorsNum = 360;
    m_resolution = 1;
    m_laser_data.resize(m_sensorsNum, 0.0);
    m_max_angle = 360;
    m_min_angle = 0;

    m_pc_stepx=2;
    m_pc_stepy=2;

    m_transform_mtrx.resize(4,4);
    m_transform_mtrx.eye();


    m_ground_frame_id = "/ground_frame";
    m_camera_frame_id = "/depth_camera_frame";


    bool bpcr = config.check("POINTCLOUD_QUALITY");
    if (bpcr != false)
    {
        yarp::os::Searchable& pointcloud_quality_config = config.findGroup("POINTCLOUD_QUALITY");
        if (pointcloud_quality_config.check("x_step")) { m_pc_stepx = pointcloud_quality_config.find("x_step").asFloat64(); }
        if (pointcloud_quality_config.check("y_step")) { m_pc_stepy = pointcloud_quality_config.find("y_step").asFloat64(); }
        yCInfo(LASER_FROM_POINTCLOUD) << "Pointcloud decimation step set to:" << m_pc_stepx << m_pc_stepy;
    }

    bool bpc = config.check("Z_CLIPPING_PLANES");
    if (bpc != false)
    {
        yarp::os::Searchable& pointcloud_clip_config = config.findGroup("Z_CLIPPING_PLANES");
        if (pointcloud_clip_config.check("floor_height"))   {m_floor_height = pointcloud_clip_config.find("floor_height").asFloat64();}
        if (pointcloud_clip_config.check("ceiling_height")) {m_ceiling_height=pointcloud_clip_config.find("ceiling_height").asFloat64();}
        if (pointcloud_clip_config.check("max_distance")) { m_pointcloud_max_distance = pointcloud_clip_config.find("max_distance").asFloat64(); }
        if (pointcloud_clip_config.check("ground_frame_id")) {m_ground_frame_id = pointcloud_clip_config.find("ground_frame_id").asString();}
        if (pointcloud_clip_config.check("camera_frame_id")) {m_camera_frame_id = pointcloud_clip_config.find("camera_frame_id").asString();}
    }
    yCInfo(LASER_FROM_POINTCLOUD) <<"Z clipping planes (floor,ceiling) have been set to ("<< m_floor_height <<","<< m_ceiling_height <<")";

    if (this->parseConfiguration(config) == false)
    {
        yCError(LASER_FROM_POINTCLOUD) << "Error parsing parameters";
        return false;
    }

    //open the tc client
    Property tcprop;
    if(!config.check("TRANSFORM_CLIENT"))
    {
        yCError(LASER_FROM_POINTCLOUD) << "missing TRANSFORM_CLIENT section in configuration file!";
        return false;
    }
    tcprop.fromString(config.findGroup("TRANSFORM_CLIENT").toString());
    tcprop.put("device", "transformClient");

    m_tc_driver.open(tcprop);
    if (!m_tc_driver.isValid())
    {
        yCError(LASER_FROM_POINTCLOUD) << "Error opening PolyDriver check parameters";
        return false;
    }
    m_tc_driver.view(m_iTc);
    if (!m_iTc)
    {
        yCError(LASER_FROM_POINTCLOUD) << "Error opening iFrameTransform interface. Device not available";
        return false;
    }
    yarp::os::Time::delay(0.1);


    //open the rgbd client
    Property prop;
    if(!config.check("RGBD_SENSOR_CLIENT"))
    {
        yCError(LASER_FROM_POINTCLOUD) << "missing RGBD_SENSOR_CLIENT section in configuration file!";
        return false;
    }
    prop.fromString(config.findGroup("RGBD_SENSOR_CLIENT").toString());
    prop.put("device", "RGBDSensorClient");
    //prop.put("ImageCarrier","mjpeg"); //this is set in the ini file
    //prop.put("DepthCarrier","udp"); //this is set in the ini file
    m_rgbd_driver.open(prop);
    if (!m_rgbd_driver.isValid())
    {
        yCError(LASER_FROM_POINTCLOUD) << "Error opening PolyDriver check parameters";
        return false;
    }
    m_rgbd_driver.view(m_iRGBD);
    if (!m_iRGBD)
    {
        yCError(LASER_FROM_POINTCLOUD) << "Error opening iRGBD interface. Device not available";
        return false;
    }
    yarp::os::Time::delay(0.1);

    //get parameters data from the camera
    m_depth_width = m_iRGBD->getRgbWidth();  //@@@ this is horrible! See yarp issue: https://github.com/robotology/yarp/issues/2290
    m_depth_height = m_iRGBD->getRgbHeight(); //@@@ this is horrible! See yarp issue: https://github.com/robotology/yarp/issues/2290
    bool propintr  = m_iRGBD->getDepthIntrinsicParam(m_propIntrinsics);
    YARP_UNUSED(propintr);
    yCInfo(LASER_FROM_POINTCLOUD) << "Depth Intrinsics:" << m_propIntrinsics.toString();
    m_intrinsics.fromProperty(m_propIntrinsics);

    PeriodicThread::start();
    yCInfo(LASER_FROM_POINTCLOUD) << "Sensor ready";

    return true;
}

bool LaserFromPointCloud::close()
{
    PeriodicThread::stop();

    if (m_rgbd_driver.isValid()) {
        m_rgbd_driver.close();
    }

    if (m_tc_driver.isValid()) {
        m_tc_driver.close();
    }

    yCInfo(LASER_FROM_POINTCLOUD) << "closed";
    return true;
}

ReturnValue LaserFromPointCloud::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_min_distance = min;
    m_max_distance = max;
    return ReturnValue_ok;
}


ReturnValue LaserFromPointCloud::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_POINTCLOUD,"setScanLimits not yet implemented");
    return ReturnValue_ok;
}

ReturnValue LaserFromPointCloud::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_POINTCLOUD,"setHorizontalResolution not yet implemented");
    return ReturnValue_ok;
}

ReturnValue LaserFromPointCloud::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    yCWarning(LASER_FROM_POINTCLOUD,"setScanRate not yet implemented");
    return ReturnValue::return_code::return_value_error_not_implemented_by_device;
}


bool LaserFromPointCloud::threadInit()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_POINTCLOUD) << "thread initialising...\n";
    yCDebug(LASER_FROM_POINTCLOUD) << "... done!\n");
#endif

    return true;
}

void rotate_pc (yarp::sig::PointCloud<yarp::sig::DataXYZ>& pc, const yarp::sig::Matrix& m)
{
    for (size_t i=0; i<pc.size(); i++)
    {
        auto v1 = pc(i).toVector4();
        auto v2 = m*v1;
        pc(i).x=v2(0);
        pc(i).y=v2(1);
        pc(i).z=v2(2);
    }
}

bool LaserFromPointCloud::acquireDataFromHW()
{
#ifdef DEBUG_TIMING
    static double t3 = yarp::os::Time::now();
    double t1 = yarp::os::Time::now();
    yCDebug(LASER_FROM_POINTCLOUD) << "thread period:" << t1 - t3;
    t3 = yarp::os::Time::now();
#endif

    bool depth_ok = m_iRGBD->getDepthImage(m_depth_image);
    if (depth_ok == false)
    {
        yCError(LASER_FROM_POINTCLOUD) << "getDepthImage failed";
        return false;
    }

    if (m_depth_image.getRawImage() == nullptr)
    {
        yCError(LASER_FROM_POINTCLOUD) << "invalid image received";
        return false;
    }

    if (m_depth_image.width() != m_depth_width ||
        m_depth_image.height() != m_depth_height)
    {
        yCError(LASER_FROM_POINTCLOUD) << "invalid image size: (" << m_depth_image.width() << " " << m_depth_image.height() << ") vs (" << m_depth_width << " " << m_depth_height << ")";
        return false;
    }

    const double myinf = std::numeric_limits<double>::infinity();
    const double mynan = std::nan("");

    //compute the point cloud
    yarp::sig::PointCloud<yarp::sig::DataXYZ> pc = yarp::sig::utils::depthToPC(m_depth_image, m_intrinsics, m_pc_roi, m_pc_stepx, m_pc_stepy);


#ifdef TEST_M
    //yCDebug(LASER_FROM_POINTCLOUD) << "pc size:" << pc.size();
#endif

    //we compute the transformation matrix from the camera to the laser reference frame

#ifdef TEST_M
    yarp::sig::Vector vvv(3);
    vvv(0) = -1.57;
    vvv(1) = 0;
    vvv(2) = -1.57;
    m = yarp::math::rpy2dcm(vvv);
    m(2, 3) = 1.2; //z translation
#else
    bool frame_exists = m_iTc->getTransform(m_camera_frame_id, m_ground_frame_id, m_transform_mtrx);
    if (frame_exists == false)
    {
        yCWarning(LASER_FROM_POINTCLOUD) << "Unable to found m matrix";
    }
#endif

    //we rototranslate the full pointcloud
    rotate_pc(pc, m_transform_mtrx);

    yarp::sig::Vector left(4);
    left[0] = (0 - m_intrinsics.principalPointX) / m_intrinsics.focalLengthX * 1000;
    left[1] = (0 - m_intrinsics.principalPointY) / m_intrinsics.focalLengthY * 1000;
    left[2] = 1000;
    left[3] = 1;

    yarp::sig::Vector right(4);
    right[0] = (m_depth_image.width() - m_intrinsics.principalPointX) / m_intrinsics.focalLengthX * 1000;
    right[1] = (0 - m_intrinsics.principalPointY) / m_intrinsics.focalLengthY * 1000;
    right[2] = 1000;
    right[3] = 1;

    double left_dist;
    double left_theta;
    double right_dist;
    double right_theta;
    left = m_transform_mtrx * left;
    right = m_transform_mtrx * right;

    LaserMeasurementData data_left;
    data_left.set_cartesian(left[0], left[1]);
    data_left.get_polar(left_dist, left_theta);

    LaserMeasurementData data_right;
    data_right.set_cartesian(right[0], right[1]);
    data_right.get_polar(right_dist, right_theta);

    bool left_elem_neg = 0;
    bool right_elem_neg = 0;

    left_theta = left_theta * 180 / M_PI;
    right_theta = right_theta * 180 / M_PI;

    if (left_theta < 0)
    {
        left_theta += 360;
        left_elem_neg = 1;
    } else if (left_theta > 360) {
        left_theta -= 360;
    }
    size_t left_elem = left_theta / m_resolution;

    if (right_theta < 0)
    {
        right_theta += 360;
        right_elem_neg = 1;
    } else if (right_theta > 360) {
        right_theta -= 360;
    }
    size_t right_elem = right_theta / m_resolution;

    //enter critical section and protect m_laser_data
    std::lock_guard<std::mutex> guard(m_mutex);
#ifdef DEBUG_TIMING
    double t4 = yarp::os::Time::now();
#endif
    //prepare an empty laserscan vector with the resolution we want
    for (auto it = m_laser_data.begin(); it != m_laser_data.end(); it++)
    {
        *it = mynan;
    }

    if ((!left_elem_neg) && (right_elem_neg))
    {
        for (size_t i = 0; i < left_elem; i++)
        {
            m_laser_data[i] = myinf;
        }
        for (size_t i = right_elem; i < m_sensorsNum; i++)
        {
            m_laser_data[i] = myinf;
        }
    }
    else
    {
        for (size_t i = right_elem; i < left_elem; i++)
        {
            m_laser_data[i] = myinf;
        }
    }


    for (size_t i = 0; i < pc.size(); i++)
    {

#ifdef TEST_M
        //yCDebug(LASER_FROM_POINTCLOUD) << pc(i).toString(5,5);
#endif

        //we obtain a point from the point cloud
        yarp::sig::Vector vec = pc(i).toVector4();

        //we check if the point is in the volume that we want to consider as possibile obstacle
        if (vec[2] > m_floor_height&& vec[2] < m_ceiling_height&&
            vec[0] < m_pointcloud_max_distance)
        {
#ifdef TEST_M
            //        yCDebug(LASER_FROM_POINTCLOUD) << "This point is ok:" << i <<"its z is:" << tvec[2];
#endif
            //by removing z, we project the 3d point on the 2D plane on which the laser works.
            //we use LaserMeasurementData struct to easily obtain a polar representation from a cartesian representation
            LaserMeasurementData data;
            data.set_cartesian(vec[0], vec[1]);

            //get the polar representation
            double distance;
            double theta;
            data.get_polar(distance, theta);

            //compute the right element of the vector where to put distance data. This is done by clusterizing angles, depending on the laser resolution.
            theta = theta * 180 / M_PI;
            if (theta < 0) {
                theta += 360;
            } else if (theta > 360) {
                theta -= 360;
            }
            size_t elem = theta / m_resolution;
            if (elem >= m_laser_data.size())
            {
                yCError(LASER_FROM_POINTCLOUD) << "Error in computing elem" << i << ">" << m_laser_data.size();
                continue;
            }

#ifdef TEST_M
            // yCDebug(LASER_FROM_POINTCLOUD) <<theta << elem <<distance;
#endif
            //update the vector of measurements, putting the NEAREST obstacle in right element of the vector.
            if (distance < m_laser_data[elem])
            {
                m_laser_data[elem] = distance;
            }
        }
        else
        {
#ifdef TEST_M
            //yCDebug(LASER_FROM_POINTCLOUD) << "this point is out of considered volume:" <<i << " z:" << vec[2];
#endif
        }
    }


#ifdef DEBUG_TIMING
    double t2 = yarp::os::Time::now();
    yCDebug(LASER_FROM_POINTCLOUD) << "tot run:" << t2 - t1 << "crit run:" << t2 - t4;
#endif

    return true;
}

void LaserFromPointCloud::run()
{
    m_mutex.lock();
    updateLidarData();
    m_mutex.unlock();
}

void LaserFromPointCloud::threadRelease()
{
#ifdef LASER_DEBUG
    yCDebug(LASER_FROM_POINTCLOUD) << "Thread releasing...";
    yCDebug(LASER_FROM_POINTCLOUD) << "... done.";
#endif

    return;
}
