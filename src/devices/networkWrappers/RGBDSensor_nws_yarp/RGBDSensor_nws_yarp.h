/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDSENSOR_NWS_YARP_H
#define YARP_DEV_RGBDSENSOR_NWS_YARP_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>


#include <yarp/sig/Vector.h>

#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IFrameGrabberControls.h>

#include "RGBDSensorMsgsImpl.h"

#include "RGBDSensor_nws_yarp_ParamsParser.h"

namespace RGBDImpl {

#define DEFAULT_THREAD_PERIOD   0.03 // s

} // namespace RGBDImpl

/**
 *  @ingroup dev_impl_nws_yarp
 *
 * \section RGBDSensor_nws_yarp_device_parameters Description of input parameters
 * \brief `RGBDSensor_nws_yarp`: A Network grabber for kinect-like devices.
 * This device will produce two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its client called RgbdSensor_nws_yarp to receive the data streams and perform remote operations.
 *
 * Parameters required by this device are shown in class: RgbdSensor_nws_yarp_ParamsParser
 */

class RGBDSensor_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public yarp::os::PortReader,
        RGBDSensor_nws_yarp_ParamsParser
{
private:
    typedef yarp::sig::ImageOf<yarp::sig::PixelFloat>    DepthImage;
    typedef yarp::os::BufferedPort<DepthImage>           DepthPortType;
    typedef yarp::os::BufferedPort<yarp::sig::FlexImage> ImagePortType;
    typedef unsigned int                                 UInt;

    enum SensorType{COLOR_SENSOR, DEPTH_SENSOR};

    template <class T>
    struct param
    {
        param(T& inVar, std::string inName)
        {
            var          = &inVar;
            parname      = inName;
        }
        T*              var;
        std::string     parname;
    };

    std::string colorFrame_StreamingPort_Name;
    std::string depthFrame_StreamingPort_Name;
    ImagePortType         colorFrame_StreamingPort;
    DepthPortType         depthFrame_StreamingPort;

    // One RPC port should be enough for the wrapper in all cases
    yarp::os::Port        rpcPort;
    std::string           rpcPort_Name;
    yarp::sig::FlexImage  colorImage;
    DepthImage            depthImage;

    //Helper class for RPCs
    std::unique_ptr<RGBDSensorMsgsImpl> m_rgbd_RPC = nullptr;
    std::mutex                     m_mutex;

    // Image data specs
    // int hDim, vDim;
    yarp::dev::IRGBDSensor* m_rgbdsensor = nullptr;
    yarp::dev::IFrameGrabberControls* m_fgCtrl = nullptr;
    yarp::dev::IRGBDSensor::RGBDSensor_status m_sensorStatus;
    int                            verbose;

    // Synch
    yarp::os::Stamp                colorStamp;
    yarp::os::Stamp                depthStamp;

    bool writeData();
    bool setCamInfo(const std::string&                     frame_id,
                    const UInt&                            seq,
                    const SensorType&                      sensorType);

public:
    RGBDSensor_nws_yarp();
    RGBDSensor_nws_yarp(const RGBDSensor_nws_yarp&) = delete;
    RGBDSensor_nws_yarp(RGBDSensor_nws_yarp&&) = delete;
    RGBDSensor_nws_yarp& operator=(const RGBDSensor_nws_yarp&) = delete;
    RGBDSensor_nws_yarp& operator=(RGBDSensor_nws_yarp&&) = delete;
    ~RGBDSensor_nws_yarp() override;

    bool        open(yarp::os::Searchable &params) override;
    bool        close() override;

    bool        read(yarp::os::ConnectionReader& connection) override;

    bool        attach(yarp::dev::PolyDriver *poly) override;
    bool        detach() override;

    bool        threadInit() override;
    void        threadRelease() override;
    void        run() override;
};

#endif   // YARP_DEV_RGBDSENSOR_NWS_YARP_H
