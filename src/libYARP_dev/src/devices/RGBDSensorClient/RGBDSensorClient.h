/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef YARP_DEV_RGBD_SENSOR_CLIENT_
#define YARP_DEV_RGBD_SENSOR_CLIENT_


#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IVisualParamsImpl.h>
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/dev/FrameGrabberControl2Impl.h>

#define DEFAULT_THREAD_PERIOD       20    //ms
#define RGBDSENSOR_TIMEOUT_DEFAULT  100   //ms

#include "RGBDSensorClient_StreamingMsgParser.h"

namespace yarp {
    namespace dev {
        class RGBDSensorClient;
    }
}



/**
 *  @ingroup dev_impl_client
 *
 * \section RGBDSensorClient Description of input parameters
 * A Network client to receive data from kinect-like devices.
 * This device will read from two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its client called RGBDSensorWrapper to receive the data streams and perfor remote operations.
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
 * XML format, using 'networks' keywork. This file is meant to be used in junction with yarprobotinterface executable,
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

class yarp::dev::RGBDSensorClient:  public DeviceDriver,
                                    public FrameGrabberControls2_Sender,
                                    public IRGBDSensor
{

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::dev::Implement_RgbVisualParams_Sender*   RgbMsgSender;
    yarp::dev::Implement_DepthVisualParams_Sender* DepthMsgSender;
protected:
    yarp::os::ConstString local_colorFrame_StreamingPort_name;
    yarp::os::ConstString local_depthFrame_StreamingPort_name;
    yarp::os::ConstString remote_colorFrame_StreamingPort_name;
    yarp::os::ConstString remote_depthFrame_StreamingPort_name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> colorFrame_StreamingPort;
    yarp::os::BufferedPort<yarp::sig::ImageOf< yarp::sig::PixelFloat> > depthFrame_StreamingPort;

    // Use a single RPC port for now
    yarp::os::ConstString local_rpcPort_name;
    yarp::os::ConstString remote_rpcPort_name;
    yarp::os::Port        rpcPort;


    /*
     * In case the client has to connect to 2 different wrappers/server because the rgb
     * and depth comes from two different sources.
     *
     * It should be possible to attach this guy to more than one port, try to see what
     * will happen when receiving 2 calls at the same time (receive one calls while serving
     * another one, it will result in concurrent thread most probably) and buffering issues.
     *

        yarp::os::ConstString local_colorFrame_rpcPort_Name;
        yarp::os::ConstString local_depthFrame_rpcPort_Name;
        yarp::os::ConstString remote_colorFrame_rpcPort_Name;
        yarp::os::ConstString remote_depthFrame_rpcPort_Name;

        yarp::os::Port colorFrame_rpcPort;
        yarp::os::Port depthFrame_rpcPort;
    */

    // Image data specs
    std::string sensorId;
    yarp::dev::IRGBDSensor *sensor_p;
    IRGBDSensor::RGBDSensor_status sensorStatus;
    int verbose;

    bool use_ROS;  // if false (default) read from YARP port, if true read from ROS topic instead (Both at the same time is not possible).
    bool initialize_YARP(yarp::os::Searchable &config);
    bool initialize_ROS(yarp::os::Searchable &config);

    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;


    // This is gonna be superseded by the synchronized when it'll be ready
    RGBDSensor_StreamingMsgParser streamingReader;
    bool fromConfig(yarp::os::Searchable &config);

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    RGBDSensorClient();
    ~RGBDSensorClient();


    virtual int  getRgbHeight();
    virtual int  getRgbWidth();
    virtual bool setRgbResolution(int width, int height);
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov);
    virtual bool setRgbFOV(double horizontalFov, double verticalFov);
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic);

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
    virtual int    getDepthHeight();
    virtual int    getDepthWidth();
    virtual bool   setDepthResolution(int width, int height);
    virtual bool   getDepthFOV(double &horizontalFov, double &verticalFov);
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov);
    virtual double getDepthAccuracy();
    virtual bool   setDepthAccuracy(double accuracy);
    virtual bool   getDepthClipPlanes(double &near, double &far);
    virtual bool   setDepthClipPlanes(double near, double far);
    virtual bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic);
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
    bool open(yarp::os::Searchable& config);

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    bool close();

    /*
     *  IRgbVisualParams interface. Look at IVisualParams.h for documentation
     */

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters ofrom the device
     * @param  extrinsic  return a rototranslation matrix describing the position
     *         of the depth optical frame with respect to the rgb frame
     * @return true if success
     */
    bool getExtrinsicParam(yarp::sig::Matrix &extrinsic);

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    IRGBDSensor::RGBDSensor_status getSensorStatus();

    /**
     * Return an error message in case of error. For debugging purpose and user notification.
     * Error message will be reset after any succesful command
     * @return A string explaining the last error occurred.
     */
    yarp::os::ConstString getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL);

    /**
     * Get the rgb frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL);

    /**
     * Get the depth frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = NULL);

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
    bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL);


    // IFrame Grabber Control 2
    //
    // Implemented by FrameGrabberControls2_Sender
    //
    using FrameGrabberControls2_Sender::getCameraDescription;
    using FrameGrabberControls2_Sender::hasFeature;
    using FrameGrabberControls2_Sender::setFeature;
    using FrameGrabberControls2_Sender::getFeature;
    using FrameGrabberControls2_Sender::hasOnOff;
    using FrameGrabberControls2_Sender::setActive;
    using FrameGrabberControls2_Sender::getActive;
    using FrameGrabberControls2_Sender::hasAuto;
    using FrameGrabberControls2_Sender::hasManual;
    using FrameGrabberControls2_Sender::hasOnePush;
    using FrameGrabberControls2_Sender::setMode;
    using FrameGrabberControls2_Sender::getMode;
    using FrameGrabberControls2_Sender::setOnePush;
};

#endif // YARP_DEV__RGBD_SENSOR_CLIENT_

