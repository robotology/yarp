/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include "laserFromPCL.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/math/Math.h>

#include <yarp/rosmsg/sensor_msgs/PointCloud2.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <mutex>

using namespace std;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

/*

yarpdev --device Rangefinder2DWrapper --subdevice laserFromPCL \
        --ROS::useROS true --ROS::ROS_nodeName /cer-laserFront \
        --ROS::ROS_topicName /laserDepth --ROS::frame_id /mobile_base_lidar_F \
        --GENERAL \
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
        --period 10 \
        --name /outlaser:o
*/

yarp::os::Publisher<yarp::rosmsg::sensor_msgs::PointCloud2>* pointCloud_outTopic = nullptr;
yarp::os::Node                                             * rosNode=nullptr;

void ros_init_pc()
{
  //  rosNode = new yarp::os::Node("laserFromDepth");

    pointCloud_outTopic=new yarp::os::Publisher<yarp::rosmsg::sensor_msgs::PointCloud2>;
    if (pointCloud_outTopic->topic("/ros_pc")==false)
    {
        yError() << "opening topic";
    }
    else
    {
        yInfo() << "topic successful";
    }
}

void ros_compute_and_send_pc(const yarp::sig::PointCloud<yarp::sig::DataXYZ>& pc, string frame_id)
{
    //yDebug() << "sizeof:" << sizeof(yarp::sig::DataXYZ);

    yarp::rosmsg::sensor_msgs::PointCloud2               rosPC_data;
    static int counter=0;
    rosPC_data.header.stamp.nsec=0;
    rosPC_data.header.stamp.sec=0;
    rosPC_data.header.seq=counter++;
    rosPC_data.header.frame_id = frame_id;

    rosPC_data.fields.resize(3);
    rosPC_data.fields[0].name       = "x";
    rosPC_data.fields[0].offset     = 0;    // offset in bytes from start of each point
    rosPC_data.fields[0].datatype   = 7;    // 7 = FLOAT32
    rosPC_data.fields[0].count      = 1;    // how many FLOAT32 used for 'x'

    rosPC_data.fields[1].name       = "y";
    rosPC_data.fields[1].offset     = 4;    // offset in bytes from start of each point
    rosPC_data.fields[1].datatype   = 7;    // 7 = FLOAT32
    rosPC_data.fields[1].count      = 1;    // how many FLOAT32 used for 'y'

    rosPC_data.fields[2].name       = "z";
    rosPC_data.fields[2].offset     = 8;    // offset in bytes from start of each point
    rosPC_data.fields[2].datatype   = 7;    // 7 = FLOAT32
    rosPC_data.fields[2].count      = 1;    // how many FLOAT32 used for 'z'

#if defined(YARP_BIG_ENDIAN)
    rosPC_data.is_bigendian = true;
#elif defined(YARP_LITTLE_ENDIAN)
    rosPC_data.is_bigendian = false;
#else
    #error "Cannot detect endianness"
#endif

#if 0
    rosPC_data.height=1;
    rosPC_data.width=pc.size();
#else
    rosPC_data.height=pc.height();
    rosPC_data.width=pc.width();
#endif

    rosPC_data.point_step = 3*4; //x, y, z
    rosPC_data.row_step   = rosPC_data.point_step*rosPC_data.width; //12 *number of points bytes
    rosPC_data.is_dense = true;   // what this field actually means?? When is it false??
    rosPC_data.data.resize(rosPC_data.row_step*rosPC_data.height);

    const char* ypointer = pc.getRawData()+12;
    unsigned char* rpointer = rosPC_data.data.data();

    //yDebug()<< pc.size() << pc.size()*4*4 << pc.dataSizeBytes() << rosPC_data.data.size();
    size_t elem =0;
    size_t yelem=0;
    for (; elem<pc.size()*3*4; elem++)
    {
        *rpointer=*ypointer;
        rpointer++;
        ypointer++; yelem++;
        if (elem%12==0) { ypointer+=4; yelem+=4;}
        // yDebug("%d" ,ypointer);
    }
   //yDebug()<<elem <<yelem;

    if (pointCloud_outTopic) pointCloud_outTopic->write(rosPC_data);
}

//-------------------------------------------------------------------------------------

bool LaserFromPCL::open(yarp::os::Searchable& config)
{
    Property subConfig;
    m_info = "LaserFromDepth device";
    m_device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    m_min_distance = 0.1; //m
    m_max_distance = 5.0;  //m
    m_floor_height = 0.1; //m
    m_ceiling_height = 2; //m
    m_publish_ros_pc =true;
    m_ground_frame_id = "/ground_frame";
    m_camera_frame_id = "/depth_camera_frame";

    bool bpc = config.check("Z_CLIPPING_PLANES");
    if (bpc != false)
    {
        yarp::os::Searchable& pointcloud_clip_config = config.findGroup("Z_CLIPPING_PLANES");
        if (pointcloud_clip_config.check("floor_height"))   {m_floor_height = pointcloud_clip_config.find("floor_height").asFloat64();}
        if (pointcloud_clip_config.check("ceiling_height")) {m_ceiling_height=pointcloud_clip_config.find("ceiling_height").asFloat64();}
        if (pointcloud_clip_config.check("ground_frame_id")) {m_ground_frame_id = pointcloud_clip_config.find("ground_frame_id").asString();}
        if (pointcloud_clip_config.check("camera_frame_id")) {m_camera_frame_id = pointcloud_clip_config.find("camera_frame_id").asString();}

    }
    yInfo() <<"Z clipping planes (floor,ceiling) have been set to ("<< m_floor_height <<","<< m_ceiling_height <<")";

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        m_clip_max_enable = general_config.check("clip_max");
        m_clip_min_enable = general_config.check("clip_min");
        if (m_clip_max_enable) { m_max_distance = general_config.find("clip_max").asFloat64(); }
        if (m_clip_min_enable) { m_min_distance = general_config.find("clip_min").asFloat64(); }
        m_do_not_clip_infinity_enable = (general_config.find("allow_infinity").asInt32()!=0);
    }
    else
    {
        yError() << "Missing GENERAL section";
        return false;
    }
    bool bs = config.check("SKIP");
    if (bs != false)
    {
        yarp::os::Searchable& skip_config = config.findGroup("SKIP");
        Bottle mins = skip_config.findGroup("min");
        Bottle maxs = skip_config.findGroup("max");
        size_t s_mins = mins.size();
        size_t s_maxs = mins.size();
        if (s_mins == s_maxs && s_maxs > 1 )
        {
            for (size_t s = 1; s < s_maxs; s++)
            {
                Range_t range;
                range.max = maxs.get(s).asFloat64();
                range.min = mins.get(s).asFloat64();
                if (range.max >= 0 && range.max <= 360 &&
                    range.min >= 0 && range.min <= 360 &&
                    range.max > range.min)
                {
                    m_range_skip_vector.push_back(range);
                }
                else
                {
                    yError() << "Invalid range in SKIP section";
                    return false;
                }
            }
        }

    }

    //open the tc client
    Property tcprop;
    if(!config.check("TRANSFORM_CLIENT"))
    {
        yError() << "missing TRANSFORM_CLIENT section in configuration file!";
        return false;
    }
    tcprop.fromString(config.findGroup("TRANSFORM_CLIENT").toString());
    tcprop.put("device", "transformClient");
    //prop.put("local", "/"+m_module_name + "/TfClient");
    //prop.put("remote", "/transformServer");

    m_tc_driver.open(tcprop);
    if (!m_tc_driver.isValid())
    {
        yError("Error opening PolyDriver check parameters");
        return false;
    }
    m_tc_driver.view(m_iTc);
    if (!m_iTc)
    {
        yError("Error opening iFrameTransform interface. Device not available");
        return false;
    }
    yarp::os::Time::delay(0.1);


    //open the rgbd client
    Property prop;
    if(!config.check("RGBD_SENSOR_CLIENT"))
    {
        yError() << "missing RGBD_SENSOR_CLIENT section in configuration file!";
        return false;
    }
    prop.fromString(config.findGroup("RGBD_SENSOR_CLIENT").toString());
    prop.put("device", "RGBDSensorClient");
    m_rgbd_driver.open(prop);
    if (!m_rgbd_driver.isValid())
    {
        yError("Error opening PolyDriver check parameters");
        return false;
    }
    m_rgbd_driver.view(m_iRGBD);
    if (!m_iRGBD)
    {
        yError("Error opening iRGBD interface. Device not available");
        return false;
    }
    yarp::os::Time::delay(0.1);

    //get parameters data from the camera
    m_depth_width = m_iRGBD->getDepthWidth();
    m_depth_height = m_iRGBD->getDepthHeight();
    bool propintr  = m_iRGBD->getDepthIntrinsicParam(m_propIntrinsics);
    yInfo() << "Depth Intrinsics:" << m_propIntrinsics.toString();
    m_intrinsics.fromProperty(m_propIntrinsics);

    m_sensorsNum = 360;
    m_resolution = 1;
    m_laser_data.resize(m_sensorsNum, 0.0);
    m_max_angle = 360;
    m_min_angle = 0;
    PeriodicThread::start();

    yInfo("Sensor ready");

    //init ros
    ros_init_pc();

    return true;
}

bool LaserFromPCL::close()
{
    PeriodicThread::stop();

    if(m_rgbd_driver.isValid())
        m_rgbd_driver.close();

    if(m_tc_driver.isValid())
        m_tc_driver.close();

    yInfo() << "LaserFromDepth closed";
    return true;
}

bool LaserFromPCL::getDistanceRange(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(mutex);
    min = m_min_distance;
    max = m_max_distance;
    return true;
}

bool LaserFromPCL::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> guard(mutex);
    m_min_distance = min;
    m_max_distance = max;
    return true;
}

bool LaserFromPCL::getScanLimits(double& min, double& max)
{
    std::lock_guard<std::mutex> guard(mutex);
    min = m_min_angle;
    max = m_max_angle;
    return true;
}

bool LaserFromPCL::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setScanLimits not yet implemented");
    return true;
}

bool LaserFromPCL::getHorizontalResolution(double& step)
{
    std::lock_guard<std::mutex> guard(mutex);
    step = m_resolution;
    return true;
}

bool LaserFromPCL::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setHorizontalResolution not yet implemented");
    return true;
}

bool LaserFromPCL::getScanRate(double& rate)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("getScanRate not yet implemented");
    return true;
}

bool LaserFromPCL::setScanRate(double rate)
{
    std::lock_guard<std::mutex> guard(mutex);
    yWarning("setScanRate not yet implemented");
    return false;
}


bool LaserFromPCL::getRawData(yarp::sig::Vector &out)
{
    std::lock_guard<std::mutex> guard(mutex);
    out = m_laser_data;
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool LaserFromPCL::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    std::lock_guard<std::mutex> guard(mutex);
#ifdef LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
    size_t size = m_laser_data.size();
    data.resize(size);
    if (m_max_angle < m_min_angle) { yError() << "getLaserMeasurement failed"; return false; }
    double laser_angle_of_view = m_max_angle - m_min_angle;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + m_min_angle)* DEG2RAD;
        data[i].set_polar(m_laser_data[i], angle);
    }
    m_device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}
bool LaserFromPCL::getDeviceStatus(Device_status &status)
{
    std::lock_guard<std::mutex> guard(mutex);
    status = m_device_status;
    return true;
}

bool LaserFromPCL::threadInit()
{
#ifdef LASER_DEBUG
    yDebug("LaserFromDepth:: thread initialising...\n");
    yDebug("... done!\n");
#endif

    return true;
}

#define TEST_M 1
double m_floor_height   = 0.3;
double m_ceiling_height = 2;

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

void LaserFromPCL::run()
{
#ifdef DEBUG_TIMING
    double t1 = yarp::os::Time::now();
#endif
    std::lock_guard<std::mutex> guard(mutex);

    m_iRGBD->getDepthImage(m_depth_image);
    if (m_depth_image.getRawImage()==nullptr)
    {
        yDebug()<<"invalid image received";
        return;
    }

    if (m_depth_image.width()!=m_depth_width ||
        m_depth_image.height()!=m_depth_height)
    {
        yDebug()<<"invalid image size";
        return;
    }

    const double myinf =std::numeric_limits<double>::infinity();

    //prepare an empty laserscan vector with the resolution we want
    for (auto it= m_laser_data.begin(); it!=m_laser_data.end(); it++)
    {
        *it= myinf;
    }

    //compute the point cloud
    yarp::sig::PointCloud<yarp::sig::DataXYZ> pc = yarp::sig::utils::depthToPC(m_depth_image, m_intrinsics);

    //if (m_publish_ros_pc) {ros_compute_and_send_pc(pc,m_camera_frame_id);}//<-------------------------

#if TEST_M
    //yDebug() << "pc size:" << pc.size();
#endif

    //we compute the transformation matrix from the camera to the laser reference frame
    yarp::sig::Matrix m(4,4); m.eye();

#if 0//TEST_M
    yarp::sig::Vector vvv(3);
    vvv(0)=-1.57;
    vvv(1)=0;
    vvv(2)=-1.57;
    m = yarp::math::rpy2dcm(vvv);
    m(2,3)=1.2; //z translation
#else
    bool frame_exists = m_iTc->getTransform(m_camera_frame_id,m_ground_frame_id,m);
    if (frame_exists==false)
    {
        yWarning() << "Unable to found m matrix";
    }
#endif

    //we rototranslate the full pointcloud
    rotate_pc(pc, m);

    if (m_publish_ros_pc) {ros_compute_and_send_pc(pc,m_ground_frame_id);}//<-------------------------

    for (size_t i=0; i<pc.size(); i++)
    {

#if TEST_M
     //yDebug() << pc(i).toString(5,5);
#endif

        //we obtain a point from the point cloud
        yarp::sig::Vector vec= pc(i).toVector4();

        //we check if the point is in the volume that we want to consider as possibile obstacle
        if (vec[2]>m_floor_height && vec[2]<m_ceiling_height)
        {
#if TEST_M
    //        yDebug() << "This point is ok:" << i <<"its z is:" << tvec[2];
#endif
            //by removing z, we project the 3d point on the 2D plane on which the laser works.
            //we use LaserMeasurementData struct to easily obtain a polar representation from a cartesian representation
            LaserMeasurementData data;
            data.set_cartesian(vec[0],vec[1]);

            //get the polar representation
            double distance;
            double theta;
            data.get_polar(distance, theta);

            //compute the right element of the vector where to put distance data. This is done by clusterizing angles, depending on the laser resolution.
            theta=theta*180/M_PI;
            if      (theta<0)   theta+=360;
            else if (theta>360) theta-=360;
            size_t elem= theta/m_resolution;
            if (elem>=m_laser_data.size())
            {
                yError() << "Error in computing elem" << i << ">" << m_laser_data.size();
                continue;
            }

#if TEST_M
            // yDebug() <<theta << elem <<distance;
#endif
            //clipping stuff
            if (m_clip_min_enable && distance < m_min_distance)
            {
                distance = m_max_distance;
            }

            if (m_clip_max_enable              &&
                distance > m_max_distance      &&
                !m_do_not_clip_infinity_enable &&
                distance <= myinf)
            {
                distance = m_max_distance;
            }

            //update the vector of measurements, putting the NEAREST obstacle in right element of the vector.
            if (distance<m_laser_data[elem]) m_laser_data[elem]=distance;
        }
        else
        {
#if TEST_M
            //yDebug() << "this point is out of considered volume:" <<i << " z:" << vec[2];
#endif
        }
    }


/*
    //the skip stuff is not used for now
    for (i = 0; i < m_range_skip_vector.size(); i++)
    {
        if (angle > m_range_skip_vector[i].min && angle < m_range_skip_vector[i].max)
        {
            distance = infinity;
        }
    }
*/

    return;
}

void LaserFromPCL::threadRelease()
{
#ifdef LASER_DEBUG
    yDebug("LaserFromDepth Thread releasing...");
    yDebug("... done.");
#endif

    return;
}

bool LaserFromPCL::getDeviceInfo(std::string &device_info)
{
    std::lock_guard<std::mutex> guard(mutex);
    device_info = m_info;
    return true;
}
