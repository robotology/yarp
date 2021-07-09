/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabber_nws_ros.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>

#include <rosPixelCode.h>

namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWS_ROS, "yarp.device.frameGrabber_nws_ros")
} // namespace


FrameGrabber_nws_ros::FrameGrabber_nws_ros() :
        PeriodicThread(s_default_period)
{
}


FrameGrabber_nws_ros::~FrameGrabber_nws_ros()
{
    close();
}


bool FrameGrabber_nws_ros::close()
{
    if (!m_active) {
        return false;
    }
    m_active = false;

    detach();

    publisherPort_image.interrupt();
    publisherPort_image.close();

    publisherPort_cameraInfo.interrupt();
    publisherPort_cameraInfo.close();

    if (node != nullptr) {
        node->interrupt();
        delete node;
        node = nullptr;
    }

    if (subdevice) {
        subdevice->close();
        delete subdevice;
        subdevice = nullptr;
    }

    isSubdeviceOwned = false;

    return true;
}


bool FrameGrabber_nws_ros::open(yarp::os::Searchable& config)
{
    if (m_active) {
        yCError(FRAMEGRABBER_NWS_ROS, "Device is already opened");
        return false;
    }

    // Check "period" option
    if (config.check("period", "refresh period(in s) of the broadcasted values through yarp ports") && config.find("period").isFloat64()) {
        m_period = config.find("period").asFloat64();
    } else {
        yCInfo(FRAMEGRABBER_NWS_ROS)
            << "Period parameter not found, using default of"
            << s_default_period
            << "seconds";
    }
    PeriodicThread::setPeriod(m_period);

    // Check "node_name" option and open node
    if (!config.check("node_name"))
    {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing node_name parameter";
        return false;
    }
    std::string nodeName = config.find("node_name").asString();
    if (nodeName.c_str()[0] != '/') {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing '/' in node_name parameter";
        return false;
    }

    node = new yarp::os::Node(nodeName);

    // Check "topic_name" option and open publisher
    if (!config.check("topic_name"))
    {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing topic_name parameter";
        return false;
    }
    std::string topicName = config.find("topic_name").asString();
    if (topicName.c_str()[0] != '/') {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing '/' in topic_name parameter";
        return false;
    }

    // set "imageTopicName" and open publisher
    if (!publisherPort_image.topic(topicName)) {
        yCError(FRAMEGRABBER_NWS_ROS) << "Unable to publish data on " << topicName << " topic, check your yarp-ROS network configuration";
        return false;
    }

    // set "cameraInfoTopicName" and open publisher


    std::string cameraInfoTopicName = topicName.substr(0,topicName.rfind('/')) + "/camera_info";
    if (!publisherPort_cameraInfo.topic(cameraInfoTopicName)) {
        yCError(FRAMEGRABBER_NWS_ROS) << "Unable to publish data on" << cameraInfoTopicName << "topic, check your yarp-ROS network configuration";
        return false;
    }

    // Check "frame_id" option
    if (!config.check("frame_id"))
    {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing frame_id parameter";
        return false;
    }
    m_frameId = config.find("frame_id").asString();
    if (m_frameId.c_str()[0] != '/') {
        yCError(FRAMEGRABBER_NWS_ROS) << "Missing '/' in frame_id parameter";
        return false;
    }

    // Check "subdevice" option and eventually open the device
    isSubdeviceOwned = config.check("subdevice");
    if (isSubdeviceOwned) {
        yarp::os::Property p;
        subdevice = new yarp::dev::PolyDriver;
        p.fromString(config.toString());
        p.put("pixelType", VOCAB_PIXEL_RGB);
        p.setMonitor(config.getMonitor(), "subdevice"); // pass on any monitoring
        p.unput("device");
        p.put("device", config.find("subdevice").asString()); // subdevice was already checked before

        // if errors occurred during open, quit here.
        subdevice->open(p);

        if (!(subdevice->isValid())) {
            yCError(FRAMEGRABBER_NWS_ROS, "Unable to open subdevice");
            return false;
        }
        if (!attach(subdevice)) {
            yCError(FRAMEGRABBER_NWS_ROS, "Unable to attach subdevice");
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWS_ROS) << "Running, waiting for attach...";
    }

    m_active = true;

    return true;
}

bool FrameGrabber_nws_ros::attach(yarp::dev::PolyDriver* poly)
{
    if (!poly->isValid()) {
        yCError(FRAMEGRABBER_NWS_ROS) << "Device " << poly << " to attach to is not valid ... cannot proceed";
        return false;
    }

    poly->view(iRgbVisualParams);
    poly->view(iFrameGrabberImage);
    poly->view(iPreciselyTimed);

    if (iFrameGrabberImage == nullptr) {
        yCError(FRAMEGRABBER_NWS_ROS) << "IFrameGrabberImage interface is not available on the device";
        return false;
    }

    if (iRgbVisualParams == nullptr) {
        yCWarning(FRAMEGRABBER_NWS_ROS) << "IRgbVisualParams interface is not available on the device";
    }

    return PeriodicThread::start();
}


bool FrameGrabber_nws_ros::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    iRgbVisualParams = nullptr;
    iFrameGrabberImage = nullptr;
    iPreciselyTimed = nullptr;

    return true;
}

bool FrameGrabber_nws_ros::threadInit()
{
    img = new yarp::sig::ImageOf<yarp::sig::PixelRgb>;
    return true;
}

void FrameGrabber_nws_ros::threadRelease()
{
    delete img;
    img = nullptr;
}


// Publish the images on the buffered port
void FrameGrabber_nws_ros::run()
{
    if (publisherPort_image.getOutputCount() == 0 && publisherPort_cameraInfo.getOutputCount() == 0) {
        // If no ports are connected, do not call getImage on the interface.
        return;
    }

    if (iPreciselyTimed) {
        m_stamp = iPreciselyTimed->getLastInputStamp();
    } else {
        m_stamp.update(yarp::os::Time::now());
    }

    if (iFrameGrabberImage && publisherPort_image.getOutputCount() > 0) {
        iFrameGrabberImage->getImage(*img);
        auto& image = publisherPort_image.prepare();

        image.data.resize(img->getRawImageSize());
        image.width = img->width();
        image.height = img->height();
        image.encoding = yarp::dev::ROSPixelCode::yarp2RosPixelCode(img->getPixelCode());
        image.step = img->getRowSize();
        image.header.frame_id = m_frameId;
        image.header.stamp = m_stamp.getTime();
        image.header.seq = m_stamp.getCount();
        image.is_bigendian = 0;

        memcpy(image.data.data(), img->getRawImage(), img->getRawImageSize());

        publisherPort_image.setEnvelope(m_stamp);
        publisherPort_image.write();
    }

    if (iRgbVisualParams && publisherPort_cameraInfo.getOutputCount() > 0) {
        auto& cameraInfo = publisherPort_cameraInfo.prepare();

        if (setCamInfo(cameraInfo)) {
            publisherPort_cameraInfo.setEnvelope(m_stamp);
            publisherPort_cameraInfo.write();
        } else {
            publisherPort_cameraInfo.unprepare();
        }
    }
}

namespace {
template <class T>
struct param
{
    param(T& inVar, std::string inName) :
        var(&inVar),
        parname(std::move(inName))
    {
    }
    T*              var;
    std::string     parname;
};
} // namespace

bool FrameGrabber_nws_ros::setCamInfo(yarp::rosmsg::sensor_msgs::CameraInfo& cameraInfo)
{
    yCAssert(FRAMEGRABBER_NWS_ROS, iRgbVisualParams);

    yarp::os::Property camData;
    if (!iRgbVisualParams->getRgbIntrinsicParam(camData)) {
        yCErrorThreadOnce(FRAMEGRABBER_NWS_ROS) << "Unable to get intrinsic param from rgb sensor!";
        return false;
    }

    if (!camData.check("distortionModel")) {
        yCWarning(FRAMEGRABBER_NWS_ROS) << "Missing distortion model";
        return false;
    }

    std::string distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob") {
        yCError(FRAMEGRABBER_NWS_ROS) << "Distortion model not supported";
        return false;
    }

    double phyF = 0.0;
    double fx = 0.0;
    double fy = 0.0;
    double cx = 0.0;
    double cy = 0.0;
    double k1 = 0.0;
    double k2 = 0.0;
    double t1 = 0.0;
    double t2 = 0.0;
    double k3 = 0.0;

    std::vector<param<double>> parVector;
    parVector.emplace_back(phyF,"physFocalLength");
    parVector.emplace_back(fx,"focalLengthX");
    parVector.emplace_back(fy,"focalLengthY");
    parVector.emplace_back(cx,"principalPointX");
    parVector.emplace_back(cy,"principalPointY");
    parVector.emplace_back(k1,"k1");
    parVector.emplace_back(k2,"k2");
    parVector.emplace_back(t1,"t1");
    parVector.emplace_back(t2,"t2");
    parVector.emplace_back(k3,"k3");

    for(auto& par : parVector) {
        if(!camData.check(par.parname)) {
            yCWarning(FRAMEGRABBER_NWS_ROS) << "Driver has not the param:" << par.parname;
            return false;
        }
        *(par.var) = camData.find(par.parname).asFloat64();
    }

    cameraInfo.header.frame_id    = m_frameId;
    cameraInfo.header.seq         = m_stamp.getCount();
    cameraInfo.header.stamp       = m_stamp.getTime();
    cameraInfo.width              = iRgbVisualParams->getRgbWidth();
    cameraInfo.height             = iRgbVisualParams->getRgbHeight();
    cameraInfo.distortion_model   = distModel;

    cameraInfo.D.resize(5);
    cameraInfo.D[0] = k1;
    cameraInfo.D[1] = k2;
    cameraInfo.D[2] = t1;
    cameraInfo.D[3] = t2;
    cameraInfo.D[4] = k3;

    cameraInfo.K.resize(9);
    cameraInfo.K[0]  = fx;       cameraInfo.K[1] = 0;        cameraInfo.K[2] = cx;
    cameraInfo.K[3]  = 0;        cameraInfo.K[4] = fy;       cameraInfo.K[5] = cy;
    cameraInfo.K[6]  = 0;        cameraInfo.K[7] = 0;        cameraInfo.K[8] = 1;

    /*
     * ROS documentation on cameraInfo message:
     * "Rectification matrix (stereo cameras only)
     * A rotation matrix aligning the camera coordinate system to the ideal
     * stereo image plane so that epipolar lines in both stereo images are
     * parallel."
     * useless in our case, it will be an identity matrix
     */

    cameraInfo.R.assign(9, 0);
    cameraInfo.R.at(0) = cameraInfo.R.at(4) = cameraInfo.R.at(8) = 1;

    cameraInfo.P.resize(12);
    cameraInfo.P[0]  = fx;      cameraInfo.P[1] = 0;    cameraInfo.P[2]  = cx;  cameraInfo.P[3]  = 0;
    cameraInfo.P[4]  = 0;       cameraInfo.P[5] = fy;   cameraInfo.P[6]  = cy;  cameraInfo.P[7]  = 0;
    cameraInfo.P[8]  = 0;       cameraInfo.P[9] = 0;    cameraInfo.P[10] = 1;   cameraInfo.P[11] = 0;

    cameraInfo.binning_x  = cameraInfo.binning_y = 0;
    cameraInfo.roi.height = cameraInfo.roi.width = cameraInfo.roi.x_offset = cameraInfo.roi.y_offset = 0;
    cameraInfo.roi.do_rectify = false;
    return true;
}
