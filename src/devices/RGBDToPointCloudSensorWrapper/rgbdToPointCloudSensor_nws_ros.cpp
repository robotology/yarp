/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "rgbdToPointCloudSensor_nws_ros.h"
#include <sstream>
#include <cstdio>
#include <cstring>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/GenericVocabs.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>
#include "rosPixelCode.h"
#include <RGBDRosConversionUtils.h>
#include <yarp/sig/PointCloudUtils.h>

using namespace RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RGBDTOPOINTCLOUDSENSORNWSROS, "yarp.devices.RgbdToPointCloudSensor_nws_ros")

RgbdToPointCloudSensor_nws_ros::RgbdToPointCloudSensor_nws_ros() :
    PeriodicThread(DEFAULT_THREAD_PERIOD),
    rosNode(nullptr),
    nodeSeq(0),
    period(DEFAULT_THREAD_PERIOD),
    sensor_p(nullptr),
    fgCtrl(nullptr),
    sensorStatus(IRGBDSensor::RGBD_SENSOR_NOT_READY),
    verbose(4),
    forceInfoSync(true),
    isSubdeviceOwned(false),
    subDeviceOwned(nullptr)
{
    m_depth_width = 0;
    m_depth_height = 0;
    m_pc_stepx = 1;
    m_pc_stepy = 1;
    //m_col_granularity = 10;
    m_ground_frame_id = "/ground_frame";
    m_camera_frame_id = "/depth_camera_frame";
}


/* destructor of the wrapper */
RgbdToPointCloudSensor_nws_ros::~RgbdToPointCloudSensor_nws_ros()
{
    close();
    sensor_p = nullptr;
    fgCtrl = nullptr;
}

/** Device driver interface */

bool RgbdToPointCloudSensor_nws_ros::open(yarp::os::Searchable &config)
{
    m_conf.fromString(config.toString());
    if(verbose >= 5)
        yCTrace(RGBDTOPOINTCLOUDSENSORNWSROS) << "\nParameters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Failed to open, check previous log for error messages.";
        return false;
    }

    setId("RGBDToPointCloudSensorWrapper for " + nodeName);

    if(!initialize_ROS(config))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << sensorId << "Error initializing ROS topic";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on through attachAll()
    if(isSubdeviceOwned)
    {
        if(! openAndAttachSubDevice(config))
        {
            yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "Error while opening subdevice");
            return false;
        }
    }
    else
    {
        if(!openDeferredAttach(config))
            return false;
    }

    return true;
}

bool RgbdToPointCloudSensor_nws_ros::fromConfig(yarp::os::Searchable &config)
{
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        if(verbose >= 3)
            yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
    }
    else
        period = config.find("period").asInt32() / 1000.0;

    //check if param exist and assign it to corresponding variable.. if it doesn't, initialize the variable with default value.
    unsigned int                    i;
    std::vector<param<string> >     rosStringParam;
    param<string>*                  prm;

    rosStringParam.emplace_back(nodeName,       nodeName_param          );
    rosStringParam.emplace_back(rosFrameId,     frameId_param           );
    rosStringParam.emplace_back(pointCloudTopicName, pointCloudTopicName_param    );

    for (i = 0; i < rosStringParam.size(); i++)
    {
        prm = &rosStringParam[i];
        if (!config.check(prm->parname))
        {
            if(verbose >= 3)
            {
                yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Missing " << prm->parname << "check your configuration file";
            }
            return false;
        }
        *(prm->var) = config.find(prm->parname).asString();
    }

    if (config.check("forceInfoSync"))
    {
        forceInfoSync = config.find("forceInfoSync").asBool();
    }

    if(config.check("subdevice")) {
        isSubdeviceOwned=true;
    } else {
        isSubdeviceOwned=false;
    }

    return true;
}

bool RgbdToPointCloudSensor_nws_ros::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RgbdToPointCloudSensor_nws_ros::openAndAttachSubDevice(Searchable& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(RGBDTOPOINTCLOUDSENSORNWSROS, "Opening IRGBDSensor subdevice");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "Opening IRGBDSensor subdevice... FAILED");
        return false;
    }
    isSubdeviceOwned = true;
    if(!attach(subDeviceOwned)) {
        return false;
    }

    return true;
}

bool RgbdToPointCloudSensor_nws_ros::close()
{
    yCTrace(RGBDTOPOINTCLOUDSENSORNWSROS, "Close");
    detachAll();

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

    if(rosNode!=nullptr)
    {
        rosNode->interrupt();
        delete rosNode;
        rosNode = nullptr;
    }

    return true;
}

/* Helper functions */

bool RgbdToPointCloudSensor_nws_ros::initialize_ROS(yarp::os::Searchable &params)
{
    // open topics here if needed
    rosNode = new yarp::os::Node(nodeName);
    nodeSeq = 0;
    if (!rosPublisherPort_color.topic(colorTopicName))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Unable to publish data on " << pointCloudTopicName.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }
    return true;
}

void RgbdToPointCloudSensor_nws_ros::setId(const std::string &id)
{
    sensorId=id;
}

std::string RgbdToPointCloudSensor_nws_ros::getId()
{
    return sensorId;
}

/**
  * IWrapper and IMultipleWrapper interfaces
  */
bool RgbdToPointCloudSensor_nws_ros::attachAll(const PolyDriverList &device2attach)
{
    // First implementation only accepts devices with both the interfaces Framegrabber and IDepthSensor,
    // on a second version maybe two different devices could be accepted, one for each interface.
    // Yet to be defined which and how parameters shall be used in this case ... using the name of the
    // interface to view could be a good initial guess.
    if (device2attach.size() != 1)
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;
    if(device2attach[0]->key == "IRGBDSensor")
    {
        yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Good name!";
    }
    else
    {
        yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Bad name!";
    }

    if (!Idevice2attach->isValid())
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(sensor_p);
    Idevice2attach->view(fgCtrl);
    if(!attach(sensor_p))
        return false;

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RgbdToPointCloudSensor_nws_ros::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning())
        yarp::os::PeriodicThread::stop();

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;

    sensor_p = nullptr;
    return true;
}

bool RgbdToPointCloudSensor_nws_ros::attach(yarp::dev::IRGBDSensor *s)
{
    if(s == nullptr)
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }
    sensor_p = s;

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RgbdToPointCloudSensor_nws_ros::attach(PolyDriver* poly)
{
    if(poly)
    {
        poly->view(sensor_p);
        poly->view(fgCtrl);
    }

    if(sensor_p == nullptr)
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RgbdToPointCloudSensor_nws_ros::detach()
{
    sensor_p = nullptr;
    return true;
}

/* IRateThread interface */

bool RgbdToPointCloudSensor_nws_ros::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RgbdToPointCloudSensor_nws_ros::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

//bool RgbdToPointCloudSensor_nws_ros::setCamInfo(yarp::rosmsg::sensor_msgs::CameraInfo& cameraInfo, const string& frame_id, const UInt& seq, const SensorType& sensorType)
//{
//    double phyF = 0.0;
//    double fx = 0.0;
//    double fy = 0.0;
//    double cx = 0.0;
//    double cy = 0.0;
//    double k1 = 0.0;
//    double k2 = 0.0;
//    double t1 = 0.0;
//    double t2 = 0.0;
//    double k3 = 0.0;
//    double stamp = 0.0;
//
//    string                  distModel, currentSensor;
//    UInt                    i;
//    Property                camData;
//    vector<param<double> >  parVector;
//    param<double>*          par;
//    bool                    ok;
//
//    currentSensor = sensorType == COLOR_SENSOR ? "Rgb" : "Depth";
//    ok            = sensorType == COLOR_SENSOR ? sensor_p->getRgbIntrinsicParam(camData) : sensor_p->getDepthIntrinsicParam(camData);
//
//    if (!ok)
//    {
//        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Unable to get intrinsic param from" << currentSensor << "sensor!";
//        return false;
//    }
//
//    if(!camData.check("distortionModel"))
//    {
//        yCWarning(RGBDTOPOINTCLOUDSENSORNWSROS) << "Missing distortion model";
//        return false;
//    }
//
//    distModel = camData.find("distortionModel").asString();
//    if (distModel != "plumb_bob")
//    {
//        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Distortion model not supported";
//        return false;
//    }
//
//    //std::vector<param<string> >     rosStringParam;
//    //rosStringParam.push_back(param<string>(nodeName, "asd"));
//
//    parVector.emplace_back(phyF,"physFocalLength");
//    parVector.emplace_back(fx,"focalLengthX");
//    parVector.emplace_back(fy,"focalLengthY");
//    parVector.emplace_back(cx,"principalPointX");
//    parVector.emplace_back(cy,"principalPointY");
//    parVector.emplace_back(k1,"k1");
//    parVector.emplace_back(k2,"k2");
//    parVector.emplace_back(t1,"t1");
//    parVector.emplace_back(t2,"t2");
//    parVector.emplace_back(k3,"k3");
//    parVector.emplace_back(stamp,"stamp");
//    for(i = 0; i < parVector.size(); i++)
//    {
//        par = &parVector[i];
//
//        if(!camData.check(par->parname))
//        {
//            yCWarning(RGBDTOPOINTCLOUDSENSORNWSROS) << "Driver has not the param:" << par->parname;
//            return false;
//        }
//        *par->var = camData.find(par->parname).asFloat64();
//    }
//
//    cameraInfo.header.frame_id    = frame_id;
//    cameraInfo.header.seq         = seq;
//    cameraInfo.header.stamp       = stamp;
//    cameraInfo.width              = sensorType == COLOR_SENSOR ? sensor_p->getRgbWidth() : sensor_p->getDepthWidth();
//    cameraInfo.height             = sensorType == COLOR_SENSOR ? sensor_p->getRgbHeight() : sensor_p->getDepthHeight();
//    cameraInfo.distortion_model   = distModel;
//
//    cameraInfo.D.resize(5);
//    cameraInfo.D[0] = k1;
//    cameraInfo.D[1] = k2;
//    cameraInfo.D[2] = t1;
//    cameraInfo.D[3] = t2;
//    cameraInfo.D[4] = k3;
//
//    cameraInfo.K.resize(9);
//    cameraInfo.K[0]  = fx;       cameraInfo.K[1] = 0;        cameraInfo.K[2] = cx;
//    cameraInfo.K[3]  = 0;        cameraInfo.K[4] = fy;       cameraInfo.K[5] = cy;
//    cameraInfo.K[6]  = 0;        cameraInfo.K[7] = 0;        cameraInfo.K[8] = 1;
//
//    /*
//     * ROS documentation on cameraInfo message:
//     * "Rectification matrix (stereo cameras only)
//     * A rotation matrix aligning the camera coordinate system to the ideal
//     * stereo image plane so that epipolar lines in both stereo images are
//     * parallel."
//     * useless in our case, it will be an identity matrix
//     */
//
//    cameraInfo.R.assign(9, 0);
//    cameraInfo.R.at(0) = cameraInfo.R.at(4) = cameraInfo.R.at(8) = 1;
//
//    cameraInfo.P.resize(12);
//    cameraInfo.P[0]  = fx;      cameraInfo.P[1] = 0;    cameraInfo.P[2]  = cx;  cameraInfo.P[3]  = 0;
//    cameraInfo.P[4]  = 0;       cameraInfo.P[5] = fy;   cameraInfo.P[6]  = cy;  cameraInfo.P[7]  = 0;
//    cameraInfo.P[8]  = 0;       cameraInfo.P[9] = 0;    cameraInfo.P[10] = 1;   cameraInfo.P[11] = 0;
//
//    cameraInfo.binning_x  = cameraInfo.binning_y = 0;
//    cameraInfo.roi.height = cameraInfo.roi.width = cameraInfo.roi.x_offset = cameraInfo.roi.y_offset = 0;
//    cameraInfo.roi.do_rectify = false;
//    return true;
//}

bool RgbdToPointCloudSensor_nws_ros::writeData()
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
    yarp::os::Property propIntrinsic;
    bool rgb_data_ok = true;
    bool depth_data_ok = true;
    bool intrinsic_ok = false;

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
    intrinsic_ok = sensor_p->getRgbIntrinsicParam(propIntrinsics);


    // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.
    if (rgb_data_ok) {
        if (depth_data_ok){
            if (intrinsic_ok) {
                yarp::sig::IntrinsicParams intrinsics(propIntrinsics);

                yarp::sig::PointCloud<yarp::sig::DataXYZRGBA> yarpCloud = yarp::sig::utils::depthRGBToPC<yarp::sig::DataXYZRGBA, yarp::sig::PixelRgb>(depthImage, colorImage, intrinsics);
                yarp::pcl::PointCloud<yarp::pcl::PointXYZRGBA> pclCloud;
                yarp::pcl::toPCL<yarp::sig::DataXYZRGBA, yarp::pcl::PointXYZRGBA>(yarpCloud, pclCloud);

                // conversion from yarp point cloud to pcl point cloud
                yarp::pcl::pclCloud.points.resize(yarpCloud.size());
                pclCloud.width = yarpCloud.width();
                pclCloud.height = yarpCloud.height();
                pclCloud.is_dense = yarpCloud.isDense();
                memcpy((char*)&pclCloud.points.at(0), yarpCloud.getRawData(), yarpCloud.dataSizeBytes());

                PointCloud2Type& rPointCloud = rosPublisherPort_pointCloud.prepare();
                yarp::rosmsg::TickTime pcTickTime = colorStamp.getTime();
                rosPublisherPort_pointCloud.setEnvelope(colorStamp);
                rosPublisherPort_pointCloud.write();
            }
        }
    }

    nodeSeq++;

    return true;
}

void RgbdToPointCloudSensor_nws_ros::run()
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
                    yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "Image not captured.. check hardware configuration");
                }
                i = 0;
                
            }
            break;
            case(IRGBDSensor::RGBD_SENSOR_NOT_READY):
            {
                if(i < 1000) {
                    if((i % 30) == 0) {
                        yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Device not ready, waiting...";
                    }
                } else {
                    yCWarning(RGBDTOPOINTCLOUDSENSORNWSROS) << "Device is taking too long to start..";
                }
                i++;
            }
            break;
            default:
            {
                if (verbose >= 1) {  // better not to print it every cycle anyway, too noisy
                    yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "%s: Sensor returned error", sensorId.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "%s: Sensor interface is not valid", sensorId.c_str());
        }
    }
}
