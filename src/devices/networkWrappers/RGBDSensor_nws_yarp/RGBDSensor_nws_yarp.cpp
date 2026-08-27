/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "RGBDSensor_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>
#include <sstream>

using namespace RGBDImpl;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;

YARP_LOG_COMPONENT(RGBDSENSORNWSYARP, "yarp.devices.RgbdSensor_nws_yarp")

RGBDSensor_nws_yarp::RGBDSensor_nws_yarp() :
    PeriodicThread(DEFAULT_THREAD_PERIOD),
    m_sensorStatus(IRGBDSensor::RGBD_SENSOR_NOT_READY),
    verbose(4)
{}

RGBDSensor_nws_yarp::~RGBDSensor_nws_yarp()
{
    close();
    m_rgbdsensor = nullptr;
    m_fgCtrl = nullptr;
}

/** Device driver interface */

bool RGBDSensor_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    //port names
    std::string rootName = m_name;
    std::string rpcPort_Name = rootName + "/rpc:i";
    colorFrame_StreamingPort_Name = rootName + "/rgbImage:o";
    depthFrame_StreamingPort_Name = rootName + "/depthImage:o";

    // Open ports
    bool bRet;
    bRet = true;
    if (!rpcPort.open(rpcPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open rpc Port" << rpcPort_Name.c_str();
        bRet = false;
    }
    rpcPort.setReader(*this);

    if (!colorFrame_StreamingPort.open(colorFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open color streaming Port" << colorFrame_StreamingPort_Name.c_str();
        bRet = false;
    }
    if (!depthFrame_StreamingPort.open(depthFrame_StreamingPort_Name))
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to open depth streaming Port" << depthFrame_StreamingPort_Name.c_str();
        bRet = false;
    }

    if (!bRet) {return false;}
    return true;
}

bool RGBDSensor_nws_yarp::close()
{
    yCTrace(RGBDSENSORNWSYARP, "Close");
    detach();

    // Closing port
    rpcPort.interrupt();
    colorFrame_StreamingPort.interrupt();
    depthFrame_StreamingPort.interrupt();

    rpcPort.close();
    colorFrame_StreamingPort.close();
    depthFrame_StreamingPort.close();

    return true;
}

/**
  * WrapperSingle interface
  */

bool RGBDSensor_nws_yarp::detach()
{
    std::lock_guard lock (m_mutex);

    if (yarp::os::PeriodicThread::isRunning()) {
        yarp::os::PeriodicThread::stop();
    }

    m_rgbdsensor = nullptr;
    m_fgCtrl = nullptr;
    return true;
}

bool RGBDSensor_nws_yarp::attach(PolyDriver* poly)
{
    std::lock_guard lock (m_mutex);

    if(poly)
    {
        poly->view(m_rgbdsensor);
        poly->view(m_fgCtrl);
    }

    // m_rgbdsensor is mandatory
    if(m_rgbdsensor == nullptr)
    {
        yCError(RGBDSENSORNWSYARP) << "Attached device has no valid IRGBDSensor interface.";
        return false;
    }

    if(m_fgCtrl == nullptr)
    {
        yCWarning(RGBDSENSORNWSYARP) << "Attached device has no valid IFrameGrabberControls interface.";
    }

    // m_fgCtrl is optional and might be null
    m_rgbd_RPC = std::make_unique<RGBDSensorMsgsImpl>(m_rgbdsensor, m_fgCtrl);

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

/* IRateThread interface */

bool RGBDSensor_nws_yarp::threadInit()
{
    // Get interface from attached device if any.
    return true;
}

void RGBDSensor_nws_yarp::threadRelease()
{
    // Detach() calls stop() which in turns calls this functions, therefore no calls to detach here!
}

bool RGBDSensor_nws_yarp::setCamInfo(const std::string& frame_id, const UInt& seq, const SensorType& sensorType)
{
    std::string                  distModel, currentSensor;
    UInt                         i;
    yarp::sig::IntrinsicParams   camData;
    bool                    ok;

    currentSensor = sensorType == COLOR_SENSOR ? "Rgb" : "Depth";
    ok            = sensorType == COLOR_SENSOR ? m_rgbdsensor->getRgbIntrinsicParam(camData) : m_rgbdsensor->getDepthIntrinsicParam(camData);

    if (!ok)
    {
        yCError(RGBDSENSORNWSYARP) << "Unable to get intrinsic param from" << currentSensor << "sensor!";
        return false;
    }

    if (camData.distortionModel.type != CameraDistortionType::YARP_PLUMB_BOB)
    {
        yCError(RGBDSENSORNWSYARP) << "Distortion model not supported";
        return false;
    }

    return true;
}

bool RGBDSensor_nws_yarp::writeData()
{
    if (!m_rgbdsensor->getImages(colorImage, depthImage, &colorStamp, &depthStamp))
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
    // In the following piece of code we are uing a copy instead of setExternal() because it is safer in a multithreaded environment.
    // Indeed colorImage and depthImage can be modified by the attached device running in a different thread.
    if (rgb_data_ok && colorFrame_StreamingPort.getOutputCount() > 0)
    {
        FlexImage& yColorImage = colorFrame_StreamingPort.prepare();
        yColorImage = colorImage;
        colorFrame_StreamingPort.setEnvelope(colorStamp);
        colorFrame_StreamingPort.write();
    }
    if (depth_data_ok && depthFrame_StreamingPort.getOutputCount() > 0)
    {
        ImageOf<PixelFloat>& yDepthImage = depthFrame_StreamingPort.prepare();
        yDepthImage = depthImage;
        depthFrame_StreamingPort.setEnvelope(depthStamp);
        depthFrame_StreamingPort.write();
    }

    return true;
}

void RGBDSensor_nws_yarp::run()
{
    std::lock_guard lock (m_mutex);

    if (m_rgbdsensor!=nullptr)
    {
        static int i = 0;
        ReturnValue v = m_rgbdsensor->getSensorStatus(m_sensorStatus);
        switch (m_sensorStatus)
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
                    yCError(RGBDSENSORNWSYARP, "%s: Sensor returned error", m_name.c_str());
                }
            }
        }
    }
    else
    {
        if(verbose >= 6) {
            yCError(RGBDSENSORNWSYARP, "%s: Sensor interface is not valid", m_name.c_str());
        }
    }
}

bool RGBDSensor_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_rgbd_RPC)
    {
        if (m_rgbd_RPC->read(connection))
        {
            return true;
        }
    }
    else
    {
        yCError(RGBDSENSORNWSYARP) << "m_rgbd_RPC interface is not valid";
        return false;
    }

    yCError(RGBDSENSORNWSYARP) << "read() Command failed";
    return false;
}
