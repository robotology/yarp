/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <algorithm>

#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>
#include <yarp/sig/ImageUtils.h>

#include "RGBDSensorFromRosTopic.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

using namespace std;

namespace {
YARP_LOG_COMPONENT(RGBD_ROS_TOPIC, "yarp.device.RGBDSensorFromRosTopic")
}


bool RGBDSensorFromRosTopic::open(Searchable& config)
{
    if (!config.check("depth_topic_name")) {
        yCError(RGBD_ROS_TOPIC) << "missing depth_topic_name parameter, using default one";
        return false;
    }
    std::string depth_topic_name = config.find("depth_topic_name").asString();
    if(depth_topic_name[0] != '/'){
        yCError(RGBD_ROS_TOPIC) << "depth_topic_name must begin with an initial /";
        return false;
    }

    if (!config.check("color_topic_name")) {
        yCError(RGBD_ROS_TOPIC) << "missing color_topic_name parameter, using default one";
        return false;
    }
    std::string color_topic_name = config.find("color_topic_name").asString();
    if(color_topic_name[0] != '/'){
        yCError(RGBD_ROS_TOPIC) << "color_topic_name must begin with an initial /";
        return false;
    }
    std::string rgb_info_topic_name = color_topic_name.substr(0,color_topic_name.rfind('/')) + "/camera_info";
    std::string depth_info_topic_name = depth_topic_name.substr(0,depth_topic_name.rfind('/')) + "/camera_info";

    if (!config.check("node_name")) {
        yCError(RGBD_ROS_TOPIC) << "missing node_name parameter, using default one";
        return false;
    }
    std::string node_name = config.find("node_name").asString();
    if(node_name[0] != '/'){
        yCError(RGBD_ROS_TOPIC) << "node_name must begin with an initial /";
        return false;
    }
    m_ros_node = new yarp::os::Node(node_name);

    //m_rgb_input_processor.useCallback();    ///@@@<-SEGFAULT
    //m_depth_input_processor.useCallback();    ///@@@<-SEGFAULT
    m_rgb_input_processor = new yarp::dev::RGBDRosConversionUtils::commonImageProcessor(color_topic_name, rgb_info_topic_name);
    m_depth_input_processor = new yarp::dev::RGBDRosConversionUtils::commonImageProcessor(depth_topic_name, depth_info_topic_name);
    m_rgb_input_processor->useCallback();    ///@@@<-OK
    m_depth_input_processor->useCallback();    ///@@@<-OK

    return true;
}

bool RGBDSensorFromRosTopic::close()
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

int RGBDSensorFromRosTopic::getRgbHeight()
{
    if (m_rgb_input_processor==nullptr)
    {
        return 0;
    }
    return static_cast<int>(m_rgb_input_processor->getHeight());
}

int RGBDSensorFromRosTopic::getRgbWidth()
{
    if (m_rgb_input_processor==nullptr)
    {
        return 0;
    }
    return static_cast<int>(m_rgb_input_processor->getWidth());
}

bool RGBDSensorFromRosTopic::getRgbSupportedConfigurations(yarp::sig::VectorOf<CameraConfig> &configurations)
{
    YARP_UNUSED(configurations);
    yCWarning(RGBD_ROS_TOPIC) << "getRgbSupportedConfigurations not implemented yet";
    return false;
}

bool RGBDSensorFromRosTopic::getRgbResolution(int &width, int &height)
{
    if (m_rgb_input_processor == nullptr)
    {
        width=0;
        height=0;
        return  true;
    }
    width  = static_cast<int>(m_rgb_input_processor->getWidth());
    height = static_cast<int>(m_rgb_input_processor->getHeight());
    return true;
}

bool RGBDSensorFromRosTopic::setDepthResolution(int width, int height)
{
    YARP_UNUSED(width);
    YARP_UNUSED(height);
    yCWarning(RGBD_ROS_TOPIC) << "setDepthResolution not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setRgbResolution(int width, int height)
{
    YARP_UNUSED(width);
    YARP_UNUSED(height);
    yCWarning(RGBD_ROS_TOPIC) << "setRgbResolution not supported";
    return false;
}


bool RGBDSensorFromRosTopic::setRgbFOV(double horizontalFov, double verticalFov)
{
    YARP_UNUSED(horizontalFov);
    YARP_UNUSED(verticalFov);
    yCWarning(RGBD_ROS_TOPIC) << "setRgbFOV not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setDepthFOV(double horizontalFov, double verticalFov)
{
    YARP_UNUSED(horizontalFov);
    YARP_UNUSED(verticalFov);
    yCWarning(RGBD_ROS_TOPIC) << "setDepthFOV not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setDepthAccuracy(double accuracy)
{
    YARP_UNUSED(accuracy);
    yCWarning(RGBD_ROS_TOPIC) << "setDepthAccuracy not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getRgbFOV(double &horizontalFov, double &verticalFov)
{
    if (m_rgb_input_processor == nullptr)
    {
        horizontalFov=0;
        verticalFov=0;
        return true;
    }
    return m_rgb_input_processor->getFOV(horizontalFov, verticalFov);
}

bool RGBDSensorFromRosTopic::getRgbMirroring(bool& mirror)
{
    YARP_UNUSED(mirror);
    yCWarning(RGBD_ROS_TOPIC) << "Mirroring not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setRgbMirroring(bool mirror)
{
    YARP_UNUSED(mirror);
    yCWarning(RGBD_ROS_TOPIC) << "Mirroring not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getRgbIntrinsicParam(Property& intrinsic)
{
    if (m_rgb_input_processor == nullptr)
    {
        intrinsic.clear();
        return  true;
    }
    return m_rgb_input_processor->getIntrinsicParam(intrinsic);
}

int  RGBDSensorFromRosTopic::getDepthHeight()
{
    if (m_depth_input_processor == nullptr)
    {
        return  0;
    }
    return static_cast<int>(m_depth_input_processor->getHeight());
}

int  RGBDSensorFromRosTopic::getDepthWidth()
{
    if (m_depth_input_processor == nullptr)
    {
        return  0;
    }
    return static_cast<int>(m_depth_input_processor->getWidth());
}

bool RGBDSensorFromRosTopic::getDepthFOV(double& horizontalFov, double& verticalFov)
{
    if (m_depth_input_processor == nullptr)
    {
        horizontalFov = 0;
        verticalFov = 0;
        return true;
    }
    return m_depth_input_processor->getFOV(horizontalFov, verticalFov);
}

bool RGBDSensorFromRosTopic::getDepthIntrinsicParam(Property& intrinsic)
{
    if (m_depth_input_processor == nullptr)
    {
        intrinsic.clear();
        return true;
    }
    return m_depth_input_processor->getIntrinsicParam(intrinsic);
}

double RGBDSensorFromRosTopic::getDepthAccuracy()
{
    yCWarning(RGBD_ROS_TOPIC) << "getDepthAccuracy not supported";
    return 0;
}

bool RGBDSensorFromRosTopic::getDepthClipPlanes(double& nearPlane, double& farPlane)
{
    YARP_UNUSED(nearPlane);
    YARP_UNUSED(farPlane);
    yCWarning(RGBD_ROS_TOPIC) << "getDepthClipPlanes not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setDepthClipPlanes(double nearPlane, double farPlane)
{
    YARP_UNUSED(nearPlane);
    YARP_UNUSED(farPlane);
    yCWarning(RGBD_ROS_TOPIC) << "setDepthClipPlanes not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getDepthMirroring(bool& mirror)
{
    YARP_UNUSED(mirror);
    yCWarning(RGBD_ROS_TOPIC) << "getDepthMirroring not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setDepthMirroring(bool mirror)
{
    YARP_UNUSED(mirror);
    yCWarning(RGBD_ROS_TOPIC) << "setDepthMirroring not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getExtrinsicParam(Matrix& extrinsic)
{
    YARP_UNUSED(extrinsic);
    yCWarning(RGBD_ROS_TOPIC) << "getExtrinsicParam not supported";
    return  false;
}

bool RGBDSensorFromRosTopic::getRgbImage(FlexImage& rgbImage, Stamp* timeStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    bool rgb_ok = false;
    if (m_rgb_input_processor!=nullptr)
        { rgb_ok = m_rgb_input_processor->getLastRGBData(rgbImage, *timeStamp); }
    return rgb_ok;
}

bool RGBDSensorFromRosTopic::getDepthImage(ImageOf<PixelFloat>& depthImage, Stamp* timeStamp)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    bool depth_ok =false;
    if (m_depth_input_processor != nullptr)
       { depth_ok = m_depth_input_processor->getLastDepthData(depthImage, *timeStamp); }
    return depth_ok;
}

bool RGBDSensorFromRosTopic::getImages(FlexImage& colorFrame, ImageOf<PixelFloat>& depthFrame, Stamp* colorStamp, Stamp* depthStamp)
{
    bool rgb_ok = false;
    bool depth_ok = false;
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_rgb_input_processor != nullptr)
       { rgb_ok = m_rgb_input_processor->getLastRGBData(colorFrame, *colorStamp); }
    if (m_depth_input_processor != nullptr)
       { depth_ok = m_depth_input_processor->getLastDepthData(depthFrame, *depthStamp); }
    return (rgb_ok && depth_ok);
}

RGBDSensorFromRosTopic::RGBDSensor_status RGBDSensorFromRosTopic::getSensorStatus()
{
    return RGBD_SENSOR_OK_IN_USE;
}

std::string RGBDSensorFromRosTopic::getLastErrorMsg(Stamp* timeStamp)
{
    YARP_UNUSED(timeStamp);
    return m_lastError;
}

/*
//IFrameGrabberControls
bool RGBDSensorFromRosTopic::getCameraDescription(CameraDescriptor* camera)
{
    camera->deviceDescription = "Ros Camera";
    camera->busType = BusType::BUS_UNKNOWN;
    return true;
}

bool RGBDSensorFromRosTopic::hasFeature(int feature, bool* hasFeature)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasFeature not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setFeature(int feature, double value)
{
    yCWarning(RGBD_ROS_TOPIC) << "setFeature not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getFeature(int feature, double *value)
{
    yCWarning(RGBD_ROS_TOPIC) << "getFeature not supported";
    return false;    return true;
}

bool RGBDSensorFromRosTopic::setFeature(int feature, double value1, double value2)
{
    yCWarning(RGBD_ROS_TOPIC) << "setFeature not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getFeature(int feature, double *value1, double *value2)
{
    yCWarning(RGBD_ROS_TOPIC) << "getFeature not supported";
    return false;
}

bool RGBDSensorFromRosTopic::hasOnOff(  int feature, bool *HasOnOff)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasOnOff not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setActive( int feature, bool onoff)
{
    yCWarning(RGBD_ROS_TOPIC) << "setActive not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getActive( int feature, bool *isActive)
{
    yCWarning(RGBD_ROS_TOPIC) << "getActive not supported";
    return false;
}

bool RGBDSensorFromRosTopic::hasAuto(int feature, bool *hasAuto)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasAuto not supported";
    return false;
}

bool RGBDSensorFromRosTopic::hasManual( int feature, bool* hasManual)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasManual not supported";
    return false;
}

bool RGBDSensorFromRosTopic::hasOnePush(int feature, bool* hasOnePush)
{
    yCWarning(RGBD_ROS_TOPIC) << "hasOnePush not supported";
    return false;
}

bool RGBDSensorFromRosTopic::setMode(int feature, FeatureMode mode)
{
    yCWarning(RGBD_ROS_TOPIC) << "setMode not supported";
    return false;
}

bool RGBDSensorFromRosTopic::getMode(int feature, FeatureMode* mode)
{
    yCWarning(RGBD_ROS_TOPIC) << "getMode not supported";
    *mode = MODE_UNKNOWN;
    return false;
}

bool RGBDSensorFromRosTopic::setOnePush(int feature)
{
    yCWarning(RGBD_ROS_TOPIC) << "setOnePush not supported";
    return false;
}
*/
