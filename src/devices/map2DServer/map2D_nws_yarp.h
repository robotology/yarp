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

#ifndef YARP_DEV_MAP2D_NWS_YARP
#define YARP_DEV_MAP2D_NWS_YARP

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Property.h>

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/RpcServer.h>
#include <yarp/sig/Vector.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/MapGrid2D.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/Map2DPath.h>
#include <yarp/dev/IMultipleWrapper.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/api.h>

/**
 *  @ingroup dev_impl_wrapper dev_impl_navigation
 *
 * \section Map2D_nws_yarp
 *
 * \brief `Map2D_nws_yarp`: A device capable of read/save collections of maps from disk, and make them accessible to any Map2DClient device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value    | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:----------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | name           |      -         | string  | -              | /map2D_nws_yarp/rpc   | No           | Full name of the rpc port opened by the Map2DServer device.       |       |

 */

class Map2D_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::PortReader,
        public yarp::dev::IMultipleWrapper
{
public:
    Map2D_nws_yarp();
    ~Map2D_nws_yarp();
    bool open(yarp::os::Searchable& params) override;
    bool close() override;
    bool detachAll() override;
    bool attachAll(const yarp::dev::PolyDriverList& l) override;

private:
    //drivers and interfaces
    yarp::dev::Nav2D::IMap2D*    m_iMap2D = nullptr;
    yarp::dev::PolyDriver        m_drv;

private:
    std::mutex                   m_mutex;
    std::string                  m_rpcPortName;
    yarp::os::RpcServer          m_rpcPort;
    bool                         m_send_maps_compressed=false;

    bool read(yarp::os::ConnectionReader& connection) override;

    void parse_string_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
    void parse_vocab_command(yarp::os::Bottle& in, yarp::os::Bottle& out);
};

#endif // YARP_DEV_MAP2D_NWS_YARP
