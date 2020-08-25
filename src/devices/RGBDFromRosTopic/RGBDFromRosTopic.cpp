/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdint>

#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <yarp/sig/ImageUtils.h>

#include "RGBDFromRosTopic.h"
#include "../RGBDSensorWrapper/rosPixelCode.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

using namespace std;

namespace {
YARP_LOG_COMPONENT(RGBD_ROS_TOPIC, "yarp.device.RGBDFromRosTopic")
}

commonImageProcessor::commonImageProcessor(string cameradata_topic_name, string camerainfo_topic_name)
{
    if (this->topic(cameradata_topic_name)==false)
    {
        yCError(RGBD_ROS_TOPIC) << "Error opening topic:" << cameradata_topic_name;
    }
    if (m_subscriber_camera_info.topic(camerainfo_topic_name) == false)
    {
        yCError(RGBD_ROS_TOPIC) << "Error opening topic:" << camerainfo_topic_name;
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

void commonImageProcessor::getLastData(yarp::sig::FlexImage& data, yarp::os::Stamp& stmp)
{
    //this blocks untils the first data is received;
    size_t counter = 0;
    while (m_contains_rgb_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS_TOPIC) << "Waiting for incoming data..."; counter = 0; }
    }

    m_port_mutex.lock();
    data = m_lastRGBImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
}

void commonImageProcessor::getLastData(yarp::sig::ImageOf<yarp::sig::PixelFloat>& data, yarp::os::Stamp& stmp)
{
    //this blocks untils the first data is received;
    size_t counter = 0;
    while (m_contains_depth_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS_TOPIC) << "Waiting for incoming data..."; counter = 0; }
    }

    m_port_mutex.lock();
    data = m_lastDepthImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
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
        yCError(RGBD_ROS_TOPIC) << "Unsupported rgb/depth format:" << v.encoding;
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
    yCError(RGBD_ROS_TOPIC) << "getIntrinsicParam not yet implemented";
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
        params.distortionModel.type = YarpDistortion::YARP_PLUM_BOB;
        params.distortionModel.k1 = m_lastCameraInfo.D[0];
        params.distortionModel.k2 = m_lastCameraInfo.D[1];
        params.distortionModel.t1 = m_lastCameraInfo.D[2];
        params.distortionModel.t2 = m_lastCameraInfo.D[3];
        params.distortionModel.k3 = m_lastCameraInfo.D[4];
    }
    else
    {
        yCError(RGBD_ROS_TOPIC) << "Unsupported distortion model";
    }
    params.toProperty(intrinsic);
    return false;
}

RGBDFromRosTopic::RGBDFromRosTopic() : m_verbose(false),
                                       m_initialized(false)
{
}

bool RGBDFromRosTopic::open(Searchable& config)
{
    m_rgb_data_topic_name = "/camera/color/image_raw";
    m_rgb_info_topic_name = "/camera/color/camera_info";
    m_depth_data_topic_name = "/camera/depth/image_rect_raw";
    m_depth_info_topic_name = "/camera/depth/camera_info";
    if (config.check("rgb_data_topic"))   { m_rgb_data_topic_name = config.find("rgb_data_topic").asString();}
    if (config.check("rgb_info_topic"))   { m_rgb_info_topic_name = config.find("rgb_info_topic").asString(); }
    if (config.check("depth_data_topic")) { m_depth_data_topic_name = config.find("depth_data_topic").asString(); }
    if (config.check("depth_info_topic")) { m_depth_info_topic_name = config.find("depth_info_topic").asString(); }
    m_verbose = config.check("verbose");

    m_ros_node = new yarp::os::Node("/RGBDFromRosTopicNode");

    //m_rgb_input_processor.useCallback();    ///@@@<-SEGFAULT
    //m_depth_input_processor.useCallback();    ///@@@<-SEGFAULT
    m_rgb_input_processor = new commonImageProcessor(m_rgb_data_topic_name, m_rgb_info_topic_name);
    m_depth_input_processor = new commonImageProcessor(m_depth_data_topic_name, m_depth_info_topic_name);
    m_rgb_input_processor->useCallback();    ///@@@<-OK
    m_depth_input_processor->useCallback();    ///@@@<-OK

    return true;
}

bool RGBDFromRosTopic::close()
{
    if (m_rgb_input_processor)
    {
       delete m_rgb_input_processor;
       m_rgb_input_processor =nullptr;
    } 
    if (m_depth_input_processor)
    {
       delete m_depth_input_processor;
       m_depth_input_processor = nullptr;
    }
    if (m_ros_node)
    { 
       delete m_ros_node;
       m_ros_node=nullptr;
    }
    return true;
}

int RGBDFromRosTopic::getRgbHeight()
{
    if (m_rgb_input_processor==nullptr) return 0;
    return (int)m_rgb_input_processor->getHeight();
}

int RGBDFromRosTopic::getRgbWidth()
{
    if (m_rgb_input_processor == nullptr) return 0;
    return (int)m_rgb_input_processor->getWidth();
}

bool RGBDFromRosTopic::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yCWarning(RGBD_ROS_TOPIC) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool RGBDFromRosTopic::getRgbResolution(int &width, int &height)
{
    if (m_rgb_input_processor == nullptr)
    {
        width=0;
        height=0;
        return  true;
    }
    width  = (int)m_rgb_input_processor->getWidth();
    height = (int)m_rgb_input_processor->getHeight();
    return true;
}

bool RGBDFromRosTopic::setDepthResolution(int width, int height)
{
    yCWarning(RGBD_ROS_TOPIC) << "setDepthResolution not supported";
    return false;
}

bool RGBDFromRosTopic::setRgbResolution(int width, int height)
{
    yCWarning(RGBD_ROS_TOPIC) << "setRgbResolution not supported";
    return false;
}


bool RGBDFromRosTopic::setRgbFOV(double horizontalFov, double verticalFov)
{
    yCWarning(RGBD_ROS_TOPIC) << "setRgbFOV not supported";
    return false;
}

bool RGBDFromRosTopic::setDepthFOV(double horizontalFov, double verticalFov)
{
    yCWarning(RGBD_ROS_TOPIC) << "setDepthFOV not supported";
    return false;
}

bool RGBDFromRosTopic::setDepthAccuracy(double accuracy)
{
    yCWarning(RGBD_ROS_TOPIC) << "setDepthAccuracy not supported";
    return false;
}

bool RGBDFromRosTopic::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    if (m_rgb_input_processor == nullptr)
    {
        horizontalFov=0;
        verticalFov=0;
        return true;
    }
    return m_rgb_input_processor->getFOV(horizontalFov, verticalFov);
}

bool RGBDFromRosTopic::getRgbMirroring(bool& mirror)
{
    yCWarning(RGBD_ROS_TOPIC) << "Mirroring not supported";
    return false;
}

bool RGBDFromRosTopic::setRgbMirroring(bool mirror)
{
    yCWarning(RGBD_ROS_TOPIC) << "Mirroring not supported";
    return false;
}

bool RGBDFromRosTopic::getRgbIntrinsicParam(Property& intrinsic)
{
    return m_rgb_input_processor->getIntrinsicParam(intrinsic);
}

int  RGBDFromRosTopic::getDepthHeight()
{
    if (m_depth_input_processor == nullptr)
    {
        return  0;
    }
    return (int)m_depth_input_processor->getHeight();
}

int  RGBDFromRosTopic::getDepthWidth()
{
    if (m_depth_input_processor == nullptr)
    {
        return  0;
    }
    return (int)m_depth_input_processor->getWidth();
}

bool RGBDFromRosTopic::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    if (m_depth_input_processor == nullptr)
    {
        horizontalFov = 0;
        verticalFov = 0;
        return true;
    }
    return m_depth_input_processor->getFOV(horizontalFov, verticalFov);
}

bool RGBDFromRosTopic::getDepthIntrinsicParam(Property& intrinsic)
{
    if (m_depth_input_processor == nullptr)
    {
        intrinsic.clear();
        return true;
    }
    return m_depth_input_processor->getIntrinsicParam(intrinsic);
}

double RGBDFromRosTopic::getDepthAccuracy()
{
    yCWarning(RGBD_ROS_TOPIC) << "getDepthAccuracy not supported";
    return 0;
}

bool RGBDFromRosTopic::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    yCWarning(RGBD_ROS_TOPIC) << "getDepthClipPlanes not supported";
    return false;
}

bool RGBDFromRosTopic::setDepthClipPlanes(double nearPlane, double farPlane)
{
    yCWarning(RGBD_ROS_TOPIC) << "setDepthClipPlanes not supported";
    return false;
}

bool RGBDFromRosTopic::getDepthMirroring(bool& mirror)
{
    yCWarning(RGBD_ROS_TOPIC) << "getDepthMirroring not supported";
    return false;
}

bool RGBDFromRosTopic::setDepthMirroring(bool mirror)
{
    yCWarning(RGBD_ROS_TOPIC) << "setDepthMirroring not supported";
    return false;
}

bool RGBDFromRosTopic::getExtrinsicParam(Matrix& extrinsic)
{
    yCWarning(RGBD_ROS_TOPIC) << "getExtrinsicParam not supported";
    return  false;
};

bool RGBDFromRosTopic::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_rgb_input_processor!=nullptr)
        m_rgb_input_processor->getLastData(rgbImage, *timeStamp);
    return true;
}

bool RGBDFromRosTopic::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_depth_input_processor != nullptr)
        m_depth_input_processor->getLastData(depthImage, *timeStamp);
    return true;
}

bool RGBDFromRosTopic::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_rgb_input_processor != nullptr)
        m_rgb_input_processor->getLastData(colorFrame, *colorStamp);
    if (m_depth_input_processor != nullptr)
        m_depth_input_processor->getLastData(depthFrame, *depthStamp);
    return true;
}

RGBDFromRosTopic::RGBDSensor_status RGBDFromRosTopic::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

std::string RGBDFromRosTopic::getLastErrorMsg(Stamp* timeStamp)
{
    return m_lastError;
}

/*
//IFrameGrabberControls
bool RGBDFromRosTopic::getCameraDescription(CameraDescriptor* camera)
{
    camera->deviceDescription = "Ros Camera";
    camera->busType = BusType::BUS_UNKNOWN;
    return true;
}

bool RGBDFromRosTopic::hasFeature(int feature, bool* hasFeature)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasFeature not supported";
    return false;
}

bool RGBDFromRosTopic::setFeature(int feature, double value)
{
    yCWarning(RGBD_ROS_TOPIC) << "setFeature not supported";
    return false;
}

bool RGBDFromRosTopic::getFeature(int feature, double *value)
{
    yCWarning(RGBD_ROS_TOPIC) << "getFeature not supported";
    return false;    return true;
}

bool RGBDFromRosTopic::setFeature(int feature, double value1, double value2)
{
    yCWarning(RGBD_ROS_TOPIC) << "setFeature not supported";
    return false;
}

bool RGBDFromRosTopic::getFeature(int feature, double *value1, double *value2)
{
    yCWarning(RGBD_ROS_TOPIC) << "getFeature not supported";
    return false;
}

bool RGBDFromRosTopic::hasOnOff(  int feature, bool *HasOnOff)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasOnOff not supported";
    return false;
}

bool RGBDFromRosTopic::setActive( int feature, bool onoff)
{
    yCWarning(RGBD_ROS_TOPIC) << "setActive not supported";
    return false;
}

bool RGBDFromRosTopic::getActive( int feature, bool *isActive)
{
    yCWarning(RGBD_ROS_TOPIC) << "getActive not supported";
    return false;
}

bool RGBDFromRosTopic::hasAuto(int feature, bool *hasAuto)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasAuto not supported";
    return false;
}

bool RGBDFromRosTopic::hasManual( int feature, bool* hasManual)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasManual not supported";
    return false;
}

bool RGBDFromRosTopic::hasOnePush(int feature, bool* hasOnePush)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasOnePush not supported";
    return false;
}

bool RGBDFromRosTopic::setMode(int feature, FeatureMode mode)
{
    yCWarning(RGBD_ROS_TOPIC) << "setMode not supported";
    return false;
}

bool RGBDFromRosTopic::getMode(int feature, FeatureMode* mode)
{
    yCWarning(RGBD_ROS_TOPIC) << "getMode not supported";
    *mode = MODE_UNKNOWN;
    return false;
}

bool RGBDFromRosTopic::setOnePush(int feature)
{
    yCWarning(RGBD_ROS_TOPIC) << "setOnePush not supported";
    return false;
}
*/
