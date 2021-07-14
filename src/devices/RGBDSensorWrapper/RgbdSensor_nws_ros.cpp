/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RgbdSensor_nws_ros.h"
#include "rosPixelCode.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>

#include <RGBDRosConversionUtils.h>
#include <cstdio>
#include <cstring>
#include <sstream>

using namespace RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RGBDSENSORNWSROS, "yarp.devices.RgbdSensor_nws_ros")


RgbdSensor_nws_ros::RgbdSensor_nws_ros() :
    PeriodicThread(DEFAULT_THREAD_PERIOD),
    m_node(nullptr),
    nodeSeq(0),
    period(DEFAULT_THREAD_PERIOD),
    sensor_p(nullptr),
    fgCtrl(nullptr),
    sensorStatus(IRGBDSensor::RGBD_SENSOR_NOT_READY),
    verbose(4),
    forceInfoSync(true),
    isSubdeviceOwned(false),
    subDeviceOwned(nullptr)
{}

RgbdSensor_nws_ros::~RgbdSensor_nws_ros()
{
    close();
    sensor_p = nullptr;
    fgCtrl = nullptr;
}

/** Device driver interface */

bool RgbdSensor_nws_ros::open(yarp::os::Searchable &config)
{
    if(verbose >= 5)
    {
        yCTrace(RGBDSENSORNWSROS) << "\nParameters are: \n" << config.toString();
    }
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        if (verbose >= 3) {
            yCInfo(RGBDSENSORNWSROS) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
        }
    }
    else
    {
        period = config.find("period").asInt32() / 1000.0;
    }

    //check if param exist and assign it to corresponding variable.. if it doesn't, initialize the variable with default value.

    if (config.check("forceInfoSync"))
    {
        forceInfoSync = config.find("forceInfoSync").asBool();
    }

    if(config.check("subdevice")) {
        isSubdeviceOwned=true;
    } else {
        isSubdeviceOwned=false;
    }

    setId("RGBDSensorWrapper for " + nodeName);

    if(!initialize_ROS(config))
    {
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on through attachAll()
    if(isSubdeviceOwned)
    {
        if(! openAndAttachSubDevice(config))
        {
            yCError(RGBDSENSORNWSROS, "Error while opening subdevice");
            return false;
        }
    }
    else
    {
        if (!openDeferredAttach(config)) {
            return false;
        }
    }

    return true;
}

bool RgbdSensor_nws_ros::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RgbdSensor_nws_ros::openAndAttachSubDevice(Searchable& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(RGBDSENSORNWSROS, "Opening IRGBDSensor subdevice");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yCError(RGBDSENSORNWSROS, "Opening IRGBDSensor subdevice... FAILED");
        return false;
    }
    isSubdeviceOwned = true;
    if(!attach(subDeviceOwned)) {
        return false;
    }

    return true;
}

bool RgbdSensor_nws_ros::close()
{
    yCTrace(RGBDSENSORNWSROS, "Close");
    detach();

    // close subdevice if it was created inside the open (--subdevice option)
    if(isSubdeviceOwned)
    {
        if(subDeviceOwned)
        {
            delete subDeviceOwned;
            subDeviceOwned=nullptr;
        }
        sensor_p = nullptr;
        fgCtrl = nullptr;
        isSubdeviceOwned = false;
    }

    if(m_node !=nullptr)
    {
        m_node->interrupt();
        delete m_node;
        m_node = nullptr;
    }

    return true;
}

/* Helper functions */

bool RgbdSensor_nws_ros::initialize_ROS(yarp::os::Searchable &params)
{
    // depth topic base name check
    if (!params.check("depth_topic_name")) {
        yCError(RGBDSENSORNWSROS) << "missing depth_topic_name parameter";
        return false;
    }
    std::string depth_topic_name = params.find("depth_topic_name").asString();
    if(depth_topic_name[0] != '/'){
        yCError(RGBDSENSORNWSROS) << "depth_topic_name must begin with an initial /";
        return false;
    }

    // color topic base name check
    if (!params.check("color_topic_name")) {
        yCError(RGBDSENSORNWSROS) << "missing color_topic_name parameter";
        return false;
    }
    std::string color_topic_name = params.find("color_topic_name").asString();
    if(color_topic_name[0] != '/'){
        yCError(RGBDSENSORNWSROS) << "color_topic_name must begin with an initial /";
        return false;
    }

    // node_name check
    if (!params.check("node_name")) {
        yCError(RGBDSENSORNWSROS) << "missing node_name parameter";
        return false;
    }
    std::string node_name = params.find("node_name").asString();
    if(node_name[0] != '/'){
        yCError(RGBDSENSORNWSROS) << "node_name must begin with an initial /";
        return false;
    }

    // node_name check
    if (!params.check("depth_frame_id")) {
        yCError(RGBDSENSORNWSROS) << "missing depth_frame_id parameter";
        return false;
    }
    m_depth_frame_id = params.find("depth_frame_id").asString();

    if (!params.check("color_frame_id")) {
        yCError(RGBDSENSORNWSROS) << "missing color_frame_id parameter";
        return false;
    }
    m_color_frame_id = params.find("color_frame_id").asString();

    // open topics here if needed
    m_node = new yarp::os::Node(nodeName);
    nodeSeq = 0;

    if (!publisherPort_color.topic(color_topic_name))
    {
        yCError(RGBDSENSORNWSROS) << "Unable to publish data on " << color_topic_name.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }

    if (!publisherPort_depth.topic(depth_topic_name))
    {
        yCError(RGBDSENSORNWSROS) << "Unable to publish data on " << depth_topic_name.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }
    std::string rgb_info_topic_name = color_topic_name.substr(0,color_topic_name.rfind('/')) + "/camera_info";
    if (!publisherPort_colorCaminfo.topic(rgb_info_topic_name))
    {
        yCError(RGBDSENSORNWSROS) << "Unable to publish data on " << rgb_info_topic_name.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }

    std::string depth_info_topic_name = depth_topic_name.substr(0,depth_topic_name.rfind('/')) + "/camera_info";
    if (!publisherPort_depthCaminfo.topic(depth_info_topic_name))
    {
        yCError(RGBDSENSORNWSROS) << "Unable to publish data on " << depth_info_topic_name.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }
    return true;
}

void RgbdSensor_nws_ros::setId(const std::string &id)
{
    sensorId=id;
}

std::string RgbdSensor_nws_ros::getId()
{
    return sensorId;
}

/**
  * WrapperSingle interface
  */

bool RgbdSensor_nws_ros::detach()
{
    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned) {
        return false;
    }

    sensor_p = nullptr;
    return true;
}

bool RgbdSensor_nws_ros::attach(PolyDriver* poly)
{
    if(poly)
    {
        poly->view(sensor_p);
        poly->view(fgCtrl);
    }

    if(sensor_p == nullptr)
    {
        yCError(RGBDSENSORNWSROS) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }

    if(fgCtrl == nullptr)
    {
        yCWarning(RGBDSENSORNWSROS) << "Attached device has no valid IFrameGrabberControls interface.";
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

/* IRateThread interface */

bool RgbdSensor_nws_ros::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RgbdSensor_nws_ros::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

bool RgbdSensor_nws_ros::setCamInfo(yarp::rosmsg::sensor_msgs::CameraInfo& cameraInfo, const string& frame_id, const UInt& seq, const SensorType& sensorType)
{
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
    double stamp = 0.0;

    string                  distModel;
    string                  currentSensor;
    UInt                    i;
    Property                camData;
    vector<param<double> >  parVector;
    param<double>*          par;
    bool                    ok;

    currentSensor = sensorType == COLOR_SENSOR ? "Rgb" : "Depth";
    ok            = sensorType == COLOR_SENSOR ? sensor_p->getRgbIntrinsicParam(camData) : sensor_p->getDepthIntrinsicParam(camData);

    if (!ok)
    {
        yCError(RGBDSENSORNWSROS) << "Unable to get intrinsic param from" << currentSensor << "sensor!";
        return false;
    }

    if(!camData.check("distortionModel"))
    {
        yCWarning(RGBDSENSORNWSROS) << "Missing distortion model";
        return false;
    }

    distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob")
    {
        yCError(RGBDSENSORNWSROS) << "Distortion model not supported";
        return false;
    }

    //std::vector<param<string> >     rosStringParam;
    //rosStringParam.push_back(param<string>(nodeName, "asd"));

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
    parVector.emplace_back(stamp,"stamp");
    for(i = 0; i < parVector.size(); i++)
    {
        par = &parVector[i];

        if(!camData.check(par->parname))
        {
            yCWarning(RGBDSENSORNWSROS) << "Driver has not the param:" << par->parname;
            return false;
        }
        *par->var = camData.find(par->parname).asFloat64();
    }

    cameraInfo.header.frame_id    = frame_id;
    cameraInfo.header.seq         = seq;
    cameraInfo.header.stamp       = stamp;
    cameraInfo.width              = sensorType == COLOR_SENSOR ? sensor_p->getRgbWidth() : sensor_p->getDepthWidth();
    cameraInfo.height             = sensorType == COLOR_SENSOR ? sensor_p->getRgbHeight() : sensor_p->getDepthHeight();
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

bool RgbdSensor_nws_ros::writeData()
{
    //colorImage.setPixelCode(VOCAB_PIXEL_RGB);
    //             depthImage.setPixelCode(VOCAB_PIXEL_MONO_FLOAT);

    //             colorImage.resize(hDim, vDim);  // Has this to be done each time? If size is the same what it does?
    //             depthImage.resize(hDim, vDim);
    if (!sensor_p->getImages(colorImage, depthImage, &colorStamp, &depthStamp))
    {
        return false;
    }

    static Stamp oldColorStamp = Stamp(0, 0);
    static Stamp oldDepthStamp = Stamp(0, 0);
    bool rgb_data_ok = true;
    bool depth_data_ok = true;

    if (((colorStamp.getTime() - oldColorStamp.getTime()) > 0) == false)
    {
        rgb_data_ok=false;
        //return true;
    }
    else { oldColorStamp = colorStamp; }

    if (((depthStamp.getTime() - oldDepthStamp.getTime()) > 0) == false)
    {
        depth_data_ok=false;
        //return true;
    }
    else { oldDepthStamp = depthStamp; }

    // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.
    if (rgb_data_ok)
    {
        yarp::rosmsg::sensor_msgs::Image&      rColorImage     = publisherPort_color.prepare();
        yarp::rosmsg::sensor_msgs::CameraInfo& camInfoC        = publisherPort_colorCaminfo.prepare();
        yarp::rosmsg::TickTime                 cRosStamp       = colorStamp.getTime();
        yarp::dev::RGBDRosConversionUtils::deepCopyImages(colorImage, rColorImage, m_color_frame_id, cRosStamp, nodeSeq);
        publisherPort_color.setEnvelope(colorStamp);
        publisherPort_color.write();
        if (setCamInfo(camInfoC, m_color_frame_id, nodeSeq, COLOR_SENSOR))
        {
            if(forceInfoSync)
                {camInfoC.header.stamp = rColorImage.header.stamp;}
            publisherPort_colorCaminfo.setEnvelope(colorStamp);
            publisherPort_colorCaminfo.write();
        }
        else
        {
            yCWarning(RGBDSENSORNWSROS, "Missing color camera parameters... camera info messages will be not sent");
        }
    }
    if (depth_data_ok)
    {
        yarp::rosmsg::sensor_msgs::Image&      rDepthImage     = publisherPort_depth.prepare();
        yarp::rosmsg::sensor_msgs::CameraInfo& camInfoD        = publisherPort_depthCaminfo.prepare();
        yarp::rosmsg::TickTime                 dRosStamp       = depthStamp.getTime();
        yarp::dev::RGBDRosConversionUtils::deepCopyImages(depthImage, rDepthImage, m_depth_frame_id, dRosStamp, nodeSeq);
        publisherPort_depth.setEnvelope(depthStamp);
        publisherPort_depth.write();
        if (setCamInfo(camInfoD, m_depth_frame_id, nodeSeq, DEPTH_SENSOR))
        {
            if(forceInfoSync)
                {camInfoD.header.stamp = rDepthImage.header.stamp;}
            publisherPort_depthCaminfo.setEnvelope(depthStamp);
            publisherPort_depthCaminfo.write();
        }
        else
        {
            yCWarning(RGBDSENSORNWSROS, "Missing depth camera parameters... camera info messages will be not sent");
        }
    }

    nodeSeq++;

    return true;
}

void RgbdSensor_nws_ros::run()
{
    if (sensor_p!=nullptr)
    {
        static int i = 0;
        sensorStatus = sensor_p->getSensorStatus();
        switch (sensorStatus)
        {
            case(IRGBDSensor::RGBD_SENSOR_OK_IN_USE) :
            {
                if (!writeData()) {
                    yCError(RGBDSENSORNWSROS, "Image not captured.. check hardware configuration");
                }
                i = 0;
            }
            break;
            case(IRGBDSensor::RGBD_SENSOR_NOT_READY):
            {
                if(i < 1000) {
                    if((i % 30) == 0) {
                        yCInfo(RGBDSENSORNWSROS) << "Device not ready, waiting...";
                    }
                } else {
                    yCWarning(RGBDSENSORNWSROS) << "Device is taking too long to start..";
                }
                i++;
            }
            break;
            default:
            {
                if (verbose >= 1) {  // better not to print it every cycle anyway, too noisy
                    yCError(RGBDSENSORNWSROS, "%s: Sensor returned error", sensorId.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDSENSORNWSROS, "%s: Sensor interface is not valid", sensorId.c_str());
        }
    }
}
