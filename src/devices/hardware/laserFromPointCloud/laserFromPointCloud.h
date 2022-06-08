/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef LASER_FROM_POINTCLOUDS_H
#define LASER_FROM_POINTCLOUDS_H

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/IntrinsicParams.h>
#include <yarp/sig/PointCloud.h>
#include <yarp/sig/PointCloudUtils.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/Lidar2DDeviceBase.h>
#include <yarp/dev/IFrameTransform.h>

#include <mutex>
#include <string>
#include <vector>

using namespace yarp::os;
using namespace yarp::dev;

typedef unsigned char byte;

//---------------------------------------------------------------------------------------------------------------
/**
 * @ingroup dev_impl_lidar
 *
 * \brief `laserFromPointCloud`: Documentation to be added
 */
class LaserFromPointCloud : public PeriodicThread, public yarp::dev::Lidar2DDeviceBase, public DeviceDriver
{
protected:
    PolyDriver m_rgbd_driver;
    IRGBDSensor* m_iRGBD = nullptr;

    PolyDriver m_tc_driver;
    IFrameTransform* m_iTc = nullptr;

    size_t m_depth_width = 0;
    size_t m_depth_height = 0;
    yarp::os::Property m_propIntrinsics;
    yarp::sig::IntrinsicParams m_intrinsics;
    yarp::sig::ImageOf<float> m_depth_image;

    //point cloud
    size_t m_pc_stepx = 0;
    size_t m_pc_stepy = 0;
    yarp::sig::utils::PCL_ROI m_pc_roi;

    //frames and point cloud clipping planes
    bool   m_publish_ros_pc;
    std::string m_ground_frame_id;
    std::string m_camera_frame_id;
    double m_floor_height;
    double m_ceiling_height;
    double m_pointcloud_max_distance;
    yarp::sig::Matrix m_transform_mtrx;

public:
    LaserFromPointCloud(double period = 0.01) : PeriodicThread(period),
        Lidar2DDeviceBase()
    {}

    ~LaserFromPointCloud()
    {
    }

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool threadInit() override;
    void threadRelease() override;
    void run() override;

public:
    //IRangefinder2D interface
    bool setDistanceRange    (double min, double max) override;
    bool setScanLimits        (double min, double max) override;
    bool setHorizontalResolution      (double step) override;
    bool setScanRate         (double rate) override;

public:
    //Lidar2DDeviceBase
    bool acquireDataFromHW() override final;
};

#endif
