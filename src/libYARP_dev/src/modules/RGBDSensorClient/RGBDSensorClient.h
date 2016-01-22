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

#ifndef _RGBD_SENSOR_CLIENT_
#define _RGBD_SENSOR_CLIENT_


#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PreciselyTimed.h>

#define DEFAULT_THREAD_PERIOD       20    //ms
#define RGBDSENSOR_TIMEOUT_DEFAULT  100   //ms

// Following three definitions would fit better in a header file
// shared between client and server ... where to place it?
#define VOCAB_PROTOCOL_VERSION VOCAB('p', 'r', 'o', 't')
#define RGBD_WRAPPER_PROTOCOL_VERSION_MAJOR 1
#define RGBD_WRAPPER_PROTOCOL_VERSION_MINOR 0

#include <RGBDSensor_StreamingMsgParser.h>

namespace yarp {
    namespace dev {
        class RGBDSensorClient;
    }
}



/**
 *  @ingroup dev_impl_wrapper
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
 * | localImagePort  |      -         | string  | -              |   -           | Yes, unless useROS='true'      | Full name of the local port to open, e.g. /myApp/image_camera                  | '/rpc' port will be added for remote operations      |
 * | localDepthPort  |      -         | string  | -              |   -           | Yes, unless useROS='true'      | Full name of the local port to open, e.g. /myApp/depth_camera                  | '/rpc' port will be added for remote operations      |
 * | remoteImagePort |      -         | string  | -              |   -           | Yes, unless useROS='true'      | Full name of the port to read color images from, e.g. /robotName/image_camera  | '/rpc' port will be added for remote operations      |
 * | remoteDepthPort |      -         | string  | -              |   -           | Yes, unless useROS='true'      | Full name of the port to read depth images from, e.g. /robotName/depth_camera  | '/rpc' port will be added for remote operations      |
 * | ROS             |      -         | group   | -              |   -           | No                             | Group containing parameter for ROS topic initialization                        | if missing, it is assumed to not use ROS topics |
 * |   -             |  useROS        | string  | true/false     |   -           |  alternative to YARP           | Set 'true' to read from ROS topic instead of YARP port                         |  -  |
 * |   -             |  imageTopic    | string  |  -             |   -           |  alternative to imagePort      | Set the name for ROS image topic                                               |  -  |
 * |   -             |  depthTopic    | string  |  -             |   -           |  alternative to depthPort      | Set the name for ROS depth topic                                               |  -  |
 * |   -             |  nodeName      | string  |  -             |   -           |  if ROS group is present       | Set the name for the equivalent ROS node                                       |  -  |
 * | synchPolicy     |      -         | string  |  -             |  latest       | No                             | Choose the policy to use to synch color and depth frame together               | Values are 'latest', 'seqNum', 'time' (this may require an addictional parameter)|
 * | watchdog        |      -         | double  |  ms            |   -           | Yes                            | Verify refresh of data on ports whitin this time, otherwise throws an error    |  - |
 *
 * ROS message type used is sensor_msgs/Image.msg ( http://docs.ros.org/api/sensor_msgs/html/msg/Image.html)
 * Some example of configuration files:
 *
 * Configuration file using .ini format, using subdevice keyword.
 *
 * \code{.unparsed}
 *  device controlboardwrapper2
 *  subdevice fakebot
 *  name /icub/head
 *
 * ** parameter for 'fakebot' subdevice follows here **
 * ...
 * \endcode
 *
 * XML format, using 'networks' keywork. This file is meant to be used in junction with robotInterface executable,
 * therefore has an addictional section at the end.
 *
 * \code{.xml}
 *  <!-- Following parameters are meaningful ONLY for robotInterface -->
 *
 * <param name="period"> 20                 </param>
 * <param name="name">   /icub/left_arm     </param>
 *
 *  <action phase="startup" level="5" type="attach">
 *      <paramlist name="networks">
 *          <!-- The param value must match the device name in the corresponding emsX file -->
 *          <elem name="imageSource">  left_upper_arm_mc </elem>
 *          <elem name="depthSource">  left_lower_arm_mc </elem>
 *       </paramlist>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 * \endcode
 *
 * Configuration file using .ini format, using network keyword
 *
 * \code{.unparsed}
 *  device controlboardwrapper2
 *  name  /robotName/partName
 *  period 10

 * \endcode
 *
 * Configuration for ROS topic using .ini format
 * \code{.unparsed}
 * [ROS]
 * useROS         true
 * ROS_topicName  /JointState
 * ROS_nodeName   /robotPublisher
 * \endcode
 *
 * Configuration for ROS topic using .xml format
 * \code{.unparsed}
 * <group name="ROS">
 *     <param name="useROS">         true             </param>    // use 'only' if you want only ROS topic and NOT yarp port
 *     <param name="ROS_topicName">  /JointState      </param>
 *     <param name="ROS_nodeName">   /robotPublisher  </param>
 * </group>
 * \endcode
 */

class yarp::dev::RGBDSensorClient:  public DeviceDriver,
                                    public IRGBDSensor
{
#ifndef DOXYGEN_SHOULD_SKIP_THIS
protected:
    yarp::os::ConstString local_colorFrame_StreamingPort_Name;
    yarp::os::ConstString local_depthFrame_StreamingPort_Name;
    yarp::os::ConstString remote_colorFrame_StreamingPort_Name;
    yarp::os::ConstString remote_depthFrame_StreamingPort_Name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> colorFrame_StreamingPort;
    yarp::os::BufferedPort<yarp::sig::FlexImage> depthFrame_StreamingPort;
//     yarp::os::Port colorFrame_StreamingPort;
//     yarp::os::Port depthFrame_StreamingPort;

    yarp::os::ConstString local_colorFrame_rpcPort_Name;
    yarp::os::ConstString local_depthFrame_rpcPort_Name;
    yarp::os::ConstString remote_colorFrame_rpcPort_Name;
    yarp::os::ConstString remote_depthFrame_rpcPort_Name;
    yarp::os::Port colorFrame_rpcPort;
    yarp::os::Port depthFrame_rpcPort;

    // It should be possible to attach this guy to more than one port, try to see what
    // will happen when receiving 2 calls at the same time (receive one calls while serving
    // another one, it will result in concurrent thread most probably) and buffering issues.
//     sensor::depth::RGBDSensor_RPCMgsParser RPC_parser;

    // Image data specs
    int hDim, vDim;
    int _rate;
    std::string sensorId;
    yarp::dev::IRGBDSensor *sensor_p;
    IRGBDSensor::RGBDSensor_status _sensorStatus;
    yarp::dev::IDepthSensor::VerboseLevel verbose;

    bool use_ROS;  // if false (default) read from YARP port, if true read from ROS topic instead (Both at the same time is not possible).
    bool initialize_YARP(yarp::os::Searchable &config);
    bool initialize_ROS(yarp::os::Searchable &config);
//     bool read(yarp::os::ConnectionReader& connection);

    // Synch
    double watchdog;
    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;
//     RGBDSensor_StreamingMsgParser<yarp::sig::FlexImage, yarp::sig::FlexImage> reader;
    RGBDSensor_StreamingMsgParser streamingReader;
    bool fromConfig(yarp::os::Searchable &config);
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

public:

    RGBDSensorClient();
    ~RGBDSensorClient();

    //
    // Device Driver interface
    //
    virtual bool open(yarp::os::Searchable& config);

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close();

    // IFrameGrabber Interfaces
    /**
     * Get an rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    /**
     * Get a raw image from the frame grabber
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);

    /**
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const;

    /**
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const;


    //
    //  IDepth Interface
    //
    /**
    * get the device hardware charactestics
    * @param device_info Searchable struct containing the device info
    * @return true if able to get information about the device.
    */
    virtual bool getDeviceInfo(yarp::os::Searchable &device_info);

    /**
    * Get the distance measurements as an image
    * @param ranges the vector containing the distance measurement
    * @return true if able to get measurement data.
    */
    virtual bool getMeasurementData(yarp::sig::FlexImage &image, yarp::os::Stamp *stamp=NULL);

    /**
    * get the device status
    * @param status the device status
    * @return true/false.
    */
    virtual bool getDeviceStatus(DepthSensor_status& status);

    /**
    * get the device detection range
    * @param min the minimum detection distance from the sensor [meter]
    * @param max the maximum detection distance from the sensor [meter]
    * @return true if able to get required info.
    */
    virtual bool getDistanceRange(double& min, double& max);

    /**
    * set the device detection range. Invalid setting will be discarded.
    * @param min the minimum detection distance from the sensor [meter]
    * @param max the maximum detection distance from the sensor [meter]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setDistanceRange(double min, double max);

    /**
    * get the horizontal scan limits / field of view with respect to the
    * front line of sight of the sensor. Angles are measured around the
    * positive Z axis (counterclockwise, if Z is up) with zero angle being
    * forward along the x axis
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if able to get required info.
    */
    virtual bool getHorizontalScanLimits(double& min, double& max);

    /**
    * set the horizontal scan limits / field of view with respect to the
    * front line of sight of the sensor. Angles are measured around the
    * positive Z axis (counterclockwise, if Z is up) with zero angle being
    * forward along the x axis
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setHorizontalScanLimits(double min, double max);

    /**
    * get the vertical scan limits / field of view with respect to the
    * front line of sight of the sensor   [degrees]
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if able to get required info.
    */
    virtual bool getverticalScanLimits(double& min, double& max);

    /**
    * set the vertical scan limits / field of view with respect to the
    * front line of sight of the sensor   [degrees]
    * @param min start angle of the scan  [degrees]
    * @param max end angle of the scan    [degrees]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setverticalScanLimits(double min, double max);

    /**
    * get the size of measured data from the device.
    * It can be WxH for camera-like devices, or the number of points for other devices.
    * @param horizontal width of image,  number of points in the horizontal scan [num]
    * @param vertical   height of image, number of points in the vertical scan [num]
    * @return true if able to get required info.
    */
    virtual bool getDataSize(double& horizontal, double &vertical);

    /**
    * set the size of measured data from the device.
    * It can be WxH for camera-like devices, or the number of points for other devices.
    * @param horizontal width of image,  number of points in the horizontal scan [num]
    * @param vertical   height of image, number of points in the vertical scan [num]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setDataSize(double& horizontal, double &vertical);

    /**
    * get the device resolution, using the current settings of scan limits
    * and data size. Will return the resolution of device at 1 meter distance.
    * @param hRes horizontal resolution [meter]
    * @param vRes vertical resolution [meter]
    * @return true if able to get required info.
    */
    virtual bool getResolution(double& hRes, double &vRes);

    /**
    * set the device resolution.
    * This call can change the current settings of scan limits, data size or scan rate
    * to match the requested resolution.
    * Verify those settings is suggested after this call.
    * Will set the resolution of device at 1meter distance, if possible.
    * @param hRes horizontal resolution [meter]
    * @param vRes vertical resolution [meter]
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setResolution(double& hRes, double &vRes);

    /**
    * get the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true if able to get required info.
    */
    virtual bool getScanRate(double& rate);

    /**
    * set the scan rate (scans per seconds)
    * @param rate the scan rate
    * @return true if message was correctly delivered to the HW device.
    */
    virtual bool setScanRate(double rate);

 /** IRGBDSensor specific interface methods*/
    virtual bool getRGBDSensor_Status(RGBDSensor_status *status);

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
    virtual bool getRGBD_Frames(yarp::sig::FlexImage *colorFrame, yarp::sig::FlexImage *depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL);

};

#endif // _RGBD_SENSOR_CLIENT_

