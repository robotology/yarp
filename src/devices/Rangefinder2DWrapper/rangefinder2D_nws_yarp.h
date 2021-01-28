/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_DEV_RANGEFINDER2D_NWS_YARP_H
#define YARP_DEV_RANGEFINDER2D_NWS_YARP_H

 //#include <list>
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

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/LaserScan2D.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/dev/api.h>
#include <yarp/dev/IPreciselyTimed.h>

#define DEFAULT_THREAD_PERIOD 0.02 //s

  /**
   *  @ingroup dev_impl_wrapper dev_impl_network_wrapper dev_impl_lidar
   *
   * \section rangefinder2D_nws_yarp_device_parameters Description of input parameters
   * A Network grabber for 2D Rangefinder devices.
   * This device will stream data on the specified YARP ports.
   *
   * This device is paired with its YARP client called Rangefinder2DClient to receive the data streams and perform RPC operations.
   *
   *   Parameters required by this device are:
   * | Parameter name | SubParameter            | Type    | Units          | Default Value | Required                       | Description                                                                                         | Notes |
   * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------: |:---------------------------------------------------------------------------------------------------:|:-----:|
   * | period         |      -                  | int     | ms             |   20          | No                             | refresh period of the broadcasted values in ms                                                      | default 20ms |
   * | name           |      -                  | string  | -              |   -           | Yes, unless useROS='only'      | Prefix name of the ports opened by the wrapper, e.g. /robotName/Rangefinder2DSensor                 | Required suffix like '/rpc' will be added by the device      |
   * | subdevice      |      -                  | string  | -              |   -           | alternative to 'attach' action | name of the subdevice to use as a data source                                                       | when used, parameters for the subdevice must be provided as well |
   * | frame_id       |      -                  | string  | -              |   -           | No                             | name of the attached frame                                                                          | Currently not used, reserved for future use                  |
   *
   * Example of configuration file using .ini format.
   *
   * \code{.unparsed}
   * device rangefinder2D_nws_yarp
   * subdevice <Rangefinder2DSensor>
   * period 20
   * name /<robotName>/Rangefinder2DSensor
   * \endcode
   */
class Rangefinder2D_nws_yarp :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PortReader
{
public:
    Rangefinder2D_nws_yarp();
    ~Rangefinder2D_nws_yarp();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    bool attachAll(const yarp::dev::PolyDriverList &p) override;
    bool detachAll() override;

    void attach(yarp::dev::IRangefinder2D *s);
    void detach();

    bool threadInit() override;
    void threadRelease() override;
    void run() override;
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    //yarp streaming data
    std::string streamingPortName;
    std::string rpcPortName;
    std::string frame_id;
    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::dev::LaserScan2D> streamingPort;

private:
    //interfaces
    yarp::dev::PolyDriver driver;
    yarp::dev::IRangefinder2D *sens_p;
    yarp::dev::IPreciselyTimed *iTimed;

private:
    //device data
    yarp::os::Stamp lastStateStamp;
    double _period;
    double minAngle, maxAngle;
    double minDistance, maxDistance;
    double resolution;
    bool   isDeviceOwned;

private:
    //private methods
    bool initialize_YARP(yarp::os::Searchable &config);
};

#endif //YARP_DEV_RANGEFINDER2D_NWS_YARP_H
