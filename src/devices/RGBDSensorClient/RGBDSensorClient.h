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
#include <yarp/dev/IPreciselyTimed.h>

#include <yarp/proto/framegrabber/FrameGrabberControls_Forwarder.h>
#include <yarp/proto/framegrabber/RgbVisualParams_Forwarder.h>
#include <yarp/proto/framegrabber/DepthVisualParams_Forwarder.h>

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
 * This device is paired with its server called RGBDSensorWrapper to receive the data streams and perform remote operations.
 *
 *   Parameters required by this device are:
 * | Parameter name  | SubParameter   | Type    | Units          | Default Value | Required      | Description                                                                           | Notes |
 * |:---------------:|:--------------:|:-------:|:--------------:|:-------------:|:------------: |:-------------------------------------------------------------------------------------:|:-----:|
 * | localImagePort  |      -         | string  | -              |   -           |               | Full name of the local port to open, e.g. /myApp/RGBD/rgb_camera:i                    |       |
 * | localDepthPort  |      -         | string  | -              |   -           |               | Full name of the local port to open, e.g. /myApp/RGBD/depth_camera:i                  |       |
 * | localRpcPort    |      -         | string  | -              |   -           |               | Full name of the local RPC port to open, e.g. /myApp/RGBD/rpc                         |       |
 * | remoteImagePort |      -         | string  | -              |   -           |               | Full name of the port to read color images from, e.g. /robotName/RGBD/image_camera:o  |       |
 * | remoteDepthPort |      -         | string  | -              |   -           |               | Full name of the port to read depth images from, e.g. /robotName/RGBD/depth_camera:o  |       |
 * | remoteRpcPort   |      -         | string  | -              |   -           |               | Full name of the remote RPC port, e.g. /robotName/RGBD/rpc                            |       |
 * | ImageCarrier    |      -         | string  | -              | udp           | No            | Carrier name for the Image stream used in the client-server connection                |       |
 * | DepthCarrier    |      -         | string  | -              | udp           | No            | Carrier name for the Depth strean used in the client-server connection                |       |
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 * device RGBDSensorClient
 * localImagePort     /clientRgbPort:i
 * localDepthPort     /clientDepthPort:i
 * localRpcPort       /clientRpcPort
 * remoteImagePort    /RGBD/rgbCamera:o
 * remoteDepthPort    /RGBD/depthCamera:o
 * remoteRpcPort      /RGBD/rpc
 * \endcode
 *
 * XML format, using 'networks' keyword. This file is meant to be used in junction with yarprobotinterface executable,
 * therefore has an addictional section at the end.
 *
 * \code{.xml}
 *  <!-- Following parameters are meaningful ONLY for yarprobotinterface -->
 *
 * <param name="localImagePort">    /clientRgbPort:i        </param>
 * <param name="localDepthPort">    /clientDepthPort:i      </param>
 * <param name="localRpcPort">      /clientRpcPort          </param>
 * <param name="remoteImagePort">   /RGBD/rgbCamera:o       </param>
 * <param name="localDepthPort">    /RGBD/depthCamera:o     </param>
 * <param name="remoteRpcPort">     /RGBD/rpc               </param>
 * \endcode
 *
 */

class RGBDSensorClient :
        public yarp::dev::DeviceDriver,
        public yarp::proto::framegrabber::FrameGrabberControls_Forwarder,
        public yarp::dev::IRGBDSensor
{
protected:
    yarp::os::Port rpcPort;
private:
    yarp::proto::framegrabber::RgbVisualParams_Forwarder* RgbMsgSender{nullptr};
    yarp::proto::framegrabber::DepthVisualParams_Forwarder* DepthMsgSender{nullptr};
protected:
    std::string local_colorFrame_StreamingPort_name;
    std::string local_depthFrame_StreamingPort_name;
    std::string remote_colorFrame_StreamingPort_name;
    std::string remote_depthFrame_StreamingPort_name;
    std::string image_carrier_type;
    std::string depth_carrier_type;

    RgbImageBufferedPort   colorFrame_StreamingPort;
    FloatImageBufferedPort depthFrame_StreamingPort;

    // Use a single RPC port for now
    std::string local_rpcPort_name;
    std::string remote_rpcPort_name;

    /*
     * In case the client has to connect to 2 different wrappers/server because the rgb
     * and depth comes from two different sources.
     *
     * It should be possible to attach this guy to more than one port, try to see what
     * will happen when receiving 2 calls at the same time (receive one calls while serving
     * another one, it will result in concurrent thread most probably) and buffering issues.
     *

        std::string local_colorFrame_rpcPort_Name;
        std::string local_depthFrame_rpcPort_Name;
        std::string remote_colorFrame_rpcPort_Name;
        std::string remote_depthFrame_rpcPort_Name;

        yarp::os::Port colorFrame_rpcPort;
        yarp::os::Port depthFrame_rpcPort;
    */

    // Image data specs
    std::string sensorId;
    yarp::dev::IRGBDSensor *sensor_p{nullptr};
    IRGBDSensor::RGBDSensor_status sensorStatus{IRGBDSensor::RGBD_SENSOR_NOT_READY};
    int verbose{2};

    bool initialize_YARP(yarp::os::Searchable &config);

    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;


    // This is gonna be superseded by the synchronized when it'll be ready
    RGBDSensor_StreamingMsgParser *streamingReader{nullptr};
    bool fromConfig(yarp::os::Searchable &config);

public:
    RGBDSensorClient();
    RGBDSensorClient(const RGBDSensorClient&) = delete;
    RGBDSensorClient(RGBDSensorClient&&) = delete;
    RGBDSensorClient& operator=(const RGBDSensorClient&) = delete;
    RGBDSensorClient& operator=(RGBDSensorClient&&) = delete;
    ~RGBDSensorClient() override;

    int  getRgbHeight() override;
    int  getRgbWidth() override;
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) override;
    bool getRgbResolution(int &width, int &height) override;
    bool setRgbResolution(int width, int height) override;
    bool getRgbFOV(double &horizontalFov, double &verticalFov) override;
    bool setRgbFOV(double horizontalFov, double verticalFov) override;
    bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) override;
    bool getRgbMirroring(bool& mirror) override;
    bool setRgbMirroring(bool mirror) override;

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
    int    getDepthHeight() override;
    int    getDepthWidth() override;
    bool   setDepthResolution(int width, int height) override;
    bool   getDepthFOV(double &horizontalFov, double &verticalFov) override;
    bool   setDepthFOV(double horizontalFov, double verticalFov) override;
    double getDepthAccuracy() override;
    bool   setDepthAccuracy(double accuracy) override;
    bool   getDepthClipPlanes(double &near, double &far) override;
    bool   setDepthClipPlanes(double near, double far) override;
    bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic) override;
    bool   getDepthMirroring(bool& mirror) override;
    bool   setDepthMirroring(bool mirror) override;

    // Device Driver interface //
    /**
     * Create and configure a device, by name.  The config
     * object should have a property called "device" that
     * is set to the common name of the device.  All other
     * properties are passed on the the device's
     * DeviceDriver::open method.
     *
     * @param config configuration options for the device
     *
     * @return  the device, if it could be created and configured,
     * otherwise NULL. The user is responsible for deallocating the
     * device.
     */
    bool open(yarp::os::Searchable& config) override;

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    bool close() override;

    /*
     *  IRgbVisualParams interface. Look at IVisualParams.h for documentation
     */

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters from the device
     * @param  extrinsic  return a rototranslation matrix describing the position
     *         of the depth optical frame with respect to the rgb frame
     * @return true if success
     */
    bool getExtrinsicParam(yarp::sig::Matrix &extrinsic) override;

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    IRGBDSensor::RGBDSensor_status getSensorStatus() override;

    /**
     * Return an error message in case of error. For debugging purpose and user notification.
     * Error message will be reset after any successful command
     * @return A string explaining the last error occurred.
     */
    std::string getLastErrorMsg(yarp::os::Stamp *timeStamp = nullptr) override;

    /**
     * Get the rgb frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choice. The conversion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = nullptr) override;

    /**
     * Get the depth frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choice. The conversion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = nullptr) override;

    /**
    * Get the both the color and depth frame in a single call. Implementation should assure the best possible synchronization
    * is achieved accordingly to synch policy set by the user.
    * TimeStamps are referred to acquisition time of the corresponding piece of information.
    * If the device is not providing TimeStamps, then 'timeStamp' field should be set to '-1'.
    * @param colorFrame pointer to FlexImage data to hold the color frame from the sensor
    * @param depthFrame pointer to FlexImage data to hold the depth frame from the sensor
    * @param colorStamp pointer to memory to hold the Stamp of the color frame
    * @param depthStamp pointer to memory to hold the Stamp of the depth frame
    * @return true if able to get both data.
    */
    bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=nullptr, yarp::os::Stamp *depthStamp=nullptr) override;


    // IFrame Grabber Control 2
    //
    // Implemented by FrameGrabberControls2_Forwarder
    //
    using FrameGrabberControls_Forwarder::getCameraDescription;
    using FrameGrabberControls_Forwarder::hasFeature;
    using FrameGrabberControls_Forwarder::setFeature;
    using FrameGrabberControls_Forwarder::getFeature;
    using FrameGrabberControls_Forwarder::hasOnOff;
    using FrameGrabberControls_Forwarder::setActive;
    using FrameGrabberControls_Forwarder::getActive;
    using FrameGrabberControls_Forwarder::hasAuto;
    using FrameGrabberControls_Forwarder::hasManual;
    using FrameGrabberControls_Forwarder::hasOnePush;
    using FrameGrabberControls_Forwarder::setMode;
    using FrameGrabberControls_Forwarder::getMode;
    using FrameGrabberControls_Forwarder::setOnePush;
};

#endif // YARP_DEV_RGBDSENSORCLIENT_RGBDSENSORCLIENT_H
