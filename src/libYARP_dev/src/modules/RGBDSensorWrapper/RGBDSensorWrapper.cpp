/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <Alberto.Cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include "RGBDSensorWrapper.h"
#include <yarpRosHelper.h>
#include "rosPixelCode.h"

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


// needed for the driver factory.
yarp::dev::DriverCreator *createRGBDSensorWrapper() {
    return new DriverCreatorOf<yarp::dev::RGBDSensorWrapper>("RGBDSensorWrapper", "RGBDSensorWrapper", "yarp::dev::RGBDSensorWrapper");
}

RGBDSensorWrapper::RGBDSensorWrapper(): RateThread(DEFAULT_THREAD_PERIOD),
                                        rate(DEFAULT_THREAD_PERIOD)
{
    sensor_p         = NULL;
    use_YARP         = true;
    use_ROS          = false;
    subDeviceOwned   = NULL;
    isSubdeviceOwned = false;
    verbose          = 4;
    sensorStatus     = IRGBDSensor::RGBD_SENSOR_NOT_READY;
}

RGBDSensorWrapper::~RGBDSensorWrapper()
{
    threadRelease();
    sensor_p = NULL;
}

/** Device driver interface */

bool RGBDSensorWrapper::open(yarp::os::Searchable &config)
{
//     DeviceResponder::makeUsage();
//     addUsage("[set] [bri] $fBrightness", "set brightness");
//     addUsage("[set] [expo] $fExposure", "set exposure");
//
    m_conf.fromString(config.toString());
    if(verbose >= 5)
        yTrace() << "\nParameters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yError() << "Device RGBDSensorWrapper failed to open, check previous log for error messsages.\n";
        return false;
    }

     setId("RGBDSensorWrapper for " + depthFrame_StreamingPort_Name);

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }

    if(!initialize_ROS(config) )
    {
        yError() << sensorId << "Error initializing ROS topic";
        return false;
    }
    return true;
}

bool RGBDSensorWrapper::fromConfig(yarp::os::Searchable &config)
{
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        if(verbose >= 3)
            yInfo() << "RGBDSensorWrapper: using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "ms";
    }
    else
        rate = config.find("period").asInt();

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        if(verbose >= 3)
            yInfo() << "RGBDSensorWrapper: ROS configuration paramters are not set, skipping ROS topic initialization.";
        use_ROS  = false;
        use_YARP = true;
    }
    else
    {
        //if(verbose >= 2)
        //    yWarning() << "RGBDSensorWrapper: ROS topic support is not yet implemented";
        Value* temp;
        string confUseRos;
        
        rosGroup.check("use_ROS", temp);
        confUseRos = temp->asString();

        if (confUseRos == "true" || confUseRos == "only")
        {
            use_ROS  = true;
            use_YARP = confUseRos == "true" ? true : false;
        }
        else
        {
            use_ROS = false;
            if (verbose >= 3 || confUseRos == "false")
            {
                yInfo("'use_ROS' value not understood.. skipping ROS topic initialization");
            }
        }
    }

    if (use_ROS)
    {
        //check if param exist and assign it to corresponding variable.. if it doesn't, initialize the variable with default value.
        unsigned int                    i;
        std::vector<param<string> >     rosStringParam;
        param<string>*                  prm;
        
        rosStringParam.push_back(param<string>(nodeName,       NODENAMEPARAM));
        rosStringParam.push_back(param<string>(rosFrameId,     FRAMEIDPARAM));
        rosStringParam.push_back(param<string>(colorTopicName, CLRTOPICNAMENAMEPARAM));
        rosStringParam.push_back(param<string>(depthTopicName, DPHTOPICNAMENAMEPARAM));
        rosStringParam.push_back(param<string>(cInfoTopicName, CLRINFOTOPICNAMEPARAM));
        rosStringParam.push_back(param<string>(dInfoTopicName, DPHINFOTOPICNAMEPARAM));
        
        for (i = 0; i < rosStringParam.size(); i++)
        {
            prm = &rosStringParam[i];
            if (!rosGroup.check(prm->parname))
            {
                if(verbose >= 3)
                {
                    yError() << "missing " << prm->parname << "check your configuration file, not using ROS";
                }
                use_ROS = false;
                return false;
            }
            *(prm->var) = rosGroup.find(prm->parname).asString().c_str();
        }
    }

    if(use_YARP)
    {
        if (!config.check("imagePort", "full name of the port for streaming color image"))
        {
            yError() << "RGBDSensorWrapper: missing 'imagePort' parameter. Check you configuration file; it must be like:";
            yError() << "   imagePort:         full name of the port, e.g. /robotName/image_camera/";
            return false;
        }
        else
        {
            colorFrame_StreamingPort_Name = config.find("imagePort").asString().c_str();
            colorFrame_rpcPort_Name       = colorFrame_StreamingPort_Name + "/rpc:i";
        }

        if (!config.check("depthPort", "full name of the port for streaming depth image"))
        {
            yError() << "RGBDSensorWrapper: missing 'depthPort' parameter. Check you configuration file; it must be like:";
            yError() << "   depthPort:         full name of the port, e.g. /robotName/depth_camera/";
            return false;
        }
        else
        {
            depthFrame_StreamingPort_Name  = config.find("depthPort").asString().c_str();
            depthFrame_rpcPort_Name = depthFrame_StreamingPort_Name + "/rpc:i";
        }
    }
    
    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(config.check("subdevice"))
    {
        isSubdeviceOwned=true;
        if(! openAndAttachSubDevice(config))
        {
            yError("ControlBoardWrapper: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        isSubdeviceOwned=false;
        if(!openDeferredAttach(config))
            return false;
    }
    return true;
}

bool RGBDSensorWrapper::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RGBDSensorWrapper::openAndAttachSubDevice(Searchable& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString().c_str());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if error occour during open, quit here.
    yDebug("opening IRGBDSensor subdevice\n");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yError("opening controlBoardWrapper2 subdevice... FAILED\n");
        return false;
    }
    isSubdeviceOwned = true;
    if(!attach(subDeviceOwned))
        return false;

    RateThread::setRate(rate);
    RateThread::start();
    return true;
}

bool RGBDSensorWrapper::close()
{
    yTrace("RGBDSensorWrapper::Close");
    detachAll();

    // close subdevice if it was created inside the open (--subdevice option)
    if(isSubdeviceOwned)
    {
        if(subDeviceOwned)
            subDeviceOwned->close();
        subDeviceOwned = NULL;
        sensor_p = NULL;
        isSubdeviceOwned = false;
    }

    // Closing port
    if(use_YARP)
    {
        colorFrame_rpcPort.interrupt();
        depthFrame_rpcPort.interrupt();
        colorFrame_StreamingPort.interrupt();
        depthFrame_StreamingPort.interrupt();

        colorFrame_rpcPort.close();
        depthFrame_rpcPort.close();
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    // Closing ROS topic
    if(use_ROS)
    {
        // bla bla bla
    }
    //
    return true;
}

/* Helper functions */

bool RGBDSensorWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    colorFrame_StreamingPort.open(colorFrame_StreamingPort_Name.c_str());
    colorFrame_rpcPort.open(colorFrame_rpcPort_Name.c_str() );
    colorFrame_rpcPort.setReader(RPC_parser);

    depthFrame_StreamingPort.open(depthFrame_StreamingPort_Name.c_str());
    depthFrame_rpcPort.open(depthFrame_rpcPort_Name.c_str() );
    depthFrame_rpcPort.setReader(RPC_parser);
    return true;
}

bool RGBDSensorWrapper::initialize_ROS(yarp::os::Searchable &params)
{
    // open topics here if needed
    rosNode = new yarp::os::Node(nodeName);
    nodeSeq = 0;
    if (!rosPublisherPort_color.topic(colorTopicName))
    {
        yError() << " Unable to publish data on " << colorTopicName.c_str() << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    if (!rosPublisherPort_depth.topic(depthTopicName))
    {
        yError() << " Unable to publish data on " << depthTopicName.c_str() << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    if (!rosPublisherPort_colorCaminfo.topic(cInfoTopicName))
    {
        yError() << " Unable to publish data on " << cInfoTopicName.c_str() << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    if (!rosPublisherPort_depthCaminfo.topic(dInfoTopicName))
    {
        yError() << " Unable to publish data on " << dInfoTopicName.c_str() << " topic, check your yarp-ROS network configuration\n";
        return false;
    }
    return true;
}

void RGBDSensorWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string RGBDSensorWrapper::getId()
{
    return sensorId;
}

/**
  * IWrapper and IMultipleWrapper interfaces
  */
bool RGBDSensorWrapper::attachAll(const PolyDriverList &device2attach)
{
    // First implementation only accepts devices with both the interfaces Framegrabber and IDepthSensor,
    // on a second version maybe two different devices could be accepted, one for each interface.
    // Yet to be defined which and how parameters shall be used in this case ... using the name of the
    // interface to view could be a good initial guess.
    if (device2attach.size() != 1)
    {
        yError("RGBDSensorWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;
    if(device2attach[0]->key == "IRGBDSensor")
    {
        yInfo() << "RGBDSensorWrapper: Good name Dude!";
    }
    else
    {
        yInfo() << "RGBDSensorWrapper: Bad name Dude!!";
    }

    if (!Idevice2attach->isValid())
    {
        yError() << "RGBDSensorWrapper: Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(sensor_p);
    if(!attach(sensor_p))
        return false;

    RateThread::setRate(rate);
    RateThread::start();

    return true;
}

bool RGBDSensorWrapper::detachAll()
{
    if (yarp::os::RateThread::isRunning())
        yarp::os::RateThread::stop();

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;

    sensor_p = NULL;
    return true;
}

bool RGBDSensorWrapper::attach(yarp::dev::IRGBDSensor *s)
{
    if(s == NULL)
    {
        yError() << "RGBDSensorWrapper: attached device has no valid IRGBDSensor interface.";
        return false;
    }
    sensor_p = s;
    return true;
}

bool RGBDSensorWrapper::attach(PolyDriver* poly)
{
    if(poly)
        poly->view(sensor_p);

    if(sensor_p == NULL)
    {
        yError() << "RGBDSensorWrapper: attached device has no valid IRGBDSensor interface.";
        return false;
    }
    return true;
}

bool RGBDSensorWrapper::detach()
{
    sensor_p = NULL;
    return true;
}

/* IRateThread interface */

bool RGBDSensorWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
//     int action = in.get(0).asVocab();
//     int inter  = in.get(1).asVocab();
    bool ret = false;

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL) {
        out.write(*returnToSender);
    }
    return true;
}

bool RGBDSensorWrapper::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RGBDSensorWrapper::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

string RGBDSensorWrapper::yarp2RosPixelCode(int code)
{
    switch(code)
    {
        case VOCAB_PIXEL_BGR:
            return BGR8;
        case VOCAB_PIXEL_BGRA:
            return BGRA8;
        case VOCAB_PIXEL_ENCODING_BAYER_BGGR16:
            return BAYER_BGGR16;
        case VOCAB_PIXEL_ENCODING_BAYER_BGGR8:
            return BAYER_BGGR8;
        case VOCAB_PIXEL_ENCODING_BAYER_GBRG16:
            return BAYER_GBRG16;
        case VOCAB_PIXEL_ENCODING_BAYER_GBRG8:
            return BAYER_GBRG8;
        case VOCAB_PIXEL_ENCODING_BAYER_GRBG16:
            return BAYER_GRBG16;
        case VOCAB_PIXEL_ENCODING_BAYER_GRBG8:
            return BAYER_GRBG8;
        case VOCAB_PIXEL_ENCODING_BAYER_RGGB16:
            return BAYER_RGGB16;
        case VOCAB_PIXEL_ENCODING_BAYER_RGGB8:
            return BAYER_RGGB8;
        case VOCAB_PIXEL_MONO:
            return MONO8;
        case VOCAB_PIXEL_MONO16:
            return MONO16;
        case VOCAB_PIXEL_RGB:
            return RGB8;
        case VOCAB_PIXEL_RGBA:
            return RGBA8;
        case VOCAB_PIXEL_MONO_FLOAT:
            return TYPE_32FC1;
        default:
            return RGB8;
    }
}

void RGBDSensorWrapper::shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest)
{
    dest.setPixelCode(src.getPixelCode());
    dest.setPixelSize(src.getPixelSize());
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}



void RGBDSensorWrapper::deepCopyImages
(
    const yarp::sig::FlexImage& src, 
    sensor_msgs_Image&          dest, 
    const string&               frame_id, 
    const TickTime&             timeStamp, 
    const unsigned int          seq
)
{
    dest.data.resize(src.getRawImageSize());
    dest.width           = src.width();
    dest.height          = src.height();
    memcpy(dest.data.data(), src.getRawImage(), src.getRawImageSize());
    dest.encoding        = yarp2RosPixelCode(src.getPixelCode());
    dest.step            = src.getRowSize();
    dest.header.frame_id = frame_id;
    dest.header.stamp    = timeStamp;
    dest.header.seq      = seq;
    dest.is_bigendian    = 0;
}

bool RGBDSensorWrapper::setCamInfo
(
    sensor_msgs_CameraInfo& cameraInfo, 
    const string&           frame_id, 
    const unsigned int&     seq
)
{
    double                      fx, fy, cx, cy, tx, ty, k1, k2, t1, t2, k3, stamp;
    string                      distModel;
    unsigned int                i;
    Property                    camData;
    vector<param<double> >      parVector;
    param<double>*              par;
    
    sensor_p->getDeviceInfo(camData);
    
    if(!camData.check("distortionModel"))
    {
        return false;
    }
    
    distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob")
    {
        yError("distortion model not supported");
        return false;
    }
    
    if(!camData.check("retificationMatrix"))
    {
        return false;
    }
    Bottle& retificationMatrix = *camData.find("retificationMatrix").asList();
    

    //std::vector<param<string> >     rosStringParam;
    //rosStringParam.push_back(param<string>(nodeName, "asd"));
    
    parVector.push_back(param<double>(fx,"focalLengthX"));
    parVector.push_back(param<double>(fy,"focalLengthY"));
    parVector.push_back(param<double>(cx,"principalPointX"));
    parVector.push_back(param<double>(cy,"principalPointY"));
    parVector.push_back(param<double>(k1,"k1"));
    parVector.push_back(param<double>(k2,"k2"));
    parVector.push_back(param<double>(t1,"t1"));
    parVector.push_back(param<double>(t2,"t2"));
    parVector.push_back(param<double>(k3,"k3"));
    parVector.push_back(param<double>(tx,"tangentialPointX"));
    parVector.push_back(param<double>(ty,"tangentialPointY"));
    parVector.push_back(param<double>(stamp,"stamp"));
    for(i = 0; i < parVector.size(); i++)
    {
        par = &parVector[i];
        
        if(!camData.check(par->parname))
        {
            return false;
        }
        *par->var = camData.find(par->parname).asDouble();
    }
    
    cameraInfo.header.frame_id    = frame_id;
    cameraInfo.header.seq         = seq;
    cameraInfo.header.stamp       = normalizeSecNSec(stamp);
    cameraInfo.width              = sensor_p->width();
    cameraInfo.height             = sensor_p->height();
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
    
    //retification matrix
    
    if (retificationMatrix.size() == 9)// 3X3 matrix;
    {
        cameraInfo.R.resize(9);
        for (i = 0; i < cameraInfo.R.size(); i++)
        {
            cameraInfo.R[i] = retificationMatrix.get(i).asDouble();
        }
    }
    else
    {
        return false;
    }
    
    
    cameraInfo.P.resize(12);
    cameraInfo.P[0]  = fx;      cameraInfo.P[1] = 0;    cameraInfo.P[2]  = cx;  cameraInfo.P[3]  = tx;
    cameraInfo.P[4]  = 0;       cameraInfo.P[5] = fy;   cameraInfo.P[6]  = cy;  cameraInfo.P[7]  = ty;
    cameraInfo.P[8]  = 0;       cameraInfo.P[9] = 0;    cameraInfo.P[10] = 1;   cameraInfo.P[11] = 0;
    
    cameraInfo.binning_x = cameraInfo.binning_y = 0;
    cameraInfo.roi.height = cameraInfo.roi.width = cameraInfo.roi.x_offset = cameraInfo.roi.y_offset = 0;
    cameraInfo.roi.do_rectify = false;
    return true;
}

bool RGBDSensorWrapper::writeData()
{
    yarp::sig::FlexImage colorImage;
    yarp::sig::FlexImage depthImage;
    
    //colorImage.setPixelCode(VOCAB_PIXEL_RGB);
    //             depthImage.setPixelCode(VOCAB_PIXEL_MONO_FLOAT);

    //             colorImage.resize(hDim, vDim);  // Has this to be done each time? If size is the same what it does?
    //             depthImage.resize(hDim, vDim);
    if (!sensor_p->getRGBD_Frames(colorImage, depthImage, &colorStamp, &depthStamp))
    {
        return false;
    }

    if (use_YARP)
    {
        yarp::sig::FlexImage& yColorImage = colorFrame_StreamingPort.prepare();
        yarp::sig::FlexImage& yDepthImage = depthFrame_StreamingPort.prepare();

        shallowCopyImages(colorImage, yColorImage);
        shallowCopyImages(depthImage, yDepthImage);
        // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.

        colorFrame_StreamingPort.setEnvelope(colorStamp);
        colorFrame_StreamingPort.write();

        depthFrame_StreamingPort.setEnvelope(depthStamp);
        depthFrame_StreamingPort.write();

    }
    if (use_ROS)
    {
        sensor_msgs_Image&      rColorImage     = rosPublisherPort_color.prepare();
        sensor_msgs_Image&      rDepthImage     = rosPublisherPort_depth.prepare();
        sensor_msgs_CameraInfo& camInfoC        = rosPublisherPort_colorCaminfo.prepare();
        sensor_msgs_CameraInfo& camInfoD        = rosPublisherPort_depthCaminfo.prepare();
        TickTime                cRosStamp, dRosStamp;
        
        cRosStamp = normalizeSecNSec(colorStamp.getTime());
        dRosStamp = normalizeSecNSec(depthStamp.getTime());
        
        deepCopyImages(colorImage, rColorImage, rosFrameId, cRosStamp, nodeSeq);
        deepCopyImages(depthImage, rDepthImage, rosFrameId, dRosStamp, nodeSeq);
        // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.

        rosPublisherPort_color.setEnvelope(colorStamp);
        rosPublisherPort_color.write();

        rosPublisherPort_depth.setEnvelope(depthStamp);
        rosPublisherPort_depth.write();

        if (setCamInfo(camInfoC, rosFrameId, nodeSeq))
        {
            rosPublisherPort_colorCaminfo.setEnvelope(colorStamp);
            rosPublisherPort_colorCaminfo.write();
        }
        else
        {
            yWarning("missing color camera parameters... camera info messages will be not sended");
        }
        if (setCamInfo(camInfoD, rosFrameId, nodeSeq))
        {
            rosPublisherPort_depthCaminfo.setEnvelope(depthStamp);
            rosPublisherPort_depthCaminfo.write();
        }
        else
        {
            yWarning("missing depth camera parameters... camera info messages will be not sended");
        }

        nodeSeq++;
    }
    return true;
}

void RGBDSensorWrapper::run()
{
    if (sensor_p!=0)
    {
        sensor_p->getRGBDSensor_Status(&sensorStatus);
        switch (sensorStatus)
        {
            case(IRGBDSensor::RGBD_SENSOR_OK_IN_USE) :
            {
                if (!writeData())
                    yError("Image not captured.. check hardware configuration");
                break;
            }
            default:
            {
                if (verbose >= 1)   // better not to print it every cycle anyway, too noisy
                    yError("RGBDSensorWrapper: %s: Sensor returned error", sensorId.c_str());
            }
        }
    }
    else
    {
        if(verbose >= 6)
            yError("RGBDSensorWrapper: %s: Sensor interface is not valid", sensorId.c_str());
    }
}
