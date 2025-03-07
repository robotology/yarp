/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_H
#define YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_H

#include "RGBDSensorClient_StreamingMsgParser.h"

#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include "RGBDSensorClient_ParamsParser.h"

#include "RGBDSensorMsgs.h"

#define DEFAULT_THREAD_PERIOD       20    //ms
#define RGBDSENSOR_TIMEOUT_DEFAULT  100   //ms


class RGBDSensor_StreamingMsgParser;


/**
 *  @ingroup dev_impl_network_clients
 *
 * \section RGBDSensorClient_device_parameters Description of input parameters
 *
 * \brief `RGBDSensorClient`: A Network client to receive data from kinect-like devices.
 * This device will read from two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its server called RGBDSensor_nws_yarp to receive the data streams and perform remote operations.
 *
 * Parameters required by this device are shown in class: RGBDSensorClient_ParamsParser
 *
 */

class RGBDSensorClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IRGBDSensor,
        public RGBDSensorClient_ParamsParser
{
protected:
    yarp::os::Port                  m_rpcPort;

protected:
    RgbImageBufferedPort            m_colorFrame_StreamingPort;
    FloatImageBufferedPort          m_depthFrame_StreamingPort;

    std::mutex                      m_mutex;
    RGBDSensorMsgs                  m_rgbdsensor_RPC;

    // Image data specs
    yarp::dev::IRGBDSensor *sensor_p{nullptr};
    IRGBDSensor::RGBDSensor_status sensorStatus{IRGBDSensor::RGBD_SENSOR_NOT_READY};
    int verbose{2};

    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;

    // This is gonna be superseded by the synchronized when it'll be ready
    RGBDSensor_StreamingMsgParser *streamingReader{nullptr};

public:
    RGBDSensorClient();
    RGBDSensorClient(const RGBDSensorClient&) = delete;
    RGBDSensorClient(RGBDSensorClient&&) = delete;
    RGBDSensorClient& operator=(const RGBDSensorClient&) = delete;
    RGBDSensorClient& operator=(RGBDSensorClient&&) = delete;
    ~RGBDSensorClient() override;

    /*
     *  IRgbVisualParams interface. Look at IVisualParams.h for documentation
     */
    int  getRgbHeight() override;
    int  getRgbWidth() override;
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig> &configurations) override;
    yarp::dev::ReturnValue getRgbResolution(int &width, int &height) override;
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue getRgbFOV(double &horizontalFov, double &verticalFov) override;
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property &intrinsic) override;
    yarp::dev::ReturnValue getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override;

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
    int    getDepthHeight() override;
    int    getDepthWidth() override;
    yarp::dev::ReturnValue getDepthResolution(int& width, int& height) override;
    yarp::dev::ReturnValue setDepthResolution(int width, int height) override;
    yarp::dev::ReturnValue getDepthFOV(double &horizontalFov, double &verticalFov) override;
    yarp::dev::ReturnValue setDepthFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getDepthAccuracy(double& accuracy) override;
    yarp::dev::ReturnValue setDepthAccuracy(double accuracy) override;
    yarp::dev::ReturnValue getDepthClipPlanes(double &near, double &far) override;
    yarp::dev::ReturnValue setDepthClipPlanes(double near, double far) override;
    yarp::dev::ReturnValue getDepthIntrinsicParam(yarp::os::Property &intrinsic) override;
    yarp::dev::ReturnValue getDepthMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setDepthMirroring(bool mirror) override;

    /*
     * Device Driver interface
     */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /*
     * IRGBDSensor specific interface methods
     */
    yarp::dev::ReturnValue getExtrinsicParam(yarp::sig::Matrix &extrinsic) override;
    yarp::dev::ReturnValue getSensorStatus(IRGBDSensor::RGBDSensor_status& status) override;
    yarp::dev::ReturnValue getLastErrorMsg(std::string& message, yarp::os::Stamp *timeStamp = nullptr) override;
    yarp::dev::ReturnValue getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = nullptr) override;
    yarp::dev::ReturnValue getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = nullptr) override;
    yarp::dev::ReturnValue getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=nullptr, yarp::os::Stamp *depthStamp=nullptr) override;

    /*
     * IFrameGrabberControls specific interface methods
     */
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor& camera) override;
    yarp::dev::ReturnValue hasFeature(int feature, bool& hasFeature) override;
    yarp::dev::ReturnValue setFeature(int feature, double value) override;
    yarp::dev::ReturnValue getFeature(int feature, double& value) override;
    yarp::dev::ReturnValue setFeature(int feature, double value1, double value2) override;
    yarp::dev::ReturnValue getFeature(int feature, double& value1, double& value2) override;
    yarp::dev::ReturnValue hasOnOff(int feature, bool& HasOnOff) override;
    yarp::dev::ReturnValue setActive(int feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(int feature, bool& isActive) override;
    yarp::dev::ReturnValue hasAuto(int feature, bool& hasAuto) override;
    yarp::dev::ReturnValue hasManual(int feature, bool& hasManual) override;
    yarp::dev::ReturnValue hasOnePush(int feature, bool& hasOnePush) override;
    yarp::dev::ReturnValue setMode(int feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(int feature, yarp::dev::FeatureMode& mode) override;
    yarp::dev::ReturnValue setOnePush(int feature) override;

};

#endif // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_H
