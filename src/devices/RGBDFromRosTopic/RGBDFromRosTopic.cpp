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

colorImageInputProcessor::colorImageInputProcessor()
{
    m_contains_data = false;
}

depthImageInputProcessor::depthImageInputProcessor()
{
    m_contains_data = false;
}

void colorImageInputProcessor::getLastData(yarp::sig::FlexImage& data, yarp::os::Stamp& stmp)
{
    //this blocks untils the first data is received;
    size_t counter = 0;
    while (m_contains_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS_TOPIC) << "Waiting for incoming data..."; counter = 0; }
    }

    m_port_mutex.lock();
    data = m_lastRGBImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
}

void depthImageInputProcessor::getLastData(depthImage& data, yarp::os::Stamp& stmp)
{
    //this blocks untils the first data is received;
    size_t counter = 0;
    while (m_contains_data == false)
    {
        yarp::os::Time::delay(0.1);
        if (counter++ > 100) { yCDebug(RGBD_ROS_TOPIC) << "Waiting for incoming data..."; counter = 0; }
    }

    m_port_mutex.lock();
    data = m_lastDepthImage;
    stmp = m_lastStamp;
    m_port_mutex.unlock();
}

size_t depthImageInputProcessor::getWidth() const
{
   return m_lastDepthImage.width();
}

size_t depthImageInputProcessor::getHeight() const
{
    return m_lastDepthImage.height();
}

size_t colorImageInputProcessor::getWidth() const
{
    return m_lastRGBImage.width();
}

size_t colorImageInputProcessor::getHeight() const
{
    return m_lastRGBImage.height();
}

void colorImageInputProcessor::onRead(yarp::rosmsg::sensor_msgs::Image& v)
{
    m_port_mutex.lock();
    int yarp_pixcode = yarp::dev::ROSPixelCode::Ros2YarpPixelCode(v.encoding);
    if (/*yarp_pixcode_is_ok*/ 1)
    {
        m_lastRGBImage.setPixelCode(yarp_pixcode);
        m_lastRGBImage.resize(v.width, v.height);
        size_t c = 0;
        for (auto it = v.data.begin(); it != v.data.end(); it++)
        {
            m_lastRGBImage.getRawImage()[c++]=*it;
        }
        m_lastStamp.update();
        m_contains_data = true;
    }
    m_port_mutex.unlock();
}

void depthImageInputProcessor::onRead(yarp::rosmsg::sensor_msgs::Image& v)
{
    m_port_mutex.lock();
    m_lastDepthImage.resize(v.width, v.height);
   // int yarp_pixcode = yarp::dev::ROSPixelCode::Ros2YarpPixelCode(v.encoding);
    if (v.encoding == TYPE_16UC1)
    {
        size_t c = 0;
        uint16_t* p   = (uint16_t*)(v.data.data());
        uint16_t* siz = (uint16_t*)(v.data.data()) + (v.data.size() / sizeof(uint16_t));
        int count = 0;
        for (; p < siz; p++)
        {
            float value = float(*p)/1000.0;
            ((float*)(m_lastDepthImage.getRawImage()))[c++] = value;
            count++;
        }
        m_lastStamp.update();
        m_contains_data = true;
    }
    else if (v.encoding == TYPE_32FC1)
    {
        size_t c = 0;
        for (auto it = v.data.begin(); it != v.data.end(); it++)
        {
            m_lastDepthImage.getRawImage()[c++] = *it;
        }
        m_lastStamp.update();
        m_contains_data = true;
    }
    else
    {
        yCError(RGBD_ROS_TOPIC) << "Unsupported depth format:" << v.encoding;
    }
    m_port_mutex.unlock();
}

bool depthImageInputProcessor::getFOV(double& horizontalFov, double& verticalFov) const
{
    yCError(RGBD_ROS_TOPIC) << "getIntrinsicParam not yet implemented";
    return false;
}

bool depthImageInputProcessor::getIntrinsicParam(yarp::os::Property& intrinsic) const
{
    yCError(RGBD_ROS_TOPIC) << "getIntrinsicParam not yet implemented";
    intrinsic.clear();
    return false;
}
bool colorImageInputProcessor::getFOV(double& horizontalFov, double& verticalFov) const
{
    yarp::sig::IntrinsicParams params;
    params.focalLengthX = m_lastCameraInfo.K[0];
    params.focalLengthY = m_lastCameraInfo.K[4];
    params.principalPointX = m_lastCameraInfo.K[2];
    params.principalPointY = m_lastCameraInfo.K[5];
    yCError(RGBD_ROS_TOPIC) << "getIntrinsicParam not yet implemented";
    return false;
}

bool colorImageInputProcessor::getIntrinsicParam(yarp::os::Property& intrinsic) const
{
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
    m_verbose = config.check("verbose");

    m_ros_node = new yarp::os::Node("/RGBDFromRosTopicNode");
    m_rgb_topic_name = "/camera/color/image_raw";
    m_depth_topic_name = "/camera/depth/image_rect_raw";
    //m_rgb_input_processor.useCallback();    ///@@@<-SEGFAULT
    //m_depth_input_processor.useCallback();    ///@@@<-SEGFAULT
    if (m_rgb_input_processor.topic(m_rgb_topic_name) == false)
    {
        yCError(RGBD_ROS_TOPIC) << "Error opening topic:" << m_rgb_topic_name;
        return false;
    }
    if (m_depth_input_processor.topic(m_depth_topic_name) == false)
    {
        yCError(RGBD_ROS_TOPIC) << "Error opening topic:" << m_depth_topic_name;
        return false;
    }
    m_rgb_input_processor.useCallback();    ///@@@<-OK
    m_depth_input_processor.useCallback();    ///@@@<-OK

    return true;
}

bool RGBDFromRosTopic::close()
{
    m_rgb_input_processor.close();
    m_depth_input_processor.close();
    if (m_ros_node) {delete m_ros_node; m_ros_node=nullptr;}
    return true;
}

int RGBDFromRosTopic::getRgbHeight()
{
    return (int)m_rgb_input_processor.getHeight();
}

int RGBDFromRosTopic::getRgbWidth()
{
    return (int)m_rgb_input_processor.getWidth();
}

bool RGBDFromRosTopic::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    yCWarning(RGBD_ROS_TOPIC) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool RGBDFromRosTopic::getRgbResolution(int &width, int &height)
{
    width  = (int)m_rgb_input_processor.getWidth();
    height = (int)m_rgb_input_processor.getHeight();
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
    return m_rgb_input_processor.getFOV(horizontalFov, verticalFov);
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
    return m_rgb_input_processor.getIntrinsicParam(intrinsic);
}

int  RGBDFromRosTopic::getDepthHeight()
{
    return (int)m_depth_input_processor.getHeight();
}

int  RGBDFromRosTopic::getDepthWidth()
{
    return (int)m_depth_input_processor.getWidth();
}

bool RGBDFromRosTopic::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    return m_depth_input_processor.getFOV(horizontalFov, verticalFov);
}

bool RGBDFromRosTopic::getDepthIntrinsicParam(Property& intrinsic)
{
    return m_depth_input_processor.getIntrinsicParam(intrinsic);
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
    m_rgb_input_processor.getLastData(rgbImage, *timeStamp);
    return true;
}

bool RGBDFromRosTopic::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_depth_input_processor.getLastData(depthImage, *timeStamp);
    return true;
}

bool RGBDFromRosTopic::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_rgb_input_processor.getLastData(colorFrame, *colorStamp);
    m_depth_input_processor.getLastData(depthFrame, *depthStamp);
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
