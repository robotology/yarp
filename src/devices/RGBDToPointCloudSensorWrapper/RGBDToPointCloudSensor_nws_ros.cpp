/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDToPointCloudSensor_nws_ros.h"
#include <sstream>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include "rosPixelCode.h"
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
    // check period
    if (!config.check("period", "refresh period of the broadcasted values in s")) {
        if(verbose >= 3) {
            yCInfo(RGBDTOPOINTCLOUDSENSORNWSROS) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
        }
    }
    else {
        period = config.find("period").asFloat64();
    }

    // nodename parameter
    if (!config.check("node_name", "the name of the ros node")) {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "missing node_name parameter";
        return false;
    }
    nodeName = config.find("node_name").asString();


    // baseTopicName parameter
    if (!config.check("topic_name", "the name of the ros node")) {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "missing topic_name parameter, using default one";
        return false;
    }
    pointCloudTopicName = config.find("topic_name").asString();

    // frame_id parameter
    if (!config.check("frame_id", "the name of the ros node")) {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "missing frame_id parameter";
        return false;
    }
    frameId = config.find("frame_id").asString();

    // open topics here if needed
    m_node = new yarp::os::Node(nodeName);
    nodeSeq = 0;
    if (!publisherPort_pointCloud.topic(pointCloudTopicName))
    {
        yCError(RGBDTOPOINTCLOUDSENSORNWSROS) << "Unable to publish data on " << pointCloudTopicName.c_str() << " topic, check your yarp-ROS network configuration";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on through attachAll()
    if(config.check("subdevice")) {
        if(! openAndAttachSubDevice(config))
        {
            yCError(RGBDTOPOINTCLOUDSENSORNWSROS, "Error while opening subdevice");
            return false;
        }
        isSubdeviceOwned=true;
    } else {
        isSubdeviceOwned=false;
    }
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

/**
  * WrapperSingle interface
  */

bool RGBDToPointCloudSensor_nws_ros::detach()
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

    if ((colorStamp.getTime() - oldColorStamp.getTime()) <= 0)
    {
        rgb_data_ok=false;
        //return true;
    }
    else { oldColorStamp = colorStamp; }

    if ((depthStamp.getTime() - oldDepthStamp.getTime()) <= 0)
    {
        depth_data_ok=false;
        //return true;
    }
    else { oldDepthStamp = depthStamp; }
    bool intrinsic_ok = sensor_p->getRgbIntrinsicParam(propIntrinsic);


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
                PointCloud2Type& pc2Ros = publisherPort_pointCloud.prepare();
                // filling ros header
                yarp::rosmsg::std_msgs::Header headerRos;
                headerRos.clear();
                headerRos.seq = nodeSeq;
                headerRos.frame_id = frameId;
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

                publisherPort_pointCloud.write();
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
