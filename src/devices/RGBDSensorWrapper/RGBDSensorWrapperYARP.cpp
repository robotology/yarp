/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "RGBDSensorWrapperYARP.h"
#include <sstream>
#include <cstdio>
#include <cstring>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/GenericVocabs.h>
#include "rosPixelCode.h"
#include <RGBDRosConversionUtils.h>

using namespace RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RGBDSENSORWRAPPERYARP, "yarp.devices.RGBDSensorWrapperYARP")

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

        case VOCAB_FRAMEGRABBER_CONTROL:
        {
            // forwarding to the proper parser.
            ret = fgCtrlParsers.respond(cmd, response);
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
                            response.addInt32(RGBD_INTERFACE_PROTOCOL_VERSION_MAJOR);
                            response.addInt32(RGBD_INTERFACE_PROTOCOL_VERSION_MINOR);
                        }
                        break;

                        case VOCAB_STATUS:
                        {
                            response.addVocab(VOCAB_RGBD_SENSOR);
                            response.addVocab(VOCAB_STATUS);
                            response.addVocab(VOCAB_IS);
                            response.addInt32(iRGBDSensor->getSensorStatus());
                        }
                        break;

                        default:
                        {
                            yCError(RGBDSENSORWRAPPERYARP) << "Interface parser received an unknown GET command. Command is " << cmd.toString();
                            response.addVocab(VOCAB_FAILED);
                        }
                        break;
                    }
                }
                break;

                case VOCAB_SET:
                {
                    yCError(RGBDSENSORWRAPPERYARP) << "Interface parser received an unknown SET command. Command is " << cmd.toString();
                    response.addVocab(VOCAB_FAILED);
                }
                break;
            }
        }
        break;

        default:
        {
            yCError(RGBDSENSORWRAPPERYARP) << "Received a command for a wrong interface " << yarp::os::Vocab::decode(interfaceType);
            return DeviceResponder::respond(cmd,response);
        }
        break;
    }
    return ret;
}


RGBDSensorWrapperYARP::RGBDSensorWrapperYARP() :
    PeriodicThread(DEFAULT_THREAD_PERIOD),
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

RGBDSensorWrapperYARP::~RGBDSensorWrapperYARP()
{
    close();
    sensor_p = nullptr;
    fgCtrl = nullptr;
}

/** Device driver interface */

bool RGBDSensorWrapperYARP::open(yarp::os::Searchable &config)
{
//     DeviceResponder::makeUsage();
//     addUsage("[set] [bri] $fBrightness", "set brightness");
//     addUsage("[set] [expo] $fExposure", "set exposure");
//
    m_conf.fromString(config.toString());
    if(verbose >= 5)
        yCTrace(RGBDSENSORWRAPPERYARP) << "\nParameters are: \n" << config.toString();

    if(!fromConfig(config))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Failed to open, check previous log for error messages.";
        return false;
    }

    setId("RGBDSensorWrapper for " + depthFrame_StreamingPort_Name);

    if(!initialize_YARP(config))
    {
        yCError(RGBDSENSORWRAPPERYARP) << sensorId << "Error initializing YARP ports";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on through attachAll()
    if(isSubdeviceOwned)
    {
        if(! openAndAttachSubDevice(config))
        {
            yCError(RGBDSENSORWRAPPERYARP, "Error while opening subdevice");
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

bool RGBDSensorWrapperYARP::fromConfig(yarp::os::Searchable &config)
{
    if (!config.check("period", "refresh period of the broadcasted values in ms"))
    {
        if(verbose >= 3)
            yCInfo(RGBDSENSORWRAPPERYARP) << "Using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "s";
    }
    else
        period = config.find("period").asInt32() / 1000.0;

    std::string rootName;
    rootName = config.check("name",Value("/"), "starting '/' if needed.").asString();

    if (!config.check("name", "Prefix name of the ports opened by the RGBD wrapper.")) {
        yCError(RGBDSENSORWRAPPERYARP) << "Missing 'name' parameter. Check you configuration file; it must be like:";
        yCError(RGBDSENSORWRAPPERYARP) << "   name:         Prefix name of the ports opened by the RGBD wrapper, e.g. /robotName/RGBD";
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

bool RGBDSensorWrapperYARP::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    isSubdeviceOwned = false;
    return true;
}

bool RGBDSensorWrapperYARP::openAndAttachSubDevice(Searchable& prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString());

    p.setMonitor(prop.getMonitor(), "subdevice"); // pass on any monitoring
    p.unput("device");
    p.put("device",prop.find("subdevice").asString());  // subdevice was already checked before

    // if errors occurred during open, quit here.
    yCDebug(RGBDSENSORWRAPPERYARP, "Opening IRGBDSensor subdevice");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yCError(RGBDSENSORWRAPPERYARP, "Opening IRGBDSensor subdevice... FAILED");
        return false;
    }
    isSubdeviceOwned = true;
    if(!attach(subDeviceOwned)) {
        return false;
    }

    return true;
}

bool RGBDSensorWrapperYARP::close()
{
    yCTrace(RGBDSENSORWRAPPERYARP, "Close");
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

bool RGBDSensorWrapperYARP::initialize_YARP(yarp::os::Searchable &params)
{
    // Open ports
    bool bRet;
    bRet = true;
    if(!rpcPort.open(rpcPort_Name))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }
    rpcPort.setReader(rgbdParser);

    if(!colorFrame_StreamingPort.open(colorFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Unable to open color streaming Port" << colorFrame_StreamingPort_Name.c_str();
        bRet = false;
    }
    if(!depthFrame_StreamingPort.open(depthFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Unable to open depth streaming Port" << depthFrame_StreamingPort_Name.c_str();
        bRet = false;
    }

    return bRet;
}

void RGBDSensorWrapperYARP::setId(const std::string &id)
{
    sensorId=id;
}

std::string RGBDSensorWrapperYARP::getId()
{
    return sensorId;
}

/**
  * IWrapper and IMultipleWrapper interfaces
  */
bool RGBDSensorWrapperYARP::attachAll(const PolyDriverList &device2attach)
{
    // First implementation only accepts devices with both the interfaces Framegrabber and IDepthSensor,
    // on a second version maybe two different devices could be accepted, one for each interface.
    // Yet to be defined which and how parameters shall be used in this case ... using the name of the
    // interface to view could be a good initial guess.
    if (device2attach.size() != 1)
    {
        yCError(RGBDSENSORWRAPPERYARP, "Cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;
    if(device2attach[0]->key == "IRGBDSensor")
    {
        yCInfo(RGBDSENSORWRAPPERYARP) << "Good name!";
    }
    else
    {
        yCInfo(RGBDSENSORWRAPPERYARP) << "Bad name!";
    }

    if (!Idevice2attach->isValid())
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Device " << device2attach[0]->key << " to attach to is not valid ... cannot proceed";
        return false;
    }

    Idevice2attach->view(sensor_p);
    Idevice2attach->view(fgCtrl);
    if(!attach(sensor_p))
        return false;

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RGBDSensorWrapperYARP::detachAll()
{
    if (yarp::os::PeriodicThread::isRunning())
        yarp::os::PeriodicThread::stop();

    //check if we already instantiated a subdevice previously
    if (isSubdeviceOwned)
        return false;

    sensor_p = nullptr;
    return true;
}

bool RGBDSensorWrapperYARP::attach(yarp::dev::IRGBDSensor *s)
{
    if(s == nullptr)
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }
    sensor_p = s;
    if(!rgbdParser.configure(sensor_p))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Error configuring interfaces for parsers";
        return false;
    }
    if (fgCtrl)
    {
        if(!rgbdParser.configure(fgCtrl))
        {
            yCError(RGBDSENSORWRAPPERYARP) << "Error configuring interfaces for parsers";
            return false;
        }
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RGBDSensorWrapperYARP::attach(PolyDriver* poly)
{
    if(poly)
    {
        poly->view(sensor_p);
        poly->view(fgCtrl);
    }

    if(sensor_p == nullptr)
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }

    if(!rgbdParser.configure(sensor_p))
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Error configuring IRGBD interface";
        return false;
    }

    if (!rgbdParser.configure(fgCtrl))
    {
        yCWarning(RGBDSENSORWRAPPERYARP) <<"Interface IFrameGrabberControl not implemented by the device";
    }

    PeriodicThread::setPeriod(period);
    return PeriodicThread::start();
}

bool RGBDSensorWrapperYARP::detach()
{
    sensor_p = nullptr;
    return true;
}

/* IRateThread interface */

bool RGBDSensorWrapperYARP::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RGBDSensorWrapperYARP::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

bool RGBDSensorWrapperYARP::setCamInfo(const string& frame_id, const UInt& seq, const SensorType& sensorType)
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
        yCError(RGBDSENSORWRAPPERYARP) << "Unable to get intrinsic param from" << currentSensor << "sensor!";
        return false;
    }

    if(!camData.check("distortionModel"))
    {
        yCWarning(RGBDSENSORWRAPPERYARP) << "Missing distortion model";
        return false;
    }

    distModel = camData.find("distortionModel").asString();
    if (distModel != "plumb_bob")
    {
        yCError(RGBDSENSORWRAPPERYARP) << "Distortion model not supported";
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
            yCWarning(RGBDSENSORWRAPPERYARP) << "Driver has not the param:" << par->parname;
            return false;
        }
        *par->var = camData.find(par->parname).asFloat64();
    }

    return true;
}

bool RGBDSensorWrapperYARP::writeData()
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
        yarp::dev::RGBDRosConversionUtils::shallowCopyImages(colorImage, yColorImage);
        colorFrame_StreamingPort.setEnvelope(colorStamp);
        colorFrame_StreamingPort.write();
    }
    if (depth_data_ok)
    {
        ImageOf<PixelFloat>& yDepthImage = depthFrame_StreamingPort.prepare();
        yarp::dev::RGBDRosConversionUtils::shallowCopyImages(depthImage, yDepthImage);
        depthFrame_StreamingPort.setEnvelope(depthStamp);
        depthFrame_StreamingPort.write();
    }

    return true;
}

void RGBDSensorWrapperYARP::run()
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
                    yCError(RGBDSENSORWRAPPERYARP, "Image not captured.. check hardware configuration");
                }
                i = 0;
            }
            break;
            case(IRGBDSensor::RGBD_SENSOR_NOT_READY):
            {
                if(i < 1000) {
                    if((i % 30) == 0) {
                        yCInfo(RGBDSENSORWRAPPERYARP) << "Device not ready, waiting...";
                    }
                } else {
                    yCWarning(RGBDSENSORWRAPPERYARP) << "Device is taking too long to start..";
                }
                i++;
            }
            break;
            default:
            {
                if (verbose >= 1) {  // better not to print it every cycle anyway, too noisy
                    yCError(RGBDSENSORWRAPPERYARP, "%s: Sensor returned error", sensorId.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDSENSORWRAPPERYARP, "%s: Sensor interface is not valid", sensorId.c_str());
        }
    }
}
