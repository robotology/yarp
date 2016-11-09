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
#include <yarp/dev/FrameGrabberControl2.h>

#define DEFAULT_THREAD_PERIOD       20    //ms
#define RGBDSENSOR_TIMEOUT_DEFAULT  100   //ms

// Following three definitions would fit better in a header file
// shared between client and server ... where to place it?
#define VOCAB_PROTOCOL_VERSION VOCAB('p', 'r', 'o', 't')
#define RGBD_WRAPPER_PROTOCOL_VERSION_MAJOR 1
#define RGBD_WRAPPER_PROTOCOL_VERSION_MINOR 0

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
 * | Parameter name  | SubParameter   | Type    | Units          | Default Value | Required                       | Description                                                                    | Notes |
 * |:---------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:------------------------------------------------------------------------------:|:-----:|
 * | localImagePort  |      -         | string  | -              |   -           | Yes, unless useROS='only'      | Full name of the local port to open, e.g. /myApp/image_camera                  | '/rpc' port will be added for remote operations      |
 * | localDepthPort  |      -         | string  | -              |   -           | Yes, unless useROS='only'      | Full name of the local port to open, e.g. /myApp/depth_camera                  | '/rpc' port will be added for remote operations      |
 * | remoteImagePort |      -         | string  | -              |   -           | Yes, unless useROS='only'      | Full name of the port to read color images from, e.g. /robotName/image_camera  | '/rpc' port will be added for remote operations      |
 * | remoteDepthPort |      -         | string  | -              |   -           | Yes, unless useROS='only'      | Full name of the port to read depth images from, e.g. /robotName/depth_camera  | '/rpc' port will be added for remote operations      |
 * | synchPolicy     |      -         | string  |  -             |  latest       | No                             | Choose the policy to use to synch color and depth frame together               | Values are 'latest', 'seqNum', 'time' (this may require an addictional parameter)|
 * | watchdog        |      -         | double  |  ms            |   -           | Yes                            | Verify refresh of data on ports whitin this time, otherwise throws an error    |  - |
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 * device RGBDSensorClient
 * localImagePort     /localImagePort
 * localDepthPort     /localDepthPort
 * remoteImagePort    /iCub/colorCamera
 * remoteDepthPort    /iCub/depthCamera
 * watchdog 200
 * \endcode
 *
 * XML format, using 'networks' keywork. This file is meant to be used in junction with yarprobotinterface executable,
 * therefore has an addictional section at the end.
 *
 * \code{.xml}
 *  <!-- Following parameters are meaningful ONLY for yarprobotinterface -->
 *
 * <param name="localImagePort">    /<robotName>/localImagePort    </param>
 * <param name="localDepthPort">    /<robotName>/localDepthPort    </param>
 * <param name="remoteImagePort">   /<robotName>/colorCamera       </param>
 * <param name="remoteDepthPort">   /<robotName>/depthCamera       </param>
 * <param name="watchdog">          200                            </param>
 * \endcode
 *
 */

class yarp::dev::RGBDSensorClient:  public DeviceDriver,
                                    public IRGBDSensor,
                                    public IFrameGrabberControls2
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::ConstString local_colorFrame_StreamingPort_Name;
    yarp::os::ConstString local_depthFrame_StreamingPort_Name;
    yarp::os::ConstString remote_colorFrame_StreamingPort_Name;
    yarp::os::ConstString remote_depthFrame_StreamingPort_Name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> colorFrame_StreamingPort;
    yarp::os::BufferedPort<yarp::sig::FlexImage> depthFrame_StreamingPort;

    yarp::os::ConstString local_colorFrame_rpcPort_Name;
    yarp::os::ConstString local_depthFrame_rpcPort_Name;
    yarp::os::ConstString remote_colorFrame_rpcPort_Name;
    yarp::os::ConstString remote_depthFrame_rpcPort_Name;
    yarp::os::Port colorFrame_rpcPort;
    yarp::os::Port depthFrame_rpcPort;

    // It should be possible to attach this guy to more than one port, try to see what
    // will happen when receiving 2 calls at the same time (receive one calls while serving
    // another one, it will result in concurrent thread most probably) and buffering issues.

    // Image data specs
    std::string sensorId;
    yarp::dev::IRGBDSensor *sensor_p;
    IRGBDSensor::RGBDSensor_status sensorStatus;
    int verbose;

    bool use_ROS;  // if false (default) read from YARP port, if true read from ROS topic instead (Both at the same time is not possible).
    bool initialize_YARP(yarp::os::Searchable &config);
    bool initialize_ROS(yarp::os::Searchable &config);

    // Synch
    double watchdog;
    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;
    RGBDSensor_StreamingMsgParser streamingReader;
    bool fromConfig(yarp::os::Searchable &config);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    RGBDSensorClient();

    ~RGBDSensorClient();


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
    int  getRgbHeight();
    int  getRgbWidth();
    bool setRgbResolution(int width, int height);
    bool getRgbFOV(double &horizontalFov, double &verticalFov);
    bool setRgbFOV(double horizontalFov, double verticalFov);
    bool getRgbIntrinsicParam(yarp::os::Property &intrinsic);
    bool getRgbSensorInfo(yarp::os::Property &info);

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
    int    getDepthHeight();
    int    getDepthWidth();
    bool   setDepthResolution(int width, int height);
    bool   getDepthFOV(double &horizontalFov, double &verticalFov);
    bool   setDepthFOV(double horizontalFov, double verticalFov);
    bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic);
    bool   getDepthSensorInfo(yarp::os::Property info);
    double getDepthAccuracy();
    bool   setDepthAccuracy(double accuracy);
    bool   getDepthClipPlanes(double &near, double &far);
    bool   setDepthClipPlanes(double near, double far);

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters of the rgb camera
     * @param  extrinsic  return a Property containing extrinsic parameters
     *       of the optical model of the camera.
     * @return true if success
     */
    bool getExtrinsicParam(yarp::os::Property &extrinsic);

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    RGBDSensor_status getSensorStatus();

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

    bool getSynchPolicy(SynchPolicy policy, yarp::os::Property params);

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

    virtual bool getCameraDescription(CameraDescriptor *camera);
    virtual bool hasFeature(int feature, bool *hasFeature);
    virtual bool setFeature(int feature, double value);
    virtual bool getFeature(int feature, double *value);
    virtual bool setFeature(int feature, double value1, double value2);
    virtual bool getFeature(int feature, double *value1, double *value2);
    virtual bool hasOnOff(int feature, bool *HasOnOff);
    virtual bool setActive(int feature, bool onoff);
    virtual bool getActive(int feature, bool *isActive);
    virtual bool hasAuto(int feature, bool *hasAuto);
    virtual bool hasManual(int feature, bool *hasManual);
    virtual bool hasOnePush(int feature, bool *hasOnePush);
    virtual bool setMode(int feature, FeatureMode mode);
    virtual bool getMode(int feature, FeatureMode *mode);
    virtual bool setOnePush(int feature);
};

#endif // YARP_DEV__RGBD_SENSOR_CLIENT_

