/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <Alberto.Cardellino@iit.it>
 *          Andrea Ruzzenenti   <Andrea.Ruzzenenti@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <stdio.h>
#include <string.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include "RGBDSensorWrapper.h"
#include <yarpRosHelper.h>
#include "rosPixelCode.h"

using namespace yarp::dev::RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


#define RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR 1
#define RGBD_INTERFACE_PROTOCOL_VERSION_MINOR 0

// needed for the driver factory.
yarp::dev::DriverCreator *createRGBDSensorWrapper() {
    return new DriverCreatorOf<yarp::dev::RGBDSensorWrapper>("RGBDSensorWrapper", "RGBDSensorWrapper", "yarp::dev::RGBDSensorWrapper");
}

RGBDSensorParser::RGBDSensorParser() : iRGBDSensor(YARP_NULLPTR) {};

bool RGBDSensorParser::configure(IRGBDSensor *interface)
{
    bool ret;
    iRGBDSensor = interface;
    ret  = rgbParser.configure(interface);
    ret &= depthParser.configure(interface);
    return ret;
}

bool RGBDSensorParser::configure(IRgbVisualParams *rgbInterface, IDepthVisualParams* depthInterface)
{
    bool ret = rgbParser.configure(rgbInterface);
    ret &= depthParser.configure(depthInterface);
    return ret;
}

bool RGBDSensorParser::respond(const Bottle& cmd, Bottle& response)
{
    bool ret = false;
    int interfaceType = cmd.get(0).asVocab();

    response.clear();
    switch(interfaceType)
    {
        case VOCAB_RGB_VISUAL_PARAMS:
        {
            // forwarding to the proper parser.
            ret = rgbParser.respond(cmd, response);
        }
        break;

        case VOCAB_DEPTH_VISUAL_PARAMS:
        {
            // forwarding to the proper parser.
            ret = depthParser.respond(cmd, response);
        }
        break;

        case VOCAB_RGBD_SENSOR:
        {
            switch (cmd.get(1).asVocab())
            {
                case VOCAB_GET:
                {
                    switch(cmd.get(2).asVocab())
                    {
                        case VOCAB_EXTRINSIC_PARAM:
                        {
                            yarp::sig::Matrix params;
                            ret = iRGBDSensor->getExtrinsicParam(params);
                            if(ret)
                            {
                                yarp::os::Bottle params_b;
                                response.addVocab(VOCAB_RGBD_SENSOR);
                                response.addVocab(VOCAB_EXTRINSIC_PARAM);
                                response.addVocab(VOCAB_IS);
                                ret &= Property::copyPortable(params, params_b);  // will it really work??
                                response.append(params_b);
                            }
                            else
                                response.addVocab(VOCAB_FAILED);
                        }
                        break;

                        case VOCAB_ERROR_MSG:
                        {
                            response.addVocab(VOCAB_RGBD_SENSOR);
                            response.addVocab(VOCAB_ERROR_MSG);
                            response.addVocab(VOCAB_IS);
                            response.addString(iRGBDSensor->getLastErrorMsg());
                            ret = true;
                        }
                        break;

                        case VOCAB_RGBD_PROTOCOL_VERSION:
                        {
                            response.addVocab(VOCAB_RGBD_SENSOR);
                            response.addVocab(VOCAB_RGBD_PROTOCOL_VERSION);
                            response.addVocab(VOCAB_IS);
                            response.addInt(RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR);
                            response.addInt(RGBD_INTERFACE_PROTOCOL_VERSION_MINOR);
                        }
                        break;

                        case VOCAB_STATUS:
                        {
                            response.addVocab(VOCAB_RGBD_SENSOR);
                            response.addVocab(VOCAB_STATUS);
                            response.addVocab(VOCAB_IS);
                            response.addInt(iRGBDSensor->getSensorStatus());
                        }
                        break;

                        default:
                        {
                            yError() << "RGBDSensor interface parser received an unknown GET command. Command is " << cmd.toString();
                            response.addVocab(VOCAB_FAILED);
                        }
                        break;
                    }
                }
                break;

                case VOCAB_SET:
                {
                    yError() << "RGBDSensor interface parser received an unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                }
                break;
            }
        }
        break;

        default:
        {
            yError() << "RGBD sensor wrapper received a command for a wrong interface " << yarp::os::Vocab::decode(interfaceType);
            ret = false;
        }
        break;
    }
    return ret;
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
        
        rosStringParam.push_back(param<string>(nodeName,       nodeName_param          ));
        rosStringParam.push_back(param<string>(rosFrameId,     frameId_param           ));
        rosStringParam.push_back(param<string>(colorTopicName, colorTopicName_param    ));
        rosStringParam.push_back(param<string>(depthTopicName, depthTopicName_param    ));
        rosStringParam.push_back(param<string>(cInfoTopicName, depthInfoTopicName_param));
        rosStringParam.push_back(param<string>(dInfoTopicName, colorInfoTopicName_param));
        
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
        yarp::os::ConstString rootName;
        rootName = config.check("name",Value("/"), "starting '/' if needed.").asString().c_str();

        if (!config.check("name", "Prefix name of the ports opened by the RGBD wrapper."))
        {
            yError() << "RGBDSensorWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
            yError() << "   name:         Prefix name of the ports opened by the RGBD wrapper, e.g. /robotName/RGBD";
            return false;
        }
        else
        {
            rootName = config.find("name").asString().c_str();
            rpcPort_Name  = rootName + "/rpc:i";
            colorFrame_StreamingPort_Name = rootName + "/rgbImage:o";
            depthFrame_StreamingPort_Name = rootName + "/depthImage:o";
        }
    }
    
    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(config.check("subdevice"))
    {
        isSubdeviceOwned=true;
        if(! openAndAttachSubDevice(config))
        {
            yError("RGBDSensorWrapper: error while opening subdevice\n");
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

    // Configuring parsers
    IRgbVisualParams * rgbVis_p;
    IDepthVisualParams * depthVis_p;

    subDeviceOwned->view(rgbVis_p);
    subDeviceOwned->view(depthVis_p);

    if(!parser.configure(sensor_p) )
    {
        yError() << "RGBD wrapper: error configuring interfaces for parsers";
        return false;
    }
    /*
    bool conf = rgbParser.configure(rgbVis_p);
    conf &= depthParser.configure(depthVis_p);

    if(!conf)
    {
        yError() << "RGBD wrapper: error configuring interfaces for parsers";
        return false;
    }
    */

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
        rpcPort.interrupt();
        colorFrame_StreamingPort.interrupt();
        depthFrame_StreamingPort.interrupt();

        rpcPort.close();
        colorFrame_StreamingPort.close();
        depthFrame_StreamingPort.close();
    }

    if(rosNode!=NULL)
    {
        rosNode->interrupt();
        delete rosNode;
        rosNode = NULL;
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
    // Open ports
    bool bRet;
    bRet = true;
    if(!rpcPort.open(rpcPort_Name.c_str()))
    {
        yError() << "RGBDSensorWrapper: unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }
    rpcPort.setReader(parser);

    if(!colorFrame_StreamingPort.open(colorFrame_StreamingPort_Name.c_str()))
    {
        yError() << "RGBDSensorWrapper: unable to open color streaming Port" << colorFrame_StreamingPort_Name.c_str();
        bRet = false;
    }
    if(!depthFrame_StreamingPort.open(depthFrame_StreamingPort_Name.c_str()))
    {
        yError() << "RGBDSensorWrapper: unable to open depth streaming Port" << depthFrame_StreamingPort_Name.c_str();
        bRet = false;
    }

    return bRet;
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

void RGBDSensorWrapper::shallowCopyImages(const ImageOf<PixelFloat>& src, ImageOf<PixelFloat>& dest)
{
    dest.setQuantum(src.getQuantum());
    dest.setExternal(src.getRawImage(), src.width(), src.height());
}



void RGBDSensorWrapper::deepCopyImages(const yarp::sig::FlexImage& src, 
                                       sensor_msgs_Image&          dest, 
                                       const string&               frame_id, 
                                       const TickTime&             timeStamp, 
                                       const UInt&                 seq)
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

void RGBDSensorWrapper::deepCopyImages(const DepthImage&  src,
                                       sensor_msgs_Image& dest,
                                       const string&      frame_id,
                                       const TickTime&    timeStamp,
                                       const UInt&        seq)
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

bool RGBDSensorWrapper::setCamInfo(sensor_msgs_CameraInfo& cameraInfo, const string& frame_id, const UInt& seq, const SensorType& sensorType)
{
    double                  fx, fy, cx, cy, k1, k2, t1, t2, k3, stamp;
    string                  distModel, currentSensor;
    UInt                    i;
    Property                camData;
    vector<param<double> >  parVector;
    param<double>*          par;
    bool                    ok;
    
    currentSensor = sensorType == COLOR_SENSOR ? "Rgb" : "Depth";
    ok            = sensorType == COLOR_SENSOR ? sensor_p->getRgbIntrinsicParam(camData) : sensor_p->getDepthIntrinsicParam(camData);

    if (!ok)
    {
        yError() << "unable to get intrinsic param from" << currentSensor << "sensor!";
        return false;
    }

    if(!camData.check("distortionModel"))
    {
        yWarning() << "missing distortion model";
        return false;
    }
    
    distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob")
    {
        yError() << "distortion model not supported";
        return false;
    }

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
    parVector.push_back(param<double>(stamp,"stamp"));
    for(i = 0; i < parVector.size(); i++)
    {
        par = &parVector[i];
        
        if(!camData.check(par->parname))
        {
            yWarning() << "RGBSensorWrapper: driver has not the param:" << par->parname;
            return false;
        }
        *par->var = camData.find(par->parname).asDouble();
    }
    
    cameraInfo.header.frame_id    = frame_id;
    cameraInfo.header.seq         = seq;
    cameraInfo.header.stamp       = normalizeSecNSec(stamp);
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

bool RGBDSensorWrapper::writeData()
{
    
    //colorImage.setPixelCode(VOCAB_PIXEL_RGB);
    //             depthImage.setPixelCode(VOCAB_PIXEL_MONO_FLOAT);

    //             colorImage.resize(hDim, vDim);  // Has this to be done each time? If size is the same what it does?
    //             depthImage.resize(hDim, vDim);
    if (!sensor_p->getImages(colorImage, depthImage, &colorStamp, &depthStamp))
    {
        return false;
    }

    if (use_YARP)
    {
        FlexImage& yColorImage           = colorFrame_StreamingPort.prepare();
        ImageOf<PixelFloat>& yDepthImage = depthFrame_StreamingPort.prepare();

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

        if (setCamInfo(camInfoC, rosFrameId, nodeSeq, COLOR_SENSOR))
        {
            rosPublisherPort_colorCaminfo.setEnvelope(colorStamp);
            rosPublisherPort_colorCaminfo.write();
        }
        else
        {
            yWarning("missing color camera parameters... camera info messages will be not sended");
        }
        if (setCamInfo(camInfoD, rosFrameId, nodeSeq, DEPTH_SENSOR))
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
        static int i = 0;
        sensorStatus = sensor_p->getSensorStatus();
        switch (sensorStatus)
        {
            case(IRGBDSensor::RGBD_SENSOR_OK_IN_USE) :
            {
                if (!writeData())
                    yError("Image not captured.. check hardware configuration");
                i = 0;
            }
            break;
            case(IRGBDSensor::RGBD_SENSOR_NOT_READY):
            {
                if(i < 1000)
                {
                    if((i % 30) == 0)
                        yInfo() << "device not ready, waiting...";
                }
                else
                {
                    yWarning() << "device is taking too long to start..";
                }
                i++;
            }
            break;
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

