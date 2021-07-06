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

#ifndef YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
#define YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/math/FrameTransform.h>

#define DEFAULT_THREAD_PERIOD 0.02 //s

/**
* @ingroup dev_impl_network_wrapper
 *
 * \brief `FrameTransformServer`: Documentation to be added
 */
class FrameTransformServer :
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader
{
public:
    FrameTransformServer();
    ~FrameTransformServer();

    bool open(yarp::os::Searchable &params) override;
    bool close() override;

    bool threadInit() override;
    void threadRelease() override;
    void run() override;

private:
    std::mutex              m_mutex;
    std::string             m_streamingPortName;
    std::string             m_rpcPortName;
    yarp::os::Stamp         m_lastStateStamp;
    double                  m_period;
    double                  m_FrameTransformTimeout;

    yarp::os::RpcServer                      m_rpcPort;
    yarp::os::BufferedPort<yarp::os::Bottle> m_streamingPort;

    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_FRAMETRANSFORMSERVER_FRAMETRANSFORMSERVER_H
