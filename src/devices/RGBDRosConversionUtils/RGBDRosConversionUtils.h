/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RGBD_ROS_UTILS_H
#define RGBD_ROS_UTILS_H

#include <iostream>
#include <cstring>
#include <mutex>

#include <yarp/os/PeriodicThread.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Property.h>

#include <yarp/os/Node.h>
#include <yarp/os/Subscriber.h>
#include <yarp/rosmsg/sensor_msgs/CameraInfo.h>
#include <yarp/rosmsg/sensor_msgs/Image.h>

#include <yarp/rosmsg/impl/yarpRosHelper.h>

typedef yarp::sig::ImageOf<yarp::sig::PixelFloat> DepthImage;

namespace yarp {
    namespace dev {
        namespace RGBDRosConversionUtils {

class commonImageProcessor:
    public yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>
{
    protected:
    yarp::sig::FlexImage   m_lastRGBImage;
    DepthImage             m_lastDepthImage;

    protected:
    std::mutex             m_port_mutex;
    mutable yarp::os::Subscriber   <yarp::rosmsg::sensor_msgs::CameraInfo> m_subscriber_camera_info;
    std::string            m_cameradata_topic_name;
    std::string            m_camerainfo_topic_name;
    mutable yarp::rosmsg::sensor_msgs::CameraInfo m_lastCameraInfo;
    yarp::os::Stamp        m_lastStamp;
    bool                   m_contains_rgb_data;
    bool                   m_contains_depth_data;

    public:
    commonImageProcessor (std::string data_topic_name, std::string camera_info_topic_name);
    virtual ~commonImageProcessor();
    using yarp::os::Subscriber<yarp::rosmsg::sensor_msgs::Image>::onRead;
    virtual void onRead(yarp::rosmsg::sensor_msgs::Image& v) override;

    public:
    size_t getWidth() const;
    size_t getHeight() const;
    bool getFOV(double& horizontalFov, double& verticalFov) const;
    bool getIntrinsicParam(yarp::os::Property& intrinsic) const;

    public:
    bool getLastRGBData(yarp::sig::FlexImage& data, yarp::os::Stamp& stmp);
    bool getLastDepthData(yarp::sig::ImageOf<yarp::sig::PixelFloat>& data, yarp::os::Stamp& stmp);
};

void deepCopyImages(const yarp::sig::FlexImage& src,
    yarp::rosmsg::sensor_msgs::Image& dest,
    const std::string& frame_id,
    const yarp::rosmsg::TickTime& timeStamp,
    const unsigned int& seq);

void deepCopyImages(const DepthImage& src,
    yarp::rosmsg::sensor_msgs::Image& dest,
    const std::string& frame_id,
    const yarp::rosmsg::TickTime& timeStamp,
    const unsigned int& seq);

void shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest);

void shallowCopyImages(const DepthImage& src, DepthImage& dest);

}
}
}

#endif
