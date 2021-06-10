/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RGBDToPointCloudSensor_nws_ros.h"
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
#include <yarp/rosmsg/std_msgs/Header.h>
#include <yarp/rosmsg/sensor_msgs/PointField.h>

using namespace RGBDToPointCloudImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RGBDTOPOINTCLOUDSENSORNWSROS, "yarp.devices.RGBDToPointCloudSensor_nws_ros");

RGBDToPointCloudSensor_nws_ros::RGBDToPointCloudSensor_nws_ros() :
    PeriodicThread(DEFAULT_THREAD_PERIOD)
{

}


/* destructor of the wrapper */
RGBDToPointCloudSensor_nws_ros::~RGBDToPointCloudSensor_nws_ros()
{
    close();
}

/** Device driver interface */

bool RGBDToPointCloudSensor_nws_ros::open(yarp::os::Searchable &config)
{
    m_conf.fromString(config.toString());
    if(verbose >= 5)
    {
        yCTrace(RGBDTOPOINTCLOUDSENSORNWSROS) << "\nParameters are: \n" << config.toString();
    }

    if(!fromConfig(config))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Failed to open, check previous log for error messages.";
        return false;
    }

    if(!initialize_ROS(config))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << nodeName << "Error initializing ROS topic";
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
        {
            return false;
        }
    }

    return true;
}

bool RGBDToPointCloudSensor_nws_ros::fromConfig(yarp::os::Searchable &config)
{
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        if(verbose >= 3)
        {
            yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
        }

    }
    else
    {
        period = config.find("period").asFloat64();
    }

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

bool RGBDToPointCloudSensor_nws_ros::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RGBDToPointCloudSensor_nws_ros::openAndAttachSubDevice(Searchable& prop)
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

bool RGBDToPointCloudSensor_nws_ros::close()
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

bool RGBDToPointCloudSensor_nws_ros::initialize_ROS(yarp::os::Searchable &params)
{
    // open topics here if needed
    rosNode = new yarp::os::Node(nodeName);
    nodeSeq = 0;
    if (!rosPublisherPort_pointCloud.topic(pointCloudTopicName))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Unable to publish data on " << pointCloudTopicName.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }
    return true;
}

/**
  * IWrapper and IMultipleWrapper interfaces
  */
bool RGBDToPointCloudSensor_nws_ros::attachAll(const PolyDriverList &device2attach)
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

    if (!Idevice2attach->isValid())
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(sensor_p);
    Idevice2attach->view(fgCtrl);
    if(!attach(sensor_p))
    {
        return false;
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RGBDToPointCloudSensor_nws_ros::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning())
    {
        yarp::os::PeriodicThread::stop();
    }

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
    {
        return false;
    }
    sensor_p = nullptr;
    return true;
}

bool RGBDToPointCloudSensor_nws_ros::attach(yarp::dev::IRGBDSensor *s)
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

bool RGBDToPointCloudSensor_nws_ros::attach(PolyDriver* poly)
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

bool RGBDToPointCloudSensor_nws_ros::detach()
{
    sensor_p = nullptr;
    return true;
}

/* IRateThread interface */

bool RGBDToPointCloudSensor_nws_ros::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RGBDToPointCloudSensor_nws_ros::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}


bool RGBDToPointCloudSensor_nws_ros::writeData()
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
    intrinsic_ok = sensor_p->getRgbIntrinsicParam(propIntrinsic);


    // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.
    if (rgb_data_ok)
    {
        if (depth_data_ok)
        {
            if (intrinsic_ok)
            {
                yarp::sig::IntrinsicParams intrinsics(propIntrinsic);
                yarp::sig::ImageOf<yarp::sig::PixelRgb> colorImagePixelRGB;
                colorImagePixelRGB.setExternal(colorImage.getRawImage(), colorImage.width(), colorImage.height());
                // create point cloud in yarp format
                yarp::sig::PointCloud<yarp::sig::DataXYZRGBA> yarpCloud = yarp::sig::utils::depthRgbToPC<yarp::sig::DataXYZRGBA,
                                                                                                         yarp::sig::PixelRgb>(depthImage,
                                                                                                                              colorImagePixelRGB,
                                                                                                                              intrinsics,
                                                                                                                              yarp::sig::utils::OrganizationType::Unorganized);
                PointCloud2Type& pc2Ros = rosPublisherPort_pointCloud.prepare();
                // filling ros header
                yarp::rosmsg::std_msgs::Header headerRos;
                headerRos.clear();
                headerRos.seq = nodeSeq;
                headerRos.frame_id = rosFrameId;
                headerRos.stamp = depthStamp.getTime();

                // filling ros point field
                std::vector<yarp::rosmsg::sensor_msgs::PointField> pointFieldRos;
                pointFieldRos.push_back(yarp::rosmsg::sensor_msgs::PointField());
                pointFieldRos.push_back(yarp::rosmsg::sensor_msgs::PointField());
                pointFieldRos.push_back(yarp::rosmsg::sensor_msgs::PointField());
                pointFieldRos.push_back(yarp::rosmsg::sensor_msgs::PointField());
                pointFieldRos[0].name = "x";
                pointFieldRos[0].offset = 0;
                pointFieldRos[0].datatype = 7;
                pointFieldRos[0].count = 1;
                pointFieldRos[1].name = "y";
                pointFieldRos[1].offset = 4;
                pointFieldRos[1].datatype = 7;
                pointFieldRos[1].count = 1;
                pointFieldRos[2].name = "z";
                pointFieldRos[2].offset = 8;
                pointFieldRos[2].datatype = 7;
                pointFieldRos[2].count = 1;
                pointFieldRos[3].name = "rgb";
                pointFieldRos[3].offset = 16;
                pointFieldRos[3].datatype = 7;
                pointFieldRos[3].count = 1;
                pc2Ros.fields = pointFieldRos;

                vector<unsigned char> vec(yarpCloud.getRawData(), yarpCloud.getRawData() + yarpCloud.dataSizeBytes() );
                pc2Ros.data = vec;
                pc2Ros.header = headerRos;
                pc2Ros.width = yarpCloud.width() * yarpCloud.height();
                pc2Ros.height = 1;
                pc2Ros.is_dense = yarpCloud.isDense();

                pc2Ros.point_step = sizeof (yarp::sig::DataXYZRGBA);
                pc2Ros.row_step   = static_cast<std::uint32_t> (sizeof (yarp::sig::DataXYZRGBA) * pc2Ros.width);

                rosPublisherPort_pointCloud.write();
            }
        }
    }

    nodeSeq++;

    return true;
}

void RGBDToPointCloudSensor_nws_ros::run()
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
                    yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "%s: Sensor returned error", nodeName.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "%s: Sensor interface is not valid", nodeName.c_str());
        }
    }
}
