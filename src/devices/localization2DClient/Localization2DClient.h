/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
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

#ifndef YARP_DEV_LOCALIZATION2DCLIENT_H
#define YARP_DEV_LOCALIZATION2DCLIENT_H


#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Mutex.h>
#include <string>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Map2DLocation.h>
#include <yarp/dev/ILocalization2D.h>


/**
 *  @ingroup dev_impl_network_clients
 *
 * \section Localization2DClient
 * A device which allows a user application retrieve the current position of the robot in the world.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -   |   -           | Yes          | Full port name opened by the Localization2DClient device.                             |       |
 * | remote         |      -         | string  | -   |   -           | Yes          | Full port name of the port opened on the server side, to which the Localization2DClient connects to.                           | E.g.(https://github.com/robotology/navigation/src/localizationServer)    |
 */

class Localization2DClient :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ILocalization2D
{
protected:
    yarp::os::Mutex               m_mutex;
    yarp::os::Port                m_rpc_port_localization_server;
    std::string         m_local_name;
    std::string         m_remote_name;

public:
    /* DeviceDriver methods */
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    /* The following methods belong to ILocalization2D interface */
    bool   getCurrentPosition(yarp::dev::Map2DLocation &loc) override;
    bool   setInitialPose(const yarp::dev::Map2DLocation& loc) override;
    bool   getLocalizationStatus(yarp::dev::LocalizationStatusEnum& status) override;
    bool   getEstimatedPoses(std::vector<yarp::dev::Map2DLocation>& poses) override;
};

#endif // YARP_DEV_LOCALIZATION2DCLIENT_H
