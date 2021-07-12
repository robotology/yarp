/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RgbdSensor_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/proto/framegrabber/CameraVocabs.h>

#include <cstdio>
#include <cstring>
#include <sstream>

using namespace RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RGBDSENSORNWSYARP, "yarp.devices.RgbdSensor_nws_yarp")

#define RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR 1
#define RGBD_INTERFACE_PROTOCOL_VERSION_MINOR 0


RGBDSensorParser::RGBDSensorParser() :
        iRGBDSensor(nullptr)
{
}

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

bool RGBDSensorParser::configure(IFrameGrabberControls *_fgCtrl)
{
    return fgCtrlParsers.configure(_fgCtrl);
}

bool RGBDSensorParser::respond(const Bottle& cmd, Bottle& response)
{
    bool ret = false;
    int interfaceType = cmd.get(0).asVocab32();

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

        case VOCAB_FRAMEGRABBER_CONTROL:
        {
            // forwarding to the proper parser.
            ret = fgCtrlParsers.respond(cmd, response);
        }
        break;

        case VOCAB_RGBD_SENSOR:
        {
            switch (cmd.get(1).asVocab32())
            {
                case VOCAB_GET:
                {
                    switch(cmd.get(2).asVocab32())
                    {
                        case VOCAB_EXTRINSIC_PARAM:
                        {
                            yarp::sig::Matrix params;
                            ret = iRGBDSensor->getExtrinsicParam(params);
                            if(ret)
                            {
                                yarp::os::Bottle params_b;
                                response.addVocab32(VOCAB_RGBD_SENSOR);
                                response.addVocab32(VOCAB_EXTRINSIC_PARAM);
                                response.addVocab32(VOCAB_IS);
                                ret &= Property::copyPortable(params, params_b);  // will it really work??
                                response.append(params_b);
                            }
                            else
                                response.addVocab32(VOCAB_FAILED);
                        }
                        break;

                        case VOCAB_ERROR_MSG:
                        {
                            response.addVocab32(VOCAB_RGBD_SENSOR);
                            response.addVocab32(VOCAB_ERROR_MSG);
                            response.addVocab32(VOCAB_IS);
                            response.addString(iRGBDSensor->getLastErrorMsg());
                            ret = true;
                        }
                        break;

                        case VOCAB_RGBD_PROTOCOL_VERSION:
                        {
                            response.addVocab32(VOCAB_RGBD_SENSOR);
                            response.addVocab32(VOCAB_RGBD_PROTOCOL_VERSION);
                            response.addVocab32(VOCAB_IS);
                            response.addInt32(RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR);
                            response.addInt32(RGBD_INTERFACE_PROTOCOL_VERSION_MINOR);
                        }
                        break;

                        case VOCAB_STATUS:
                        {
                            response.addVocab32(VOCAB_RGBD_SENSOR);
                            response.addVocab32(VOCAB_STATUS);
                            response.addVocab32(VOCAB_IS);
                            response.addInt32(iRGBDSensor->getSensorStatus());
                        }
                        break;

                        default:
                        {
                            yCError(RGBDSENSORNWSYARP) << "Interface parser received an unknown GET command. Command is " << cmd.toString();
                            response.addVocab32(VOCAB_FAILED);
                        }
                        break;
                    }
                }
                break;

                case VOCAB_SET:
                {
                    yCError(RGBDSENSORNWSYARP) << "Interface parser received an unknown SET command. Command is " << cmd.toString();
                    response.addVocab32(VOCAB_FAILED);
                }
                break;
            }
        }
        break;

        default:
        {
            yCError(RGBDSENSORNWSYARP) << "Received a command for a wrong interface " << yarp::os::Vocab32::decode(interfaceType);
            return DeviceResponder::respond(cmd,response);
        }
        break;
    }
    return ret;
}


RgbdSensor_nws_yarp::RgbdSensor_nws_yarp() :
    PeriodicThread(DEFAULT_THREAD_PERIOD),
    period(DEFAULT_THREAD_PERIOD),
    sensor_p(nullptr),
    fgCtrl(nullptr),
    sensorStatus(IRGBDSensor::RGBD_SENSOR_NOT_READY),
    verbose(4),
    isSubdeviceOwned(false),
    subDeviceOwned(nullptr)
{}

RgbdSensor_nws_yarp::~RgbdSensor_nws_yarp()
{
    close();
    sensor_p = nullptr;
    fgCtrl = nullptr;
}

/** Device driver interface */

bool RgbdSensor_nws_yarp::open(yarp::os::Searchable &config)
{
//     DeviceResponder::makeUsage();
//     addUsage("[set] [bri] $fBrightness", "set brightness");
//     addUsage("[set] [expo] $fExposure", "set exposure");
//
    m_conf.fromString(config.toString());
    if(verbose >= 5)
        yCTrace(RGBDSENSORNWSYARP) << "\nParameters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yCError(RGBDSENSORNWSYARP) << "Failed to open, check previous log for error messages.";
        return false;
    }

    setId("RGBDSensorWrapper for " + depthFrame_StreamingPort_Name);

    if(!initialize_YARP(config))
    {
        yCError(RGBDSENSORNWSYARP) << sensorId << "Error initializing YARP ports";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on through attachAll()
    if(isSubdeviceOwned)
    {
        if(! openAndAttachSubDevice(config))
        {
            yCError(RGBDSENSORNWSYARP, "Error while opening subdevice");
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

bool RgbdSensor_nws_yarp::fromConfig(yarp::os::Searchable &config)
{
    if (!config.check("period", "refresh period of the broadcasted values in s"))
    {
        if(verbose >= 3)
            yCInfo(RGBDSENSORNWSYARP) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
    }
    else
        period = config.find("period").asFloat64();

    std::string rootName;
    rootName = config.check("name",Value("/"), "starting '/' if needed.").asString();

    if (!config.check("name", "Prefix name of the ports opened by the RGBD wrapper.")) {
        yCError(RGBDSENSORNWSYARP) << "Missing 'name' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORNWSYARP) << "   name:         Prefix name of the ports opened by the RGBD wrapper, e.g. /robotName/RGBD";
        return false;
    }

    rootName = config.find("name").asString();
    rpcPort_Name  = rootName + "/rpc:i";
    colorFrame_StreamingPort_Name = rootName + "/rgbImage:o";
    depthFrame_StreamingPort_Name = rootName + "/depthImage:o";

    if(config.check("subdevice")) {
        isSubdeviceOwned=true;
    } else {
        isSubdeviceOwned=false;
    }

    return true;
}

bool RgbdSensor_nws_yarp::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RgbdSensor_nws_yarp::openAndAttachSubDevice(Searchable& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(RGBDSENSORNWSYARP, "Opening IRGBDSensor subdevice");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yCError(RGBDSENSORNWSYARP, "Opening IRGBDSensor subdevice... FAILED");
        return false;
    }
    isSubdeviceOwned = true;
    if(!attach(subDeviceOwned)) {
        return false;
    }

    return true;
}

bool RgbdSensor_nws_yarp::close()
{
    yCTrace(RGBDSENSORNWSYARP, "Close");
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

    // Closing port

    rpcPort.interrupt();
    colorFrame_StreamingPort.interrupt();
    depthFrame_StreamingPort.interrupt();

    rpcPort.close();
    colorFrame_StreamingPort.close();
    depthFrame_StreamingPort.close();

    return true;
}

/* Helper functions */

bool RgbdSensor_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    // Open ports
    bool bRet;
    bRet = true;
    if(!rpcPort.open(rpcPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }
    rpcPort.setReader(rgbdParser);

    if(!colorFrame_StreamingPort.open(colorFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open color streaming Port" << colorFrame_StreamingPort_Name.c_str();
        bRet = false;
    }
    if(!depthFrame_StreamingPort.open(depthFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open depth streaming Port" << depthFrame_StreamingPort_Name.c_str();
        bRet = false;
    }

    return bRet;
}

void RgbdSensor_nws_yarp::setId(const std::string &id)
{
    sensorId=id;
}

std::string RgbdSensor_nws_yarp::getId()
{
    return sensorId;
}

/**
  * WrapperSingle interface
  */

bool RgbdSensor_nws_yarp::detach()
{
    if (yarp::os::PeriodicThread::isRunning())
        yarp::os::PeriodicThread::stop();

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;

    sensor_p = nullptr;
    return true;
}

bool RgbdSensor_nws_yarp::attach(PolyDriver* poly)
{
    if(poly)
    {
        poly->view(sensor_p);
        poly->view(fgCtrl);
    }

    if(sensor_p == nullptr)
    {
        yCError(RGBDSENSORNWSYARP) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }

    if(!rgbdParser.configure(sensor_p))
    {
        yCError(RGBDSENSORNWSYARP) << "Error configuring IRGBD interface";
        return false;
    }

    if(fgCtrl != nullptr)
    {
        if (!rgbdParser.configure(fgCtrl)) {
            yCError(RGBDSENSORNWSYARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }
    else
    {
        yCWarning(RGBDSENSORNWSYARP) << "Attached device has no valid IFrameGrabberControls interface.";
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

/* IRateThread interface */

bool RgbdSensor_nws_yarp::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RgbdSensor_nws_yarp::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

bool RgbdSensor_nws_yarp::setCamInfo(const string& frame_id, const UInt& seq, const SensorType& sensorType)
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
        yCError(RGBDSENSORNWSYARP) << "Unable to get intrinsic param from" << currentSensor << "sensor!";
        return false;
    }

    if(!camData.check("distortionModel"))
    {
        yCWarning(RGBDSENSORNWSYARP) << "Missing distortion model";
        return false;
    }

    distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob")
    {
        yCError(RGBDSENSORNWSYARP) << "Distortion model not supported";
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
            yCWarning(RGBDSENSORNWSYARP) << "Driver has not the param:" << par->parname;
            return false;
        }
        *par->var = camData.find(par->parname).asFloat64();
    }

    return true;
}

bool RgbdSensor_nws_yarp::writeData()
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
        FlexImage& yColorImage = colorFrame_StreamingPort.prepare();
        yColorImage.setPixelCode(colorImage.getPixelCode());
        yColorImage.setQuantum(colorImage.getQuantum());
        yColorImage.setExternal(colorImage.getRawImage(), colorImage.width(), colorImage.height());
        colorFrame_StreamingPort.setEnvelope(colorStamp);
        colorFrame_StreamingPort.write();
    }
    if (depth_data_ok)
    {
        ImageOf<PixelFloat>& yDepthImage = depthFrame_StreamingPort.prepare();
        yDepthImage.setQuantum(depthImage.getQuantum());
        yDepthImage.setExternal(depthImage.getRawImage(), depthImage.width(), depthImage.height());
        depthFrame_StreamingPort.setEnvelope(depthStamp);
        depthFrame_StreamingPort.write();
    }

    return true;
}

void RgbdSensor_nws_yarp::run()
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
                    yCError(RGBDSENSORNWSYARP, "Image not captured.. check hardware configuration");
                }
                i = 0;
            }
            break;
            case(IRGBDSensor::RGBD_SENSOR_NOT_READY):
            {
                if(i < 1000) {
                    if((i % 30) == 0) {
                        yCInfo(RGBDSENSORNWSYARP) << "Device not ready, waiting...";
                    }
                } else {
                    yCWarning(RGBDSENSORNWSYARP) << "Device is taking too long to start..";
                }
                i++;
            }
            break;
            default:
            {
                if (verbose >= 1) {  // better not to print it every cycle anyway, too noisy
                    yCError(RGBDSENSORNWSYARP, "%s: Sensor returned error", sensorId.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDSENSORNWSYARP, "%s: Sensor interface is not valid", sensorId.c_str());
        }
    }
}
