/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdint>

#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <yarp/sig/ImageUtils.h>
#include <yarp/dev/RGBDSensorParamParser.h>

#include "RGBDRosConversionUtils.h"
#include "rosPixelCode.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::dev::RGBDRosConversionUtils;
using namespace std;

namespace {
YARP_LOG_COMPONENT(RGBD_ROS, "yarp.device.RGBDRosConversion")
}

commonImageProcessor::commonImageProcessor(string cameradata_topic_name, string camerainfo_topic_name)
{
    if (this->topic(cameradata_topic_name)==false)
    {
        yCError(RGBD_ROS) << "Error opening topic:" << cameradata_topic_name;
    }
    if (m_subscriber_camera_info.topic(camerainfo_topic_name) == false)
    {
        yCError(RGBD_ROS) << "Error opening topic:" << camerainfo_topic_name;
    }
    m_cameradata_topic_name = cameradata_topic_name;
    m_camerainfo_topic_name = camerainfo_topic_name;
    m_contains_rgb_data = false;
    m_contains_depth_data = false;
}
commonImageProcessor::~commonImageProcessor()
{
    this->close();
    m_subscriber_camera_info.close();
}

bool commonImageProcessor::getLastRGBData(yarp::sig::FlexImage& data, yarp::os::Stamp& stmp)
{
    if (m_contains_rgb_data == false) { return false;}

    //this blocks untils the first data is received;
    /*size_t counter = 0;
    while (m_contains_rgb_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS) << "Waiting for incoming rgb data..."; counter = 0; }
    }*/

    m_port_mutex.lock();
    data = m_lastRGBImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
    return true;
}

bool commonImageProcessor::getLastDepthData(yarp::sig::ImageOf<yarp::sig::PixelFloat>& data, yarp::os::Stamp& stmp)
{
    if (m_contains_depth_data == false) { return false;}

    //this blocks untils the first data is received;
    /*size_t counter = 0;
    while (m_contains_depth_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS) << "Waiting for incoming depth data..."; counter = 0; }
    }*/

    m_port_mutex.lock();
    data = m_lastDepthImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
    return true;
}

size_t commonImageProcessor::getWidth() const
{
   return m_lastDepthImage.width();
}

size_t commonImageProcessor::getHeight() const
{
    return m_lastDepthImage.height();
}

void commonImageProcessor::onRead(yarp::rosmsg::sensor_msgs::Image& v)
{
    m_port_mutex.lock();
    int yarp_pixcode = yarp::dev::ROSPixelCode::Ros2YarpPixelCode(v.encoding);
    if (yarp_pixcode == VOCAB_PIXEL_RGB ||
        yarp_pixcode == VOCAB_PIXEL_BGR)
    {
        m_lastRGBImage.setPixelCode(yarp_pixcode);
        m_lastRGBImage.resize(v.width, v.height);
        size_t c = 0;
        for (auto it = v.data.begin(); it != v.data.end(); it++)
        {
            m_lastRGBImage.getRawImage()[c++]=*it;
        }
        m_lastStamp.update();
        m_contains_rgb_data = true;
    }
    else if (v.encoding == TYPE_16UC1)
    {
        m_lastDepthImage.resize(v.width, v.height);
        size_t c = 0;
        uint16_t* p = (uint16_t*)(v.data.data());
        uint16_t* siz = (uint16_t*)(v.data.data()) + (v.data.size() / sizeof(uint16_t));
        int count = 0;
        for (; p < siz; p++)
        {
            float value = float(*p) / 1000.0;
            ((float*)(m_lastDepthImage.getRawImage()))[c++] = value;
            count++;
        }
        m_lastStamp.update();
        m_contains_depth_data = true;
    }
    else if (v.encoding == TYPE_32FC1)
    {
        m_lastDepthImage.resize(v.width, v.height);
        size_t c = 0;
        for (auto it = v.data.begin(); it != v.data.end(); it++)
        {
            m_lastDepthImage.getRawImage()[c++] = *it;
        }
        m_lastStamp.update();
        m_contains_depth_data = true;
    }
    else
    {
        yCError(RGBD_ROS) << "Unsupported rgb/depth format:" << v.encoding;
    }
    m_port_mutex.unlock();
}

bool commonImageProcessor::getFOV(double& horizontalFov, double& verticalFov) const
{
    yarp::rosmsg::sensor_msgs::CameraInfo* tmp = m_subscriber_camera_info.read(true);
    m_lastCameraInfo = *tmp;
    yarp::sig::IntrinsicParams params;
    params.focalLengthX = m_lastCameraInfo.K[0];
    params.focalLengthY = m_lastCameraInfo.K[4];
    params.principalPointX = m_lastCameraInfo.K[2];
    params.principalPointY = m_lastCameraInfo.K[5];
    yCError(RGBD_ROS) << "getIntrinsicParam not yet implemented";
    return false;
}

bool commonImageProcessor::getIntrinsicParam(yarp::os::Property& intrinsic) const
{
    yarp::rosmsg::sensor_msgs::CameraInfo* tmp = m_subscriber_camera_info.read(true);
    m_lastCameraInfo = *tmp;
    intrinsic.clear();
    yarp::sig::IntrinsicParams params;
    params.focalLengthX = m_lastCameraInfo.K[0];
    params.focalLengthY = m_lastCameraInfo.K[4];
    params.principalPointX = m_lastCameraInfo.K[2];
    params.principalPointY = m_lastCameraInfo.K[5];
    // distortion model
    if (m_lastCameraInfo.distortion_model=="plumb_bob")
    {
        params.distortionModel.type = YarpDistortion::YARP_PLUMB_BOB;
        params.distortionModel.k1 = m_lastCameraInfo.D[0];
        params.distortionModel.k2 = m_lastCameraInfo.D[1];
        params.distortionModel.t1 = m_lastCameraInfo.D[2];
        params.distortionModel.t2 = m_lastCameraInfo.D[3];
        params.distortionModel.k3 = m_lastCameraInfo.D[4];
    }
    else
    {
        yCError(RGBD_ROS) << "Unsupported distortion model";
    }
    params.toProperty(intrinsic);
    return false;
}


void yarp::dev::RGBDRosConversionUtils::shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest)
{
    dest.setPixelCode(src.getPixelCode());
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}

void yarp::dev::RGBDRosConversionUtils::shallowCopyImages(const ImageOf<PixelFloat>& src, ImageOf<PixelFloat>& dest)
{
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}

void yarp::dev::RGBDRosConversionUtils::deepCopyImages(const yarp::sig::FlexImage& src,
    yarp::rosmsg::sensor_msgs::Image& dest,
    const string& frame_id,
    const yarp::rosmsg::TickTime& timeStamp,
    const unsigned int& seq)
{
    dest.data.resize(src.getRawImageSize());
    dest.width = src.width();
    dest.height = src.height();
    memcpy(dest.data.data(), src.getRawImage(), src.getRawImageSize());
    dest.encoding = yarp::dev::ROSPixelCode::yarp2RosPixelCode(src.getPixelCode());
    dest.step = src.getRowSize();
    dest.header.frame_id = frame_id;
    dest.header.stamp = timeStamp;
    dest.header.seq = seq;
    dest.is_bigendian = 0;
}

void yarp::dev::RGBDRosConversionUtils::deepCopyImages(const DepthImage& src,
    yarp::rosmsg::sensor_msgs::Image& dest,
    const string& frame_id,
    const yarp::rosmsg::TickTime& timeStamp,
    const unsigned int& seq)
{
    dest.data.resize(src.getRawImageSize());

    dest.width = src.width();
    dest.height = src.height();

    memcpy(dest.data.data(), src.getRawImage(), src.getRawImageSize());

    dest.encoding = yarp::dev::ROSPixelCode::yarp2RosPixelCode(src.getPixelCode());
    dest.step = src.getRowSize();
    dest.header.frame_id = frame_id;
    dest.header.stamp = timeStamp;
    dest.header.seq = seq;
    dest.is_bigendian = 0;
}
