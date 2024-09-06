/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

#include <yarp/sig/LaserScan2D.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/api.h>
#include "Rangefinder2D_nws_yarp_ParamsParser.h"
#include "Rangefinder2DServerImpl.h"

#define DEFAULT_THREAD_PERIOD 0.02 //s

  /**
   *  @ingroup dev_impl_nws_yarp dev_impl_lidar
   *
   * \brief `rangefinder2D_nws_yarp`: A Network grabber for 2D Rangefinder devices.
   * This device will stream data on the specified YARP ports.
   *
   * This device is paired with its YARP client called rangefinder2D_nwc_yarp to receive the data streams and perform RPC operations.
   *
   * Parameters required by this device are shown in class: Rangefinder2D_nws_yarp_ParamsParser
   */
class Rangefinder2D_nws_yarp :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public Rangefinder2D_nws_yarp_ParamsParser
{
public:
    Rangefinder2D_nws_yarp();
    ~Rangefinder2D_nws_yarp();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    void attach(yarp::dev::IRangefinder2D *s);
    bool attach(yarp::dev::PolyDriver* driver) override;
    bool detach() override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;
    bool read(yarp::os::ConnectionReader& connection) override;

private:
    // thrift
    IRangefinder2DRPCd m_RPC;

    yarp::os::Port rpcPort;
    yarp::os::BufferedPort<yarp::sig::LaserScan2D> streamingPort;

    //interfaces
    yarp::dev::IRangefinder2D *sens_p=nullptr;

    //device data
    yarp::os::Stamp lastStateStamp;
    double minAngle, maxAngle;
    double minDistance, maxDistance;
    double resolution;
};

#endif //YARP_DEV_RANGEFINDER2D_NWS_YARP_H
