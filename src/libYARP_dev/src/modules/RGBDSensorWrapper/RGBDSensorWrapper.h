// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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

#ifndef _DEPTH_SENSOR_WRAPPER_H_
#define _DEPTH_SENSOR_WRAPPER_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>

#include "RGBDSensorWrapper_RPCMsgParser.h"

namespace yarp{
    namespace dev{
        class RGBDSensorWrapper;
    }
}

#define DEFAULT_THREAD_PERIOD 30 //ms

// Following three definitions would fit better in a header file
// shared between client and server ... where to place it?
#define VOCAB_PROTOCOL_VERSION VOCAB('p', 'r', 'o', 't')
#define RGBD_WRAPPER_PROTOCOL_VERSION_MAJOR 1
#define RGBD_WRAPPER_PROTOCOL_VERSION_MINOR 0

/**
 *  @ingroup dev_impl_wrapper
 *
 * \section RGBDSensorWrapper Description of input parameters
 * A Network grabber for kinect-like devices.
 * This device will produce two streams of data through different ports, one for the color frame and the other one
 * for depth image following Framegrabber and IDepthSensor interfaces specification respectively.
 * See they documentation for more details about each interface.
 *
 * This device is paired with its client called RGBDSensorClient to receive the data streams and perform remote operations.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                       | Description                                                                    | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:------------------------------------------------------------------------------:|:-----:|
 * | period         |      -         | int     | ms             |   20          | No                             | refresh period of the broadcasted values in ms                                 | default 20ms |
 * | imagePort      |      -         | string  | -              |   -           | Yes, unless useROS='only'      | full name of the port for streaming color image, e.g. /robotName/image_camera  | '/rpc' port will be added for remote operations      |
 * | depthPort      |      -         | string  | -              |   -           | Yes, unless useROS='only'      | full name of the port for streaming depth image, e.g. /robotName/depth_camera  | '/rpc' port will be added for remote operations      |
 * | subdevice      |      -         | string  | -              |   -           | alternative to 'attach' action | name of the subdevice to use as a data source                                  | when used, parameters for the subdevice must be provided as well |
 *
 * ROS message type used is sensor_msgs/Image.msg ( http://docs.ros.org/api/sensor_msgs/html/msg/Image.html)
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device RGBDSensorWrapper
 * subdevice RGBDsensor
 * period 30
 * imagePort /<robotName>/colorCamera
 * depthPort /<robotName>/depthCamera
 * \endcode
 */

class yarp::dev::RGBDSensorWrapper: public yarp::dev::DeviceDriver,
                                    public yarp::dev::IWrapper,
                                    public yarp::dev::IMultipleWrapper,
                                    public yarp::os::RateThread
{
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    yarp::os::ConstString colorFrame_StreamingPort_Name;
    yarp::os::ConstString depthFrame_StreamingPort_Name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> colorFrame_StreamingPort;
    yarp::os::BufferedPort<yarp::sig::FlexImage> depthFrame_StreamingPort;

    yarp::os::Port colorFrame_rpcPort;
    yarp::os::Port depthFrame_rpcPort;
    yarp::os::ConstString colorFrame_rpcPort_Name;
    yarp::os::ConstString depthFrame_rpcPort_Name;
    // It should be possible to attach this guy to more than one port, try to see what
    // will happen when receiving 2 calls at the same time (receive one calls while serving
    // another one, it will result in concurrent thread most probably) and buffering issues.
    sensor::depth::RGBDSensor_RPCMgsParser RPC_parser;

    // Image data specs
//     int hDim, vDim;
    int rate;
    std::string sensorId;
    yarp::dev::IRGBDSensor *sensor_p;
    IRGBDSensor::RGBDSensor_status sensorStatus;
    int verbose;

    bool use_YARP;
    bool use_ROS;
    bool initialize_YARP(yarp::os::Searchable &config);
    bool initialize_ROS(yarp::os::Searchable &config);
    bool read(yarp::os::ConnectionReader& connection);

    // Open the wrapper only, the attach method needs to be called before using it
    // Typical usage: robotInterface
    bool openDeferredAttach(yarp::os::Searchable& prop);

    // If a subdevice parameter is given, the wrapper will open it and attach to immediatly.
    // Typical usage: simulator or command line
    bool isSubdeviceOwned;
    yarp::dev::PolyDriver *subDeviceOwned;
    bool openAndAttachSubDevice(yarp::os::Searchable& prop);

    // Synch
    yarp::os::Stamp colorStamp;
    yarp::os::Stamp depthStamp;

#endif //DOXYGEN_SHOULD_SKIP_THIS

public:
    RGBDSensorWrapper();
    ~RGBDSensorWrapper();

    bool open(yarp::os::Searchable &params);
    bool fromConfig(yarp::os::Searchable &params);
    bool close();

    void setId(const std::string &id);
    std::string getId();

    /**
      * Specify which sensor this thread has to read from.
      */
    bool attachAll(const PolyDriverList &p);
    bool detachAll();

    bool attach(PolyDriver *poly);
    bool attach(yarp::dev::IRGBDSensor *s);
    bool detach();

    bool threadInit();
    void threadRelease();
    void run();
};

#endif   // _DEPTH_SENSOR_WRAPPER_H_
