/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARD_NWS_ROS_H
#define YARP_DEV_CONTROLBOARD_NWS_ROS_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/os/PeriodicThread.h>

#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IAxisInfo.h>

#include <yarp/os/Stamp.h>
#include <yarp/sig/Vector.h>

#include <string>
#include <vector>

#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/rosmsg/sensor_msgs/JointState.h>


/**
 *  @ingroup dev_impl_nws_ros
 *
 * \brief `controlBoard_nws_ros`: A controlBoard network wrapper server for ROS.
 *
 * \section controlBoard_nws_ros_device_parameters Description of input parameters
 *
 *  Parameters required by this device are:
 * | Parameter name  | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:---------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | node_name       |      -         | string  | -              |   -           | Yes                         | set the name for ROS node                                         | must start with a leading '/' |
 * | topic_name      |      -         | string  | -              |   -           | Yes                         | set the name for ROS topic                                        | must start with a leading '/' |
 * | period          |      -         | double  | s              |   0.02        | No                          | refresh period of the broadcasted values in s                     | optional, default 20ms |
 * | subdevice       |      -         | string  | -              |   -           | No                          | name of the subdevice to instantiate                              | when used, parameters for the subdevice must be provided as well |
 *
 * ROS message type used is sensor_msgs/JointState.msg (http://docs.ros.org/api/sensor_msgs/html/msg/JointState.html)
 */

class ControlBoard_nws_ros :
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::WrapperSingle
{
private:
    yarp::rosmsg::sensor_msgs::JointState ros_struct;

    yarp::sig::Vector times; // time for each joint

    std::vector<std::string> jointNames; // name of the joints
    std::string nodeName;                // name of the rosNode
    std::string topicName;               // name of the rosTopic

    yarp::os::Node* node; // ROS node
    std::uint32_t counter {0}; // incremental counter in the ROS message

    yarp::os::PortWriterBuffer<yarp::rosmsg::sensor_msgs::JointState> outputState_buffer; // Buffer associated to the ROS topic
    yarp::os::Publisher<yarp::rosmsg::sensor_msgs::JointState> publisherPort;             // Dedicated ROS topic publisher

    static constexpr double default_period = 0.02; // s
    double period {default_period};

    yarp::os::Stamp time; // envelope to attach to the state port

    yarp::dev::DeviceDriver* subdevice_ptr{nullptr};
    bool subdevice_owned {true};
    size_t subdevice_joints {0};
    bool subdevice_ready = false;

    yarp::dev::IPositionControl* iPositionControl{nullptr};
    yarp::dev::IEncodersTimed* iEncodersTimed{nullptr};
    yarp::dev::ITorqueControl* iTorqueControl{nullptr};
    yarp::dev::IAxisInfo* iAxisInfo{nullptr};

    bool setDevice(yarp::dev::DeviceDriver* device, bool owned);
    bool openAndAttachSubDevice(yarp::os::Property& prop);

    void closeDevice();
    void closePorts();
    bool updateAxisName();

public:
    ControlBoard_nws_ros();
    ControlBoard_nws_ros(const ControlBoard_nws_ros&) = delete;
    ControlBoard_nws_ros(ControlBoard_nws_ros&&) = delete;
    ControlBoard_nws_ros& operator=(const ControlBoard_nws_ros&) = delete;
    ControlBoard_nws_ros& operator=(ControlBoard_nws_ros&&) = delete;
    ~ControlBoard_nws_ros() override = default;

    // yarp::dev::DeviceDriver
    bool close() override;
    bool open(yarp::os::Searchable& prop) override;

    // yarp::dev::WrapperSingle
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;

    // yarp::os::PeriodicThread
    void run() override;
};

#endif // YARP_DEV_CONTROLBOARD_NWS_ROS_H
