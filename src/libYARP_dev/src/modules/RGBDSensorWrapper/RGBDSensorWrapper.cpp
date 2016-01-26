/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <Alberto.Cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include "RGBDSensorWrapper.h"

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createRGBDSensorWrapper() {
    return new DriverCreatorOf<yarp::dev::RGBDSensorWrapper>("RGBDSensorWrapper", "RGBDSensorWrapper", "yarp::dev::RGBDSensorWrapper");
}

RGBDSensorWrapper::RGBDSensorWrapper(): RateThread(DEFAULT_THREAD_PERIOD),
                                        _rate(DEFAULT_THREAD_PERIOD)
{
    // TBD: default values for hDim and vDim should be zero, using standard values just for easy testing
//     hDim = 640;
//     vDim = 480;
    sensor_p = NULL;
    use_YARP = true;
    use_ROS  = false;
    subDeviceOwned = NULL;
    _isSubdeviceOwned = false;
    verbose  = DEFAULT_VERBOSE_LEVEL;
    _sensorStatus = IRGBDSensor::RGBD_SENSOR_NOT_READY;
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
    if(verbose >= yarp::dev::IDepthSensor::VERY_VERBOSE)
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
        if(verbose >= IDepthSensor::CHATTY)
            yInfo() << "RGBDSensorWrapper: using default 'period' parameter of " << DEFAULT_THREAD_PERIOD << "ms";
    }
    else
        _rate = config.find("period").asInt();

    Bottle &rosGroup = config.findGroup("ROS");
    if(rosGroup.isNull())
    {
        if(verbose >= IDepthSensor::CHATTY)
            yInfo() << "RGBDSensorWrapper: ROS configuration paramters are not set, skipping ROS topic initialization.";
        use_ROS  = false;
        use_YARP = true;
    }
    else
    {
        if(verbose >= IDepthSensor::DEFAULT)
            yWarning() << "RGBDSensorWrapper: ROS topic support is not yet implemented";
        use_ROS = false;
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
            colorFrame_StreamingPort_Name  = config.find("imagePort").asString().c_str();
            colorFrame_rpcPort_Name = colorFrame_StreamingPort_Name + "/rpc:i";
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
        _isSubdeviceOwned=true;
        if(! openAndAttachSubDevice(config))
        {
            yError("ControlBoardWrapper: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        _isSubdeviceOwned=false;
        if(!openDeferredAttach(config))
            return false;
    }
    return true;
}

bool RGBDSensorWrapper::openDeferredAttach(Searchable& prop)
{
    // I dunno what to do here now...
    _isSubdeviceOwned = false;
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
    _isSubdeviceOwned = true;
    if(!attach(subDeviceOwned))
        return false;

    RateThread::setRate(_rate);
    RateThread::start();
    return true;
}

bool RGBDSensorWrapper::close()
{
    yTrace("RGBDSensorWrapper::Close");
    detachAll();

    // close subdevice if it was created inside the open (--subdevice option)
    if(_isSubdeviceOwned)
    {
        if(subDeviceOwned)
            subDeviceOwned->close();
        subDeviceOwned = NULL;
        sensor_p = NULL;
        _isSubdeviceOwned = false;
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

    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool RGBDSensorWrapper::detachAll()
{
    if (yarp::os::RateThread::isRunning())
        yarp::os::RateThread::stop();

    //check if we already instantiated a subdevice previously
    if (_isSubdeviceOwned)
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

void RGBDSensorWrapper::run()
{
    if (sensor_p!=0)
    {
        sensor_p->getRGBDSensor_Status(&_sensorStatus);
        // convert to switch case
        if (_sensorStatus == IRGBDSensor::RGBD_SENSOR_OK_IN_USE)
        {
            yarp::sig::FlexImage& colorImage = colorFrame_StreamingPort.prepare();
            yarp::sig::FlexImage& depthImage = depthFrame_StreamingPort.prepare();

//             colorImage.setPixelCode(VOCAB_PIXEL_RGB);
//             depthImage.setPixelCode(VOCAB_PIXEL_MONO_FLOAT);

//             colorImage.resize(hDim, vDim);  // Has this to be done each time? If size is the same what it does?
//             depthImage.resize(hDim, vDim);

            bool ret = sensor_p->getRGBD_Frames(colorImage, depthImage, &colorStamp, &depthStamp);

            // TBD: We should check here somehow if the timestamp was correctly updated and, if not, update it ourselves.
            if(ret)
            {
                colorFrame_StreamingPort.setEnvelope(colorStamp);
                colorFrame_StreamingPort.write();

                depthFrame_StreamingPort.setEnvelope(depthStamp);
                depthFrame_StreamingPort.write();
            }
        }
        else
        {
            if(verbose >= IDepthSensor::QUIET)   // better not to print it every cycle anyway, too noisy
                yError("RGBDSensorWrapper: %s: Sensor returned error", sensorId.c_str());
        }
    }
    else
    {
        if(verbose >= IDepthSensor::VERY_VERY_VERBOSE)
            yError("RGBDSensorWrapper: %s: Sensor interface is not valid", sensorId.c_str());
    }
}
