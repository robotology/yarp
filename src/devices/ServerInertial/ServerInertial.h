/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERVERINERTIAL_SERVERINERTIAL_H
#define YARP_DEV_SERVERINERTIAL_SERVERINERTIAL_H

#include <cstdio>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

// ROS state publisher
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/Imu.h>
#include <yarp/rosmsg/impl/yarpRosHelper.h>


/**
 *  @ingroup dev_impl_wrapper
 *
 * \brief `inertial`: Export an inertial sensor.
 * The network interface is a single Port.
 * We will stream bottles with 12 floats:
 * \code{.unparsed}
 * 0  1   2  = Euler orientation data (X, Y, Z)  global frame representation.
 * 3  4   5  = Calibrated 3-axis (X, Y, Z) acceleration data
 * 6  7   8  = Calibrated 3-axis (X, Y, Z) gyroscope data
 * 9 10  11  = Calibrated 3-axis (X, Y, Z) magnetometer data
 * \endcode
 *
 *  It reads the data from an Inertial measurement unit sensor and sends them through yarp port.
 *
 * \section inertial_device_parameters Description of input parameters
 *
 * Parameters accepted in the config argument of the open method:
 * | Parameter name | Type    | Units          | Default Value | Required  | Description   | Notes |
 * |:--------------:|:------: |:--------------:|:-------------:|:--------: |:-------------:|:-----:|
 * | name           | string  |  -             |   -           | Yes       | full name of the port opened by the device, like /robotName/deviceId/sensorType:o | must start with a '/' character |
 * | period         | double     |  s             |   0.005       | No        | refresh period of the broadcasted values in ms (optional, default 5ms) | - |
 * | subdevice      | string  |  -             |   -           | alternative to attach action| name of the yarp IMU device driver to be instantiated | if using yarprobotinterface or custom program the 'attach' action can be used instead |
 * | ROS            | group   |  -             |   -           | No                          | Group containing parameter for ROS topic initialization           | if missing, it is assumed to not use ROS topics |
 * |  useROS        | string  | true/false/only|   -           |  if ROS group is present    | set 'true' to have both yarp ports and ROS topic, set 'only' to have only ROS topic and no yarp port|  - |
 * |  ROS_TopicName | string  |  -             |   -           |  if ROS group is present    | set the name for ROS topic                                        | must start with a leading '/' |
 * |  ROS_nodeName  | string  |  -             |   -           |  if ROS group is present    | set the name for ROS node                                         | must start with a leading '/' |
 * |  ROS_msgType   | string  |  enum          |   -           |  if ROS group is present    | choose the message to be sent through ROS topic                   | supported value now is ONLY geometry_msgs/WrenchedStamped |
 * |  frame_id      | string  |  -             |   -           |  if ROS group is present    | name of reference frame the measures are referred to              | - |
 *
 *  ROS message type used is sensor_msgs/Imu.msg (http://docs.ros.org/api/sensor_msgs/html/msg/Imu.html)
 *
 * Some example of configuration files:
 *
 * Configuration file using .ini format
 *
 * \code{.unparsed}
 * name         /inertial
 * period       0.02
 * [ROS]
 * useROS        true
 * ROS_topicName /ROSinertial
 * ROS_nodeName  /IMUPublisher
 * frame_id      r_shoulder
 * \endcode
 *
 * Configuration file using .xml format.
 *
 * \code{.unparsed}
 *
 * <param name="name">              /inertial                   </param>
 * <param name="period">            0.02                          </param>
 * <group name="ROS">
 *     <param name="useROS">         true                       </param>    // use 'only' if you want only ROS topic and NOT yarp ports
 *     <param name="ROS_topicName">  /ROSinertial               </param>
 *     <param name="ROS_nodeName">   /IMUPublisher              </param>
 *     <param name="frame_id">       r_shoulder                 </param>
 * </group>
 *
 *  <!-- Following parameters are meaningful ONLY for yarprobotinterface -->
 *  <action phase="startup" level="5" type="attach">
 *      <paramlist name="networks">
 *          <!-- The param value must match the device name in the corresponding device configuration file -->
 *          <elem name="imu_device">  left_upper_arm_mc </elem>
 *       </paramlist>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 *
 *
 * \endcode
 *
 *  ROS message type used is sensor_msgs/Imu.msg
 */

class ServerInertial :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
        private yarp::os::Thread,
        public yarp::os::PortReader,
        public yarp::dev::IGenericSensor
{
private:
    bool spoke;
    std::string partName;
    yarp::dev::PolyDriver *IMU_polydriver;
    yarp::dev::IGenericSensor *IMU; //The inertial device
    yarp::dev::IPreciselyTimed *iTimed;
    double period;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::os::Bottle> writer;
    int prev_timestamp_counter;
    int curr_timestamp_counter;
    int trap;
    bool strict;

    // ROS data
    ROSTopicUsageType                                   useROS;                     // decide if open ROS topic or not
    std::string                                         frame_id;                   // name of the frame measures are referred to
    std::string                                         rosNodeName;                // name of the rosNode
    std::string                                         rosTopicName;               // name of the rosTopic
    yarp::os::Node                                      *rosNode;                   // add a ROS node
    yarp::os::NetUint32                                 rosMsgCounter;              // incremental counter in the ROS message
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::Imu> rosPublisherPort;           // Dedicated ROS topic publisher
    std::vector<yarp::os::NetFloat64>                   covariance;                 // empty matrix to store covariance data needed by ROS msg

    bool checkROSParams(yarp::os::Searchable &config);
    bool initialize_ROS();
    bool initialize_YARP(yarp::os::Searchable &prop);

public:
    ServerInertial();
    ServerInertial(const ServerInertial&) = delete;
    ServerInertial(ServerInertial&&) = delete;
    ServerInertial& operator=(const ServerInertial&) = delete;
    ServerInertial& operator=(ServerInertial&&) = delete;
    ~ServerInertial() override;

    /**
     * Open the device driver.
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;

    bool close() override;

    virtual bool getInertial(yarp::os::Bottle &bot);

    void run() override;

    bool read(yarp::os::ConnectionReader& connection) override;

    bool read(yarp::sig::Vector &out) override;

    bool getChannels(int *nc) override;

    bool calibrate(int ch, double v) override;

    /**    IWrapper interface
     * Attach to another object.
     * @param poly the polydriver that you want to attach to.
     * @return true/false on success failure.
     */
    bool attach(yarp::dev::PolyDriver *poly) override;
    bool detach() override;

    /**   IMultipleWrapper interface
     * Attach to a list of objects.
     * @param p the polydriver list that you want to attach to.
     * @return true/false on success failure.
     */
    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

private:

    bool ownDevices;
    yarp::dev::PolyDriver *subDeviceOwned;

    // Open the wrapper only, the attach method needs to be called before using it
    bool openDeferredAttach(yarp::os::Property& prop);

    // If a subdevice parameter is given to the wrapper, it will open it as well
    // and attach to it immediately.
    bool openAndAttachSubDevice(yarp::os::Property& prop);
};


#endif // YARP_DEV_SERVERINERTIAL_SERVERINERTIAL_H
